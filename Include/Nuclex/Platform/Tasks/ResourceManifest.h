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

#ifndef NUCLEX_PLATFORM_TASKS_RESOURCEMANIFEST_H
#define NUCLEX_PLATFORM_TASKS_RESOURCEMANIFEST_H

#include "Nuclex/Platform/Config.h"

#include "Nuclex/Platform/Tasks/ResourceType.h"

#include <cstddef> // for std::size_t
#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Listing of resources that are needed to perform a task</summary>
  class ResourceManifest {

    #pragma region struct Entry

    /// <summary>Specifies the amount of a resource that a task needs to execute</summary>
    public: struct Entry {

      /// <summary>Amount of the resource (core count, bytes memory) the task needs</summary>
      public: std::size_t Amount;

      /// <summary>Kind of resource the task will occupy to do its work</summary>
      public: Tasks::ResourceType Type;

    };

    #pragma endregion // struct Entry

    /// <summary>Creates a resource manifest with one resource requirement</summary>
    /// <param name="resourceType">Type of resource the manifest should list</param>
    /// <param name="resourceAmount">Amount of the resource that is required</param>
    /// <returns>A pointer to the new resource manifest</returns>
    public: static std::shared_ptr<ResourceManifest> Create(
      Tasks::ResourceType resourceType, std::size_t resourceAmount
    );

    /// <summary>Creates a resource manifest with two resource requirements</summary>
    /// <param name="resource1Type">Type of the first resource in the manifest</param>
    /// <param name="resource1Amount">Amount required of the first resource</param>
    /// <param name="resource2Type">Type of the second resource in the manifest</param>
    /// <param name="resource2Amount">Amount required of the second resource</param>
    /// <returns>A pointer to the new resource manifest</returns>
    public: static std::shared_ptr<ResourceManifest> Create(
      Tasks::ResourceType resource1Type, std::size_t resource1Amount,
      Tasks::ResourceType resource2Type, std::size_t resource2Amount
    );

    /// <summary>Creates a resource manifest with three resource requirements</summary>
    /// <param name="resource1Type">Type of the first resource in the manifest</param>
    /// <param name="resource1Amount">Amount required of the first resource</param>
    /// <param name="resource2Type">Type of the second resource in the manifest</param>
    /// <param name="resource2Amount">Amount required of the second resource</param>
    /// <param name="resource3Type">Type of the third resource in the manifest</param>
    /// <param name="resource3Amount">Amount required of the third resource</param>
    /// <returns>A pointer to the new resource manifest</returns>
    public: static std::shared_ptr<ResourceManifest> Create(
      Tasks::ResourceType resource1Type, std::size_t resource1Amount,
      Tasks::ResourceType resource2Type, std::size_t resource2Amount,
      Tasks::ResourceType resource3Type, std::size_t resource3Amount
    );

    /// <summary>Builds the sum of two resource manifests</summary>
    /// <param name="first">First resource manifest to combine</param>
    /// <param name="second">First resource manifest to combine</param>
    /// <returns>A resource manifest that is the sum of both input manifests</returns>
    public: static std::shared_ptr<ResourceManifest> Combine(
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

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Type of pointer returned by the <see cref="ResourceManifest::Create" /> methods
  /// </summary>
  typedef std::shared_ptr<ResourceManifest> ResourceManifestPointer;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Videl::Tasks

#endif // NUCLEX_PLATFORM_TASKS_RESOURCEMANIFEST_H
