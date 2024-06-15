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
