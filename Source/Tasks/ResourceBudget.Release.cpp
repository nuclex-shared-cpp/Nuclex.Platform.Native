#pragma region CPL License
/*
Nuclex Native Framework
Copyright (C) 2002-2023 Nuclex Development Labs

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
#include "Nuclex/Platform/Tasks/ResourceManifest.h" // for ResourceManifest

#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  void ResourceBudget::Release(
    const std::array<std::size_t, MaximumResourceType + 1> &allocatedUnitIndices,
    const std::shared_ptr<TaskEnvironment> &environment,
    const ResourceManifestPointer &taskResources /* = ResourceManifestPointer() */
  ) {
    if(taskResources) {
      const ResourceManifest &manifest = *taskResources.get();

      std::size_t count = manifest.Count;
      while(count > 0) {
        --count;

        std::size_t resourceTypeIndex = static_cast<std::size_t>(
          manifest.Resources[count].Type
        );
        std::size_t unitIndex = allocatedUnitIndices[resourceTypeIndex];

        this->resources[resourceTypeIndex].Remaining[unitIndex].fetch_add(
          manifest.Resources[count].Amount
        );
      }
    }

    if(environment && environment->Resources) {
      const ResourceManifest &manifest = *environment->Resources.get();

      std::size_t count = manifest.Count;
      while(count > 0) {
        --count;

        std::size_t resourceTypeIndex = static_cast<std::size_t>(
          manifest.Resources[count].Type
        );
        std::size_t unitIndex = allocatedUnitIndices[resourceTypeIndex];

        this->resources[resourceTypeIndex].Remaining[unitIndex].fetch_add(
          manifest.Resources[count].Amount
        );
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void ResourceBudget::Release(
    const std::array<std::size_t, MaximumResourceType + 1> &allocatedUnitIndices,
    const ResourceManifestPointer &primaryResources,
    const ResourceManifestPointer &secondaryResources /* = ResourceManifestPointer() */
  ) {
    if(secondaryResources) {
      const ResourceManifest &manifest = *secondaryResources.get();

      std::size_t count = manifest.Count;
      while(count > 0) {
        --count;

        std::size_t resourceTypeIndex = static_cast<std::size_t>(
          manifest.Resources[count].Type
        );
        std::size_t unitIndex = allocatedUnitIndices[resourceTypeIndex];

        this->resources[resourceTypeIndex].Remaining[unitIndex].fetch_add(
          manifest.Resources[count].Amount
        );
      }
    }

    if(primaryResources) {
      const ResourceManifest &manifest = *primaryResources.get();

      std::size_t count = manifest.Count;
      while(count > 0) {
        --count;

        std::size_t resourceTypeIndex = static_cast<std::size_t>(
          manifest.Resources[count].Type
        );
        std::size_t unitIndex = allocatedUnitIndices[resourceTypeIndex];

        this->resources[resourceTypeIndex].Remaining[unitIndex].fetch_add(
          manifest.Resources[count].Amount
        );
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
