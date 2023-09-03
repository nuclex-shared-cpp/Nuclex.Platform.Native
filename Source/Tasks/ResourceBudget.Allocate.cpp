#pragma region CPL License
/*
Nuclex Native Framework
Copyright (C) 2002-2021 Nuclex Development Labs

This library is free software; you can redistribute it and/or
modify it under the terms of the IBM Common Public License as
published by the IBM Corporation; either version 1.0 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
IBM Common Public License for more details.

You should have received a copy of the IBM Common Public
License along with this library
*/
#pragma endregion // CPL License

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "./ResourceBudget.h"

#include "Nuclex/Platform/Tasks/TaskEnvironment.h" // for TaskEnvironment
#include "Nuclex/Platform/Tasks/ResourceManifest.h" // for ResouceManifest

#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

  // ReversibleBudgeteer
  //
  //     ###      Caution! This is a very specialized class to support RAII-like rollback
  //    ## ##     without depending on a call stack scrambling re-throw in case of error.
  //   ## | ##
  //  ##  '  ##   It takes pointers out of std::shared_ptr and stores them and it writes
  // ###########  selected unit indices as a side effect.
  //
  // See this class as an out-sourcing of the ResourceBudget::Allocate() method and never
  // let the lifetime of an instance extend beyond the scope of that method!

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Deducts resources from a budget and optionally rolls everything back when destroyed
  /// </summary>
  /// <typeparam name="TUsableResource">
  ///   Type UsableResource, passed via template parameter so it can be private in the class
  /// </typeparam>
  /// <typeparam name="Count">
  ///   Number of resource types defined in the <see cref="ResourceType" /> enumeration
  /// <typeparam>
  template<typename TUsableResource, std::size_t Count>
  class ReversibleBudgeteer {

    /// <summary>Initializes a new reversible budget deductor</summary>
    /// <param name="resources">Resource budget from which to deduct</param>
    /// <param name="selectedUnitIndices">
    ///   Array containing either (-1) or indices for the resource unit that should provide
    ///   each resource of the matching resource type index. This is used to i.e. when tasks
    ///   need to run on the same GPU (== video memory unit) an AI was initialized on.
    /// </param>
    public: ReversibleBudgeteer(
      std::array<TUsableResource, Count> &resources,
      std::array<std::size_t, Count> &selectedUnitIndices
    ) :
      resources(resources),
      primaryResources(nullptr),
      secondaryResources(nullptr),
      deductedResourceIndex(0),
      selectedUnitIndices(selectedUnitIndices) {}

    /// <summary>Rolls back all changes unless <see cref="Commit" /> was called</summary>
    public: ~ReversibleBudgeteer();

    /// <summary>Prevents the automatic rollback of the resource deduction</summary>
    public: void Commit() {
      this->primaryResources = nullptr;
    }

    /// <summary>
    ///   Tries to subtract the resources from the specified manifest from the budget
    /// </summary>
    /// <param name="manifest">Manifest whose resource list will be deducted</param>
    /// <returns>
    ///   True if the whole manifest was deducted without exceeding the budget,
    ///   false if less than the whole manifest could be deducted (the destructor will
    ///   roll back any changes made)
    /// </returns>
    public: bool TrySubtract(const Nuclex::Platform::Tasks::ResourceManifestPointer &manifest);

    /// <summary>Tries to deduct a single resource from the resource budget</summary>
    /// <param name="resourceType">Type of resource from which to deduct</param>
    /// <param name="resourceAmount">Amount of the resource that will be deducted</param>
    /// <param name="unitIndex">
    ///   Either std::size_t(-1) to select a unit automatically or the index of the unit
    ///   providing the resource that must be used
    /// </param>
    /// <returns>
    ///   True if the resource could be deducted, false if the remaining amount was insufficient
    /// </returns>
    private: bool tryDeduct(
      Nuclex::Platform::Tasks::ResourceType resourceType,
      std::size_t resourceAmount,
      std::size_t &unitIndex
    );

    /// <summary>Resource budget from which the requests will be deducted</summary>
    private: std::array<TUsableResource, Count> &resources;
    /// <summary>First set of resources that has been subtracted</summary>
    private: const Nuclex::Platform::Tasks::ResourceManifest *primaryResources;
    /// <summary>Secondary set of resources that has been subtracted</summary>
    private: const Nuclex::Platform::Tasks::ResourceManifest *secondaryResources;
    /// <summary>Index one past the most recently deducted resource</summary>
    /// <remarks>
    ///   If two resource manifests are deducted, the index continues counting past
    ///   the end of the first resource manifest into the second one.
    /// </remarks>
    private: std::size_t deductedResourceIndex;
    /// <summary>Units from which resources should be/have been deducted</summary>
    private: std::array<std::size_t, Count> &selectedUnitIndices;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TUsableResource, std::size_t Count>
  ReversibleBudgeteer<TUsableResource, Count>::~ReversibleBudgeteer() {
    if(this->primaryResources != nullptr) {

      // We only need to touch the secondary resources if both a secondary resource
      // manifest exists and if the resource index has reached the secondary manifest
      // (otherwise, the budget allocation failed at the first listed resource)
      if(this->secondaryResources != nullptr) {
        if(this->deductedResourceIndex > this->primaryResources->Count) {

          // Return all previously deducted resources from the secondary resource manifest
          std::size_t count = this->deductedResourceIndex - this->primaryResources->Count;
          while(count > 0) {
            --count;
            --this->deductedResourceIndex;

            std::size_t resourceTypeIndex = static_cast<std::size_t>(
              this->secondaryResources->Resources[count].Type
            );
            std::size_t unitIndex = this->selectedUnitIndices[resourceTypeIndex];

            this->resources[resourceTypeIndex].Remaining[unitIndex].fetch_add(
              this->secondaryResources->Resources[count].Amount
            );
          }

        } // if resource deduction reached secondary manifest
      } // if secondary resource manifest present

      // Return all previously deducted resources from the primary resource manifest
      while(this->deductedResourceIndex > 0) {
        --this->deductedResourceIndex;

        std::size_t resourceTypeIndex = static_cast<std::size_t>(
          this->primaryResources->Resources[this->deductedResourceIndex].Type
        );
        std::size_t unitIndex = this->selectedUnitIndices[resourceTypeIndex];

        this->resources[resourceTypeIndex].Remaining[unitIndex].fetch_add(
          this->primaryResources->Resources[this->deductedResourceIndex].Amount
        );
      }

    } // if primary resource manifest present
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TUsableResource, std::size_t Count>
  bool ReversibleBudgeteer<TUsableResource, Count>::TrySubtract(
    const Nuclex::Platform::Tasks::ResourceManifestPointer &manifest
  ) {
    const Nuclex::Platform::Tasks::ResourceManifest *unwrappedManifest = manifest.get();
    if(this->primaryResources == nullptr) {
      this->primaryResources = unwrappedManifest;
    } else {
      assert(this->secondaryResources == nullptr);
      this->secondaryResources = unwrappedManifest;
    }

    // Attempt to deduct all requested resource from the budget's resource array.
    // If any resource request can not be fulfilled, we simply return false, leave
    // the selectedUnitIndices list in disarray and roll back in the destructor.
    std::size_t count = unwrappedManifest->Count;
    for(std::size_t index = 0; index < count; ++index) {
      std::size_t resourceTypeIndex = static_cast<std::size_t>(
        unwrappedManifest->Resources[index].Type
      );

      bool successfullyDeducted = tryDeduct(
        unwrappedManifest->Resources[index].Type,
        unwrappedManifest->Resources[index].Amount,
        this->selectedUnitIndices[resourceTypeIndex]
      );
      if(!successfullyDeducted) {
        return false; // automatic rollback follows on scope exit of instance
      }

      // Advance the index of successfully deducted resource so we can roll back again.
      // Notice that this index counts linearly over the primary and secondary resource
      // manifest in case two resource manifests were subtracted.
      ++this->deductedResourceIndex;
    }

    return true;
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TUsableResource, std::size_t Count>
  bool ReversibleBudgeteer<TUsableResource, Count>::tryDeduct(
    Nuclex::Platform::Tasks::ResourceType resourceType,
    std::size_t resourceAmount,
    std::size_t &unitIndex
  ) {
    std::size_t resourceTypeIndex = static_cast<std::size_t>(resourceType);

    // If no unit index is specified, look at all units that could provide the resource
    if(unitIndex == std::size_t(-1)) {
      std::size_t unitCount = this->resources[resourceTypeIndex].UnitCount;
      for(unitIndex = 0; unitIndex < unitCount; ++unitIndex) {

        // Simple C-A-S loop to check if the current unit has sufficient resources and,
        // if so, deduct the required resource amount
        std::size_t remaining = this->resources[resourceTypeIndex].Remaining[unitIndex].load(
          std::memory_order::memory_order_acquire
        );
        while(remaining >= resourceAmount) {
          std::size_t newRemaining = remaining - resourceAmount;
          if(
            this->resources[resourceTypeIndex].Remaining[unitIndex].compare_exchange_weak(
              remaining, newRemaining
            )
          ) {
            return true;
          }
        }

      }
    } else { // The caller wants a specified unit to provide the resource

      // Same C-A-S loop as above, check if the requested unit has sufficient resources and,
      // if so, deduct the required resource amount
      std::size_t remaining = this->resources[resourceTypeIndex].Remaining[unitIndex].load(
        std::memory_order::memory_order_acquire
      );
      while(remaining >= resourceAmount) {
        std::size_t newRemaining = remaining - resourceAmount;
        if(
          this->resources[resourceTypeIndex].Remaining[unitIndex].compare_exchange_weak(
            remaining, newRemaining
            //std::memory_order::memory_order_release, std::memory_order::memory_order_relaxed
          )
        ) {
          return true;
        }
      }

    } // If resource unit index specified / not specified

    // We found no unit that could provide the requested amount of the specified resource
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Adds a resource manifest's resource amounts to a simple resource array</summary>
  /// <typeparam name="Count">Total number of resource types there are</typeparapm>
  /// <param name="resourceSet">
  ///   Resource array to which the manifest's resource amount will be added
  /// </param>
  /// <param name="resourceManifest">
  ///   Resource manifest whose resource amounts will be added to the specified array
  /// </param>
  template<std::size_t Count>
  void addManifestToResourceSet(
    std::array<std::size_t, Count> &resourceSet,
    const Nuclex::Platform::Tasks::ResourceManifestPointer &resourceManifest
  ) {
    std::size_t resourceCount = resourceManifest->Count;
    for(std::size_t index = 0; index < resourceCount; ++ index) {
      std::size_t resourceTypeIndex = static_cast<std::size_t>(
        resourceManifest->Resources[index].Type
      );
      resourceSet[resourceTypeIndex] += resourceManifest->Resources[index].Amount;
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::Pick(
    std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
    const std::shared_ptr<TaskEnvironment> &environment,
    const ResourceManifestPointer &taskResources /* = ResourceManifestPointer() */
  ) const {
    if(environment && environment->Resources) {
      return Pick(inOutUnitIndices, environment->Resources, taskResources);
    } else if(taskResources) {
      return Pick(inOutUnitIndices, taskResources);
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::Pick(
    std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
    const ResourceManifestPointer &primaryResources,
    const ResourceManifestPointer &secondaryResources /* = ResourceManifestPointer() */
  ) const {

    // Sum up the required resources because we need to find a unit providing
    // each resource that has enough space for both at once.
    std::size_t required[MaximumResourceType + 1] = {0};
    {
      for(std::size_t index = 0; index < primaryResources->Count; ++index) {
        std::size_t resourceIndex = static_cast<std::size_t>(
          primaryResources->Resources[index].Type
        );
        required[resourceIndex] = primaryResources->Resources[index].Amount;
      }
      if(secondaryResources) {
        for(std::size_t index = 0; index < secondaryResources->Count; ++index) {
          std::size_t resourceIndex = (
            static_cast<std::size_t>(secondaryResources->Resources[index].Type)
          );
          required[resourceIndex] += secondaryResources->Resources[index].Amount;
        }
      }
    }

    // CHECK: Should we prefer the unit with the most resources or the tightest fit?
    //   The unit with the most resources would allow for multi-GPU systems to use the
    //   most powerful GPU in low contention. The unit with the tightest fit would
    //   minimize redistribution of tasks later on by keeping free resources as
    //   concentrated on single units as possible.

    // Now try find units for the requested resources
    for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
      if(0 < required[index]) {
        std::size_t closestAvailable = std::size_t(-1);

        // Look for the unit with the tightest fit to the resources being asked for
        std::size_t unitCount = this->resources[index].UnitCount;
        for(std::size_t unitIndex = 0; unitIndex < unitCount; ++unitIndex) {
          std::size_t available = this->resources[index].Remaining[unitIndex].load(
            std::memory_order::memory_order_acquire
          );
          if(available >= required[index]) {
            std::size_t surplusAvailable = available - required[index];
            if(surplusAvailable < closestAvailable) {
              closestAvailable = surplusAvailable;
              inOutUnitIndices[index] = unitIndex;
            }
          } // if enough of the resource is available
        } // for each unit index

        // If no unit can fulfill this resource requirement, we fail
        if(closestAvailable == std::size_t(-1)) {
          return false;
        }
      } // resource type is needed

    } // for each resource type

    // No requirement found that exceeded that amount of resources we can deliver
    return true;
  }

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::Allocate(
    std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
    const std::shared_ptr<TaskEnvironment> &environment,
    const ResourceManifestPointer &taskResources /* = ResourceManifestPointer() */
  ) {
    if(environment && environment->Resources) {
      return Allocate(inOutUnitIndices, environment->Resources, taskResources);
    } else if(taskResources) {
      return Allocate(inOutUnitIndices, taskResources);
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool ResourceBudget::Allocate(
    std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
    const ResourceManifestPointer &primaryResources,
    const ResourceManifestPointer &secondaryResources /* = ResourceManifestPointer() */
  ) {
    ReversibleBudgeteer<UsableResource, MaximumResourceType + 1> budgeteer(
      this->resources, inOutUnitIndices
    );
    if(primaryResources) {
      if(!budgeteer.TrySubtract(primaryResources)) {
        return false;
      }
    }

    if(secondaryResources) {
      if(!budgeteer.TrySubtract(secondaryResources)) {
        return false;
      }
    }

    budgeteer.Commit();
    return true;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
