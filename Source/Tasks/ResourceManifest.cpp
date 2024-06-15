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

#include "Nuclex/Platform/Tasks/ResourceManifest.h"

#include <cstdint> // for std::uint8_t
#include <cassert> // for assert()
#include <array> // for std::array

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Calculates the offset of the requirements from the resource manifest</summary>
  /// <returns>
  ///   The offset, in bytes, from a pointer to a resource manifest to the first resource
  ///   requirement stored in it
  /// </returns>
  template<typename TResourceManifest>
  constexpr std::size_t getResourceRequirementOffset() {
    constexpr std::size_t extraByteCount = (
      sizeof(TResourceManifest) %
      alignof(Nuclex::Platform::Tasks::ResourceManifest::Entry)
    );
    return (
      sizeof(TResourceManifest) + (
        (extraByteCount == 0) ?
        (0) :
        (sizeof(TResourceManifest) - extraByteCount)
      )
    );
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Custom alloctor that allocates a shared resource manifest</summary>
  /// <typeparam name="TResourceManifest">
  ///   Type that will be allocated, expected to derive from ResourceManifest
  /// </typeparam>
  /// <remarks>
  ///   Normally, a non-templated implementation of this allocator would seem to suffice,
  ///   but <code>std::allocate_shared()</code> implementations will very likely
  ///   (via the type-changing copy constructor) allocate a type inherited from our
  ///   <see cref="ResourceManifest" /> that packages the reference counter of
  ///   the <code>std::shared_ptr</code> together with the instance.
  /// </remarks>
  template<class TResourceManifest>
  class ResourceManifestAllocator {

    /// <summary>Type of element the allocator is for, required by standard</summary>
    public: typedef TResourceManifest value_type;

    /// <summary>Initializes a new allocator using the specified appended list size</summary>
    /// <param name="resourceCount">Number of resources  to allocate extra space for</param>
    public: ResourceManifestAllocator(std::size_t resourceCount) :
      resourceCount(resourceCount) {}

    /// <summary>
    ///   Creates this allocator as a clone of an allocator for a different type
    /// </summary>
    /// <typeparam name="TOther">Type the existing allocator is allocating for</typeparam>
    /// <param name="other">Existing allocator whose attributes will be copied</param>
    public: template<class TOther> ResourceManifestAllocator(
      const ResourceManifestAllocator<TOther> &other
    ) : resourceCount(other.resourceCount) {}

    /// <summary>Allocates memory for the specified number of elements (must be 1)</summary>
    /// <param name="count">Number of elements to allocate memory for (must be 1)</param>
    /// <returns>The allocated (but not initialized) memory for the requested type</returns>
    public: TResourceManifest *allocate(std::size_t count) {
      NUCLEX_PLATFORM_NDEBUG_UNUSED(count);
      assert(count == 1);

      std::size_t requiredByteCount = getResourceRequirementOffset<TResourceManifest>() + (
        sizeof(Nuclex::Platform::Tasks::ResourceManifest::Entry[2]) * this->resourceCount / 2
      );

      return reinterpret_cast<TResourceManifest *>(new std::uint8_t[requiredByteCount]);
    }

    /// <summary>Frees memory for the specified element (count must be 1)</summary>
    /// <param name="instance">Instance for which memory will be freed</param>
    /// <param name="count">Number of instances that will be freed (must be 1)</param>
    public: void deallocate(TResourceManifest *instance, std::size_t count) {
      NUCLEX_PLATFORM_NDEBUG_UNUSED(count);
      assert(count == 1);

      delete[] reinterpret_cast<std::uint8_t *>(instance);
    }

    /// <summary>Size of the resource list for which extra space will be allocated</summary>
    public: std::size_t resourceCount;

  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<ResourceManifest> ResourceManifest::Create(
    Tasks::ResourceType resourceType, std::size_t resourceAmount
  ) {
    std::shared_ptr<ResourceManifest> resourceManifest = (
      std::allocate_shared<ResourceManifest>(
        ResourceManifestAllocator<ResourceManifest>(1)
      )
    );

    Entry *resources = reinterpret_cast<Entry *>(
      reinterpret_cast<std::uint8_t *>(resourceManifest.get()) +
      getResourceRequirementOffset<ResourceManifest>()
    );

    resourceManifest->Count = 1;
    resourceManifest->Resources = resources;
    resourceManifest->AccessedHardDriveMask = 0;

    resources[0].Amount = resourceAmount;
    resources[0].Type = resourceType;

    return resourceManifest;
  }

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<ResourceManifest> ResourceManifest::Create(
    Tasks::ResourceType resource1Type, std::size_t resource1Amount,
    Tasks::ResourceType resource2Type, std::size_t resource2Amount
  ) {
    std::shared_ptr<ResourceManifest> resourceManifest = (
      std::allocate_shared<ResourceManifest>(
        ResourceManifestAllocator<ResourceManifest>(2)
      )
    );

    Entry *resources = reinterpret_cast<Entry *>(
      reinterpret_cast<std::uint8_t *>(resourceManifest.get()) +
      getResourceRequirementOffset<ResourceManifest>()
    );

    resourceManifest->Count = 2;
    resourceManifest->Resources = resources;
    resourceManifest->AccessedHardDriveMask = 0;

    resources[0].Amount = resource1Amount;
    resources[0].Type = resource1Type;
    resources[1].Amount = resource2Amount;
    resources[1].Type = resource2Type;

    return resourceManifest;
  }

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<ResourceManifest> ResourceManifest::Create(
    Tasks::ResourceType resource1Type, std::size_t resource1Amount,
    Tasks::ResourceType resource2Type, std::size_t resource2Amount,
    Tasks::ResourceType resource3Type, std::size_t resource3Amount
  ) {
    std::shared_ptr<ResourceManifest> resourceManifest = (
      std::allocate_shared<ResourceManifest>(
        ResourceManifestAllocator<ResourceManifest>(3)
      )
    );

    Entry *resources = reinterpret_cast<Entry *>(
      reinterpret_cast<std::uint8_t *>(resourceManifest.get()) +
      getResourceRequirementOffset<ResourceManifest>()
    );

    resourceManifest->Count = 3;
    resourceManifest->Resources = resources;
    resourceManifest->AccessedHardDriveMask = 0;

    resources[0].Amount = resource1Amount;
    resources[0].Type = resource1Type;
    resources[1].Amount = resource2Amount;
    resources[1].Type = resource2Type;
    resources[2].Amount = resource3Amount;
    resources[2].Type = resource3Type;

    return resourceManifest;
  }

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<ResourceManifest> ResourceManifest::Combine(
    const std::shared_ptr<ResourceManifest> &first,
    const std::shared_ptr<ResourceManifest> &second
  ) {

    // Determine the resource types that will be present in the combined manifest
    std::array<bool, MaximumResourceType + 1> presentResourceTypes;
    {
      for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
        presentResourceTypes[index] = false;
      }
      for(std::size_t index = 0; index < first->Count; ++index) {
        std::size_t resourceIndex = static_cast<std::size_t>(first->Resources[index].Type);
        presentResourceTypes[resourceIndex] = true;
      }
      for(std::size_t index = 0; index < second->Count; ++index) {
        std::size_t resourceIndex = static_cast<std::size_t>(second->Resources[index].Type);
        presentResourceTypes[resourceIndex] = true;
      }
    }

    // Count the number of unique resource types (so we know how many resource entries
    // to allocate in the combined manifest)
    std::size_t resourceTypeCount = 0;
    for(std::size_t index = 0; index < MaximumResourceType + 1; ++index) {
      if(presentResourceTypes[index]) {
        ++resourceTypeCount;
      }
    }

    // Now we can do the usual thing to allocate and fill a combined resource manifest
    std::shared_ptr<ResourceManifest> resourceManifest = (
      std::allocate_shared<ResourceManifest>(
        ResourceManifestAllocator<ResourceManifest>(resourceTypeCount)
      )
    );

    Entry *resources = reinterpret_cast<Entry *>(
      reinterpret_cast<std::uint8_t *>(resourceManifest.get()) +
      getResourceRequirementOffset<ResourceManifest>()
    );

    resourceManifest->Count = resourceTypeCount;
    resourceManifest->Resources = resources;
    resourceManifest->AccessedHardDriveMask = (
      first->AccessedHardDriveMask | second->AccessedHardDriveMask
    );

    // Copy the resources from the first manifest over directly
    std::size_t addedResourceTypeCount = 0;
    for(std::size_t index = 0; index < first->Count; ++index) {
      resources[addedResourceTypeCount].Type = first->Resources[index].Type;
      resources[addedResourceTypeCount].Amount = first->Resources[index].Amount;
      ++addedResourceTypeCount;
    }

    // Then go over the entries in the second manifest and figure out whether
    // their resource type is already present and needs to be summed or whether
    // to add a new resource entry to the combined manifest.
    for(std::size_t index = 0; index < second->Count; ++index) {
      std::size_t existingIndex;
      for(existingIndex = 0; existingIndex < first->Count; ++existingIndex) {
        if(resources[existingIndex].Type == second->Resources[index].Type) {
          break;
        }
      }

      if(existingIndex < first->Count) {
        resources[existingIndex].Amount += second->Resources[index].Amount;
      } else {
        resources[addedResourceTypeCount].Type = second->Resources[index].Type;
        resources[addedResourceTypeCount].Amount = second->Resources[index].Amount;
        ++addedResourceTypeCount;
      }
    }

    return resourceManifest;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
