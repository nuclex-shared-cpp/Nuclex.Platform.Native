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

#ifndef NUCLEX_PLATFORM_TASKS_RESOURCEMANIFEST_H
#define NUCLEX_PLATFORM_TASKS_RESOURCEMANIFEST_H

#include "Nuclex/Platform/Config.h"

#include "Nuclex/Platform/Tasks/ResourceType.h"

#include <cstddef> // for std::size_t
#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Listing of resources that are needed to perform a task</summary>
  class NUCLEX_PLATFORM_TYPE ResourceManifest {

    #pragma region struct Entry

    /// <summary>Specifies the amount of a resource that a task needs to execute</summary>
    public: struct Entry {

      /// <summary>Amount of the resource (core count, bytes memory) the task needs</summary>
      public: std::size_t Amount;

      /// <summary>Kind of resource the task will occupy to do its work</summary>
      public: ResourceType Type;

    };

    #pragma endregion // struct Entry

    /// <summary>Creates a resource manifest with one resource requirement</summary>
    /// <param name="resourceType">Type of resource the manifest should list</param>
    /// <param name="resourceAmount">Amount of the resource that is required</param>
    /// <returns>A pointer to the new resource manifest</returns>
    public: NUCLEX_PLATFORM_API static std::shared_ptr<ResourceManifest> Create(
      ResourceType resourceType, std::size_t resourceAmount
    );

    /// <summary>Creates a resource manifest with two resource requirements</summary>
    /// <param name="resource1Type">Type of the first resource in the manifest</param>
    /// <param name="resource1Amount">Amount required of the first resource</param>
    /// <param name="resource2Type">Type of the second resource in the manifest</param>
    /// <param name="resource2Amount">Amount required of the second resource</param>
    /// <returns>A pointer to the new resource manifest</returns>
    public: NUCLEX_PLATFORM_API static std::shared_ptr<ResourceManifest> Create(
      ResourceType resource1Type, std::size_t resource1Amount,
      ResourceType resource2Type, std::size_t resource2Amount
    );

    /// <summary>Creates a resource manifest with three resource requirements</summary>
    /// <param name="resource1Type">Type of the first resource in the manifest</param>
    /// <param name="resource1Amount">Amount required of the first resource</param>
    /// <param name="resource2Type">Type of the second resource in the manifest</param>
    /// <param name="resource2Amount">Amount required of the second resource</param>
    /// <param name="resource3Type">Type of the third resource in the manifest</param>
    /// <param name="resource3Amount">Amount required of the third resource</param>
    /// <returns>A pointer to the new resource manifest</returns>
    public: NUCLEX_PLATFORM_API static std::shared_ptr<ResourceManifest> Create(
      ResourceType resource1Type, std::size_t resource1Amount,
      ResourceType resource2Type, std::size_t resource2Amount,
      ResourceType resource3Type, std::size_t resource3Amount
    );

    /// <summary>Builds the sum of two resource manifests</summary>
    /// <param name="first">First resource manifest to combine</param>
    /// <param name="second">First resource manifest to combine</param>
    /// <returns>A resource manifest that is the sum of both input manifests</returns>
    public: NUCLEX_PLATFORM_API static std::shared_ptr<ResourceManifest> Combine(
      const std::shared_ptr<ResourceManifest> &first,
      const std::shared_ptr<ResourceManifest> &second
    );

    /// <summary>Number of resources in the manifest</summary>
    public: std::size_t Count;
    /// <summary>List of the resource types with the amount needed of each</summary>
    public: const Entry *Resources;
    /// <summary>Bit mask indicating the hard drives that will be accessed</summary>
    /// <remarks>
    ///   This field is initialized to 0 (no hard drives accessed). It should be directly
    ///   assigned in case a workload accesses any hard drives. Methods such as
    ///   <see cref="Combine" /> will use bitwise operations as appropriate for flags.
    /// </remarks>
    public: std::size_t AccessedHardDriveMask;

    // Could track NetworkAdapters to focus bandwidth on single action, but pointless?
    // Could track Camera accesses, but makes little sense?

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Type of pointer returned by the <see cref="ResourceManifest::Create" /> methods
  /// </summary>
  typedef std::shared_ptr<ResourceManifest> ResourceManifestPointer;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_RESOURCEMANIFEST_H
