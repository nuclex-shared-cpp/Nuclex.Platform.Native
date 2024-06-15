#pragma region Apache License 2.0
/*
Nuclex Native Framework
Copyright (C) 2002-2024 Markus Ewald / Nuclex Development Labs

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma endregion // Apache License 2.0

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "./ResourceBudget.h"

#include "Nuclex/Platform/Tasks/TaskEnvironment.h" // for TaskEnvironment
#include "Nuclex/Platform/Tasks/ResourceManifest.h" // for ResourceManifest

#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Constructs new array of usable resources and initializes it</summary>
  /// <typeparam name="TUsableResource">
  ///   The <see cref="StandardTaskCoordinate.UsableResource" /> type, always
  /// </typeparam>
  /// <typeparam name="Count">Number of resource types that have been defined</typeparam>
  /// <returns>An array of usable resources, initializes to sane defaults</returns>
  template<typename TUsableResource, std::size_t Count>
  constexpr std::array<TUsableResource, Count> makeDefaultResourceArray() {
    std::array<TUsableResource, Count> resources;
    resources.fill(TUsableResource { 0, 0, nullptr, nullptr });
    return resources;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Calculates the total number of units across all resource types</summary>
  /// <typeparam name="TUsableResource">
  ///   The <see cref="StandardTaskCoordinate.UsableResource" /> type, always
  /// </typeparam>
  /// <typeparam name="Count">Number of resource types that have been defined</typeparam>
  /// <param name="resource">Resource array whose units will be counted</param>
  /// <returns>The total number of units in the resource array</returns>
  template<typename TUsableResource, std::size_t Count>
  std::size_t getTotalUnitCount(const std::array<TUsableResource, Count> &resources) {
    std::size_t totalUnitCount = 0;
    for(std::size_t index = 0; index < Count; ++index) {
      totalUnitCount += resources[index].UnitCount;
    }

    return totalUnitCount;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Returns the offset, in bytes, of the atomic counters for remaining resources
  /// </summary>
  /// <typeparam name="TUsableResource">
  ///   The <see cref="StandardTaskCoordinate.UsableResource" /> type, always
  /// </typeparam>
  /// <typeparam name="Count">Number of resource types that have been defined</typeparam>
  /// <param name="totalUnitCount">Total number of units across all resources</param>
  /// <returns>The offset, in bytes, of the atomic remaining resource counters</returns>
  constexpr std::size_t getAtomicsOffset(std::size_t totalUnitCount) {
    std::size_t atomicsOffset = sizeof(std::size_t[2]) * totalUnitCount / 2;
    return atomicsOffset + (
      ((atomicsOffset % alignof(std::atomic_size_t)) == 0) ?
      0 : (alignof(std::atomic_size_t) - atomicsOffset)
    );
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Allocates memory for all resource units in a resource array</summary>
  /// <typeparam name="TUsableResource">
  ///   The <see cref="StandardTaskCoordinate.UsableResource" /> type, always
  /// </typeparam>
  /// <typeparam name="Count">Number of resource types that have been defined</typeparam>
  /// <param name="resources">
  ///   Resource array with pre-filled UnitCount fields, will be modified to receive
  ///   pointers covering the total and remaining resource counts of each unit
  /// </param>
  /// <returns>A pointer to the allocated memory block which later needs to be freed</returns>
  /// <remarks>
  ///   This is very specialized method used internally by the resource budget class
  ///   to avoid allocating lots of tiny fragmented blocks.
  /// </remarks>
  template<typename TUsableResource, std::size_t Count>
  void freeUsableResourceInventory(
    std::uint8_t *allocatedMemoryBlock, std::array<TUsableResource, Count> &resources
  ) {
    if(allocatedMemoryBlock != nullptr) {
      for(std::size_t index = 0; index < Count; ++index) {
        std::size_t unitCount = resources[index].UnitCount;
        if(unitCount >= 1) {
          for(std::size_t unitIndex = 0; unitIndex < unitCount; ++unitIndex) {
            resources[index].Remaining[unitIndex].~atomic();
          }
        }
      }

      delete[] allocatedMemoryBlock;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Checks whether two arrays have identical unit counts on all resources</summary>
  /// <typeparam name="TUsableResource">
  ///   The <see cref="StandardTaskCoordinate.UsableResource" /> type, always
  /// </typeparam>
  /// <typeparam name="Count">Number of resource types that have been defined</typeparam>
  /// <param name="first">First resource array that will be compared</param>
  /// <param name="second">Second resource array that will be compared</param>
  /// <returns>True if both resource arrays have identical unit counts</returns>
  template<typename TUsableResource, std::size_t Count>
  bool areTopologiesIdentical(
    const std::array<TUsableResource, Count> &first,
    const std::array<TUsableResource, Count> &second
  ) {
    for(std::size_t index = 0; index < Count; ++index) {
      if(first[index].UnitCount != second[index].UnitCount) {
        return false;
      }
    }

    return true;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  ResourceBudget::ResourceBudget() :
    resources(makeDefaultResourceArray<UsableResource, MaximumResourceType + 1>()),
    busyHardDrives(0),
    allocatedMemoryBlock(nullptr) {}

  // ------------------------------------------------------------------------------------------- //

  ResourceBudget::ResourceBudget(const ResourceBudget &other) :
    resources(),
    busyHardDrives(other.busyHardDrives),
    allocatedMemoryBlock(nullptr) {

    std::size_t totalUnitCount = getTotalUnitCount(other.resources);
    std::size_t atomicsOffset = getAtomicsOffset(totalUnitCount);

    // Allocate enough memory for the resource totals and atomic values behind them
    std::unique_ptr<std::uint8_t[]> memoryBlock(
      new std::uint8_t[(sizeof(std::atomic_size_t[2]) * totalUnitCount / 2) + atomicsOffset]
    );
    std::size_t *sizes = reinterpret_cast<std::size_t *>(memoryBlock.get());
    std::atomic_size_t *atomics = reinterpret_cast<std::atomic_size_t *>(
      memoryBlock.get() + atomicsOffset
    );

    for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
      std::size_t unitCount = other.resources[index].UnitCount;

      this->resources[index].UnitCount = unitCount;
      this->resources[index].HighestTotal = other.resources[index].HighestTotal;
      this->resources[index].Total = sizes;
      this->resources[index].Remaining = atomics;

      for(std::size_t unitIndex = 0; unitIndex < unitCount; ++unitIndex) {
        this->resources[index].Total[unitIndex] = other.resources[index].Total[unitIndex];
        new(atomics + unitIndex) std::atomic_size_t(
          other.resources[index].Remaining[unitIndex].load(
            std::memory_order::memory_order_relaxed
          )
        );
      }

      sizes += unitCount;
      atomics += unitCount;
    }

    this->allocatedMemoryBlock = memoryBlock.release();
    std::atomic_thread_fence(std::memory_order_release);
  }

  // ------------------------------------------------------------------------------------------- //

  ResourceBudget::ResourceBudget(ResourceBudget &&other) :
    resources(other.resources),
    busyHardDrives(other.busyHardDrives),
    allocatedMemoryBlock(other.allocatedMemoryBlock) {
    other.allocatedMemoryBlock = nullptr;
  }

  // ------------------------------------------------------------------------------------------- //

  ResourceBudget::~ResourceBudget() {
    freeUsableResourceInventory(this->allocatedMemoryBlock, this->resources);
  }

  // ------------------------------------------------------------------------------------------- //

  ResourceBudget &ResourceBudget::operator =(const ResourceBudget &other) {
    if(areTopologiesIdentical(this->resources, other.resources)) {
      for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
        std::size_t unitCount = this->resources[index].UnitCount;
        this->resources[index].UnitCount = unitCount;
        this->resources[index].HighestTotal = other.resources[index].HighestTotal;
        for(std::size_t unitIndex = 0; unitIndex < unitCount; ++unitIndex) {
          this->resources[index].Total[unitIndex] = other.resources[index].Total[unitIndex];
          this->resources[index].Remaining[unitIndex].store(
            other.resources[index].Remaining[unitIndex].load(
              std::memory_order::memory_order_relaxed
            ),
            std::memory_order::memory_order_relaxed
          );
        }
      }
    } else {
      freeUsableResourceInventory(this->allocatedMemoryBlock, this->resources);

      std::size_t totalUnitCount = getTotalUnitCount(other.resources);
      std::size_t atomicsOffset = getAtomicsOffset(totalUnitCount);

      // Allocate enough memory for the resource totals and atomic values behind them
      std::unique_ptr<std::uint8_t[]> memoryBlock(
        new std::uint8_t[(sizeof(std::atomic_size_t[2]) * totalUnitCount / 2) + atomicsOffset]
      );
      std::size_t *sizes = reinterpret_cast<std::size_t *>(memoryBlock.get());
      std::atomic_size_t *atomics = reinterpret_cast<std::atomic_size_t *>(
        memoryBlock.get() + atomicsOffset
      );

      for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
        std::size_t unitCount = other.resources[index].UnitCount;

        this->resources[index].UnitCount = unitCount;
        this->resources[index].HighestTotal = other.resources[index].HighestTotal;
        this->resources[index].Total = sizes;
        this->resources[index].Remaining = atomics;

        for(std::size_t unitIndex = 0; unitIndex < unitCount; ++unitIndex) {
          this->resources[index].Total[unitIndex] = other.resources[index].Total[unitIndex];
          new(atomics + unitIndex) std::atomic_size_t(
            other.resources[index].Remaining[unitIndex].load(
              std::memory_order::memory_order_relaxed
            )
          );
        }

        sizes += unitCount;
        atomics += unitCount;
      }

      this->allocatedMemoryBlock = memoryBlock.release();
    }

    std::atomic_thread_fence(std::memory_order_release);
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  ResourceBudget &ResourceBudget::operator =(ResourceBudget &&other) {
    freeUsableResourceInventory(this->allocatedMemoryBlock, this->resources);
    this->allocatedMemoryBlock = other.allocatedMemoryBlock;
    this->resources = std::move(other.resources);
    other.allocatedMemoryBlock = nullptr;
    this->busyHardDrives = other.busyHardDrives;
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  void ResourceBudget::AddResource(
    Tasks::ResourceType resourceType, std::size_t amountAvailable
  ) {
    std::size_t resourceTypeIndex = static_cast<std::size_t>(resourceType);
    assert(
      (resourceTypeIndex < this->resources.size()) &&
      u8"Resource type within range of enumeration"
    );

    std::size_t totalUnitCount = getTotalUnitCount(this->resources) + 1;
    std::size_t atomicsOffset = getAtomicsOffset(totalUnitCount);

    // Allocate enough memory for the resource totals and atomic values behind them
    std::unique_ptr<std::uint8_t[]> memoryBlock(
      new std::uint8_t[(sizeof(std::atomic_size_t[2]) * totalUnitCount / 2) + atomicsOffset]
    );
    std::size_t *sizes = reinterpret_cast<std::size_t *>(memoryBlock.get());
    std::atomic_size_t *atomics = reinterpret_cast<std::atomic_size_t *>(
      memoryBlock.get() + atomicsOffset
    );

    // Now copy the existing unit budgets into the new memory block, then switch out
    // the pointers. When we hit the resource to which the user is adding a unit,
    // we append the new unit as extra entry in that resource on-the-go.
    for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
      std::size_t unitCount = this->resources[index].UnitCount;

      // Copy the existing values over (if any)
      for(std::size_t unitIndex = 0; unitIndex < unitCount; ++unitIndex) {
        sizes[unitIndex] = this->resources[index].Total[unitIndex];
        new(atomics + unitIndex) std::atomic_size_t(
          this->resources[index].Remaining[unitIndex].load(
            std::memory_order::memory_order_relaxed
          )
        );
        this->resources[index].Remaining[unitIndex].~atomic();
      }

      // Replace our old pointers with the new memory block we just filled
      this->resources[index].Total = sizes;
      this->resources[index].Remaining = atomics;

      // If the current resource is the one to which the user added a unit,
      // append that unit to the end of the per-unit total and remaining amounts.
      if(index == resourceTypeIndex) {
        if(unitCount == 0) { // This is the first unit of the resource type
          this->resources[index].UnitCount = 1;
          this->resources[index].HighestTotal = amountAvailable;
          this->resources[index].Total[0] = amountAvailable;
          new(atomics) std::atomic_size_t(amountAvailable);
          unitCount = 1;
        } else { // Resource type has gained an additional unit
          this->resources[index].UnitCount = unitCount + 1;
          this->resources[index].HighestTotal = std::max(
            this->resources[index].HighestTotal, amountAvailable
          );
          this->resources[index].Total[unitCount] = amountAvailable;
          new(atomics + unitCount) std::atomic_size_t(amountAvailable);
          ++unitCount;
        }
      }

      // Advance the pointers such that the next resouce's total and remaining amounts
      // will start further into the array
      sizes += unitCount;
      atomics += unitCount;
    }

    // Replacement done, we can get rid of the old memory block and keep the new one
    delete[] this->allocatedMemoryBlock;
    this->allocatedMemoryBlock = memoryBlock.release();
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t ResourceBudget::QueryResourceMaximum(ResourceType resourceType) const {
    std::size_t index = static_cast<std::size_t>(resourceType);
    assert(
      (index < this->resources.size()) && u8"Resource type within range of enumeration"
    );
    return this->resources[index].HighestTotal;
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t ResourceBudget::CountResourceUnits(ResourceType resourceType) const {
    std::size_t index = static_cast<std::size_t>(resourceType);
    assert(
      (index < this->resources.size()) && u8"Resource type within range of enumeration"
    );
    return this->resources[index].UnitCount;
  }

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::CanEverExecute(
    const std::shared_ptr<TaskEnvironment> &environment,
    const ResourceManifestPointer &taskResources /* = ResourceManifestPointer() */
  ) const {
    if(environment && environment->Resources) {
      return CanEverExecute(environment->Resources, taskResources);
    } else if(taskResources) {
      return CanEverExecute(taskResources);
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::CanEverExecute(
    const ResourceManifestPointer &primaryResources,
    const ResourceManifestPointer &secondaryResources /* = ResourceManifestPointer() */
  ) const {
    std::size_t required[MaximumResourceType + 1] = {0};

    // First, put the resources from the environment in our tally
    for(std::size_t index = 0; index < primaryResources->Count; ++index) {
      std::size_t resourceIndex = static_cast<std::size_t>(
        primaryResources->Resources[index].Type
      );
      required[resourceIndex] = primaryResources->Resources[index].Amount;
    }

    // Add the resources required by the task itself
    if(secondaryResources) {
      for(std::size_t index = 0; index < secondaryResources->Count; ++index) {
        std::size_t resourceIndex = (
          static_cast<std::size_t>(secondaryResources->Resources[index].Type)
        );
        required[resourceIndex] += secondaryResources->Resources[index].Amount;
      }
    }

    // Now look for any resource whose combined total exceeds the maximum we can provide
    for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
      std::size_t highestTotal = this->resources[index].HighestTotal;
      if(highestTotal < required[index]) {
        return false;
      }
    }

    // No requirement found that exceeded that amount of resources we can deliver
    return true;
  }

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::CanExecuteNow(
    const std::shared_ptr<TaskEnvironment> &environment,
    const ResourceManifestPointer &taskResources /* = ResourceManifestPointer() */
  ) const {
    if(environment && environment->Resources) {
      return CanExecuteNow(environment->Resources, taskResources);
    } else if(taskResources) {
      return CanExecuteNow(taskResources);
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::CanExecuteNow(
    const ResourceManifestPointer &primaryResources,
    const ResourceManifestPointer &secondaryResources /* = ResourceManifestPointer() */
  ) const {
    std::size_t required[MaximumResourceType + 1] = {0};

    // First, put the resources from the environment in our tally
    for(std::size_t index = 0; index < primaryResources->Count; ++index) {
      std::size_t resourceIndex = static_cast<std::size_t>(
        primaryResources->Resources[index].Type
      );
      required[resourceIndex] = primaryResources->Resources[index].Amount;
    }

    // Add the resources required by the task itself
    if(secondaryResources) {
      for(std::size_t index = 0; index < secondaryResources->Count; ++index) {
        std::size_t resourceIndex = (
          static_cast<std::size_t>(secondaryResources->Resources[index].Type)
        );
        required[resourceIndex] += secondaryResources->Resources[index].Amount;
      }
    }

    // Now look for any resource whose combined total exceeds the maximum we can provide
    for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {

      // Look for the unit with the highest amount of the resource being asked for
      std::size_t highestAvailable = 0;
      {
        std::size_t unitCount = this->resources[index].UnitCount;
        for(std::size_t unitIndex = 0; unitIndex < unitCount; ++unitIndex) {
          std::size_t available = this->resources[index].Remaining[unitIndex].load(
            std::memory_order::memory_order_acquire
          );
          if(highestAvailable < available) {
            highestAvailable = available;
          }
        }
      }

      if(highestAvailable < required[index]) {
        return false;
      }
    }

    // No requirement found that exceeded that amount of resources we can deliver
    return true;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
