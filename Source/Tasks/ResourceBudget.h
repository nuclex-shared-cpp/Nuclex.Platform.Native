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

#ifndef NUCLEX_PLATFORM_TASKS_RESOURCEBUDGET_H
#define NUCLEX_PLATFORM_TASKS_RESOURCEBUDGET_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Tasks/ResourceType.h"

#include <cstddef> // for std::size_t
#include <cstdint> // for std::uint8_t
#include <array> // for std::array
#include <atomic> // for std::atomic
#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class TaskEnvironment;
  class ResourceManifest;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Videl::Tasks

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Keeps a running tally of the remaining resource of a task coordinator</summary>
  class ResourceBudget {

    /// <summary>Short alias for a shared pointer to a resource manifest</summary>
    /// <remarks>
    ///   You'll find the same typedef in ResourceManifest.h which we didn't include here
    ///   to keep dependencies small. It's only used for readability readons and it's a nested
    ///   typedef to avoid clashes when ResourceManifest.h gets included before this file.
    /// </remarks>
    private: typedef std::shared_ptr<ResourceManifest> ResourceManifestPointer;

    /// <summary>Initalizes a new resource budget</summary>
    public: ResourceBudget();

    /// <summary>Creates a clone of an existing resource budget</summary>
    /// <param name="other">Resource budget that will be cloned</param>
    public: ResourceBudget(const ResourceBudget &other);

    /// <summary>Takes over the contents of another resource budget</summary>
    /// <param name="other">Resource budget that will be moved</param>
    public: ResourceBudget(ResourceBudget &&other);

    /// <summary>Frees all resources owned by the resource budget</summary>
    public: ~ResourceBudget();

    /// <summary>Overwrites this resource budget with another resource budget</summary>
    /// <param name="other">Other resource budget that will be copied</param>
    /// <returns>This resource budget after the copy</returns>
    public: ResourceBudget &operator =(const ResourceBudget &other);

    /// <summary>Replaces this resource budget with another resource budget</summary>
    /// <param name="other">Other resource budget that will be moved</param>
    /// <returns>This resource budget after the assignment</returns>
    public: ResourceBudget &operator =(ResourceBudget &&other);

    /// <summary>Adds a resource that the task manager can allocate to tasks</summary>
    /// <param name="resourceType">Type of resource the task manager can allocate</param>
    /// <param name="amountAvailable">Amount of the resource available on the system</param>
    /// <remarks>
    ///   <para>
    ///     Calling this method multiple times with the same resource type will not accumulate
    ///     resources but instead handle it as an alternative resource unit (for example,
    ///     adding two times 16 GiB video memory does not allow the coordinator to run tasks
    ///     requiring 32 GiB video memory, but it will allow for two tasks to run in parallel).
    ///   </para>
    ///   <para>
    ///     The order of calls matters and sets up the individual resource units. For example,
    ///     if you add two sets of video memory, the video memory added in the first call is
    ///     treated as resource unit 0 and the video memory from the second all as resource
    ///     unit 1. This is important when the task coordinator tells tasks which resource unit
    ///     they are supposed to use.
    ///   </para>
    /// </remarks>
    public: void AddResource(ResourceType resourceType, std::size_t amountAvailable);

    /// <summary>Queries the amount of a resource still available in the budget</summary>
    /// <param name="resourceType">Type of resource that will be queried</param>
    /// <returns>The total amount of the queried resource in the system</returns>
    /// <remarks>
    ///   The task coordinator may return a number slightly lower than the maximum,
    ///   for example to keep some RAM available for the operating system. If there are
    ///   multiple GPUs installed, querying for video memory, for example, will return
    ///   the highest amount of video memory installed on any single device.
    /// </remarks>
    public: std::size_t QueryResource(Tasks::ResourceType resourceType) const;

    /// <summary>Counts the number of resource units that exist for a given resource</summary>
    /// <param name="resourceType">Type of resource whose units will be counted</param>
    /// <returns>The number of resource units providing the specified resource</returns>
    public: std::size_t CountResourceUnits(ResourceType resourceType) const;

    /// <summary>Picks resource units that can provide the requested resources</summary>
    /// <param name="inOutUnitIndices">
    ///   <para>
    ///     On input, unit indices that *must* be used (i.e. to select a certain GPU).
    ///     Provide an empty vector or set its contents to std::size_t(-1) in order to
    ///     allow any unit to be selected.
    ///   </para>
    ///   <para>
    ///     Upon return, receives the actually selected unit indices. Tasks must honor
    ///     the unit selections and pick i.e. the correct GPU (the ordering of units
    ///     always matches the order by which <see cref="HardwareMetrics" /> lists units).
    ///   </para>
    /// </param>
    /// <param name="environment">Task environment whose resources will be blocked</param>
    /// <param name="taskResources">Task-specific resources that will be blocked</param>
    /// <returns>
    ///   True if the budget had enough reserves to allocate all requested resources
    ///   (and units have been picked, but no resource amounts deducted!), false if one
    ///   or more resources were insufficient.
    /// </returns>
    /// <remarks>
    ///   <para>
    ///     This method is useful if you want to find resource units that have sufficient
    ///     capacity to activate an environment and run a task (that also needs resources)
    ///     on them at the same time, but not allocate the memory yet.
    ///   </para>
    ///   <para>
    ///     It is normally followed by a call to <see cref="Allocate" />, possibly only
    ///     to claim the environment memory but not the task memory in case the goal is to
    ///     active an environment, while merely making sure it is put on resource units
    ///     that can later carry a task using that environment.
    ///   </para>
    /// </remarks>
    public: bool Pick(
      std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
      const std::shared_ptr<TaskEnvironment> &environment,
      const ResourceManifestPointer &taskResources = ResourceManifestPointer()
    ) const;

    /// <summary>Picks resource units that can provide the requested resources</summary>
    /// <param name="inOutUnitIndices">
    ///   <para>
    ///     On input, unit indices that *must* be used (i.e. to select a certain GPU).
    ///     Provide an empty vector or set its contents to std::size_t(-1) in order to
    ///     allow any unit to be selected.
    ///   </para>
    ///   <para>
    ///     Upon return, receives the actually selected unit indices. Tasks must honor
    ///     the unit selections and pick i.e. the correct GPU (the ordering of units
    ///     always matches the order by which <see cref="HardwareMetrics" /> lists units).
    ///   </para>
    /// </param>
    /// <param name="primaryResources">First resource set that will be blocked</param>
    /// <param name="secondaryResources">Second resource set that will also be blocked</param>
    /// <returns>
    ///   True if the budget had enough reserves to allocate all requested resources
    ///   (and units have been picked, but no resource amounts deducted!), false if one
    ///   or more resources were insufficient.
    /// </returns>
    /// <remarks>
    ///   <para>
    ///     This method is useful if you want to find resource units that have sufficient
    ///     capacity to activate an environment and run a task (that also needs resources)
    ///     on them at the same time, but not allocate the memory yet.
    ///   </para>
    ///   <para>
    ///     It is normally followed by a call to <see cref="Allocate" />, possibly only
    ///     to claim the environment memory but not the task memory in case the goal is to
    ///     activate an environment, while merely making sure it is put on resource units
    ///     that can later carry a task using that environment.
    ///   </para>
    /// </remarks>
    public: bool Pick(
      std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
      const ResourceManifestPointer &primaryResources,
      const ResourceManifestPointer &secondaryResources = ResourceManifestPointer()
    ) const;

    /// <summary>Allocates the specified resouces in the budget if possible</summary>
    /// <param name="inOutUnitIndices">
    ///   <para>
    ///     On input, unit indices that *must* be used (i.e. to select a certain GPU).
    ///     Provide an empty vector or set its contents to std::size_t(-1) in order to
    ///     allow any unit to be selected.
    ///   </para>
    ///   <para>
    ///     Upon return, receives the actually selected unit indices. Tasks must honor
    ///     the unit selections and pick i.e. the correct GPU (the ordering of units
    ///     always matches the order by which <see cref="HardwareMetrics" /> lists units).
    ///   </para>
    /// </param>
    /// <param name="environment">Task environment whose resources will be blocked</param>
    /// <param name="taskResources">Task-specific resources that will be blocked</param>
    /// <returns>
    ///   True if the budget had enough reserves to allocate all requested resources,
    ///   false if one or more resources were insufficient and nothing was blocked at all.
    /// </returns>
    public: bool Allocate(
      std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
      const std::shared_ptr<TaskEnvironment> &environment,
      const ResourceManifestPointer &taskResources = ResourceManifestPointer()
    );

    /// <summary>Allocates the specified resouces in the budget if possible</summary>
    /// <param name="inOutUnitIndices">
    ///   <para>
    ///     On input, unit indices that *must* be used (i.e. to select a certain GPU).
    ///     Provide an empty vector or set its contents to std::size_t(-1) in order to
    ///     allow any unit to be selected.
    ///   </para>
    ///   <para>
    ///     Upon return, receives the actually selected unit indices. Tasks must honor
    ///     the unit selections and pick i.e. the correct GPU (the ordering of units
    ///     always matches the order by which <see cref="HardwareMetrics" /> lists units).
    ///   </para>
    /// </param>
    /// <param name="primaryResources">First resource set that will be blocked</param>
    /// <param name="secondaryResources">Second resource set that will also be blocked</param>
    /// <returns>
    ///   True if the budget had enough reserves to allocate all requested resources,
    ///   false if one or more resources were insufficient and nothing was blocked at all.
    /// </returns>
    public: bool Allocate(
      std::array<std::size_t, MaximumResourceType + 1> &inOutUnitIndices,
      const ResourceManifestPointer &primaryResources,
      const ResourceManifestPointer &secondaryResources = ResourceManifestPointer()
    );

    /// <summary>Returns the specified resouces to the budget</summary>
    /// <param name="allocatedUnitIndices">
    ///   Unit indices that were returned by <see cref"Allocate" /> and to which
    ///   the resources will be released again.
    /// </param>
    /// <param name="environment">Task environment whose resources will be returned</param>
    /// <param name="taskResources">Task-specific resources that will be returned</param>
    public: void Release(
      const std::array<std::size_t, MaximumResourceType + 1> &allocatedUnitIndices,
      const std::shared_ptr<TaskEnvironment> &environment,
      const ResourceManifestPointer &taskResources = ResourceManifestPointer()
    );

    /// <summary>Returns the specified resouces to the budget</summary>
    /// <param name="allocatedUnitIndices">
    ///   Unit indices that were returned by <see cref"Allocate" /> and to which
    ///   the resources will be released again.
    /// </param>
    /// <param name="primaryResources">First resource set that will be returned</param>
    /// <param name="secondaryResources">Second resource set that will also be returned</param>
    public: void Release(
      const std::array<std::size_t, MaximumResourceType + 1> &allocatedUnitIndices,
      const ResourceManifestPointer &primaryResources,
      const ResourceManifestPointer &secondaryResources = ResourceManifestPointer()
    );

    /// <summary>Checks whether it is at all possible to execute a task</summary>
    /// <param name="environment">Environment needed to execute the task</param>
    /// <param name="taskResources">Resources the task needs to execute</param>
    /// <returns>True if the task is possible to execute</returns>
    public: bool CanEverExecute(
      const std::shared_ptr<TaskEnvironment> &environment,
      const ResourceManifestPointer &taskResources = ResourceManifestPointer()
    ) const;

    /// <summary>Checks whether it is at all possible to execute a task</summary>
    /// <param name="primaryResources">Resources from the task's environment</param>
    /// <param name="secondaryResources">Resources the task needs to execute</param>
    /// <returns>True if the task is possible to execute</returns>
    public: bool CanEverExecute(
      const ResourceManifestPointer &primaryResources,
      const ResourceManifestPointer &secondaryResources = ResourceManifestPointer()
    ) const;

    /// <summary>Checks whether the task can be executed right now</summary>
    /// <param name="environment">Environment needed to execute the task</param>
    /// <param name="taskResources">Resources the task needs to execute</param>
    /// <returns>True if the task can currently be executed</returns>
    public: bool CanExecuteNow(
      const std::shared_ptr<TaskEnvironment> &environment,
      const ResourceManifestPointer &taskResources = ResourceManifestPointer()
    ) const;

    /// <summary>Checks whether the task can be executed right now</summary>
    /// <param name="primaryResources">Resources from the task's environment</param>
    /// <param name="secondaryResources">Resources the task needs to execute</param>
    /// <returns>True if the task can currently be executed</returns>
    public: bool CanExecuteNow(
      const ResourceManifestPointer &primaryResources,
      const ResourceManifestPointer &secondaryResources = ResourceManifestPointer()
    ) const;

    #pragma region struct UsableResource

    /// <summary>Informations about a resource that has been added to the budget</summary>
    private: struct UsableResource {

      /// <summary>Number of units of this resource (i.e. GPUs or hard drives)</summary>
      public: std::size_t UnitCount;
      /// <summary>Highest total amount of this resource any unit can provide</summary>
      public: std::size_t HighestTotal;
      /// <summary>Total amounts of this resource per unit</summary>
      public: std::size_t *Total;
      /// <summary>Remaining amount of this resource per unit</summary>
      public: std::atomic_size_t *Remaining;

    };

    #pragma endregion // struct UsableResource

    /// <summary>Resources that are managed by the task coordinator</summary>
    private: std::array<UsableResource, MaximumResourceType + 1> resources;
    /// <summary>Hard drives that are currently blocked / allocated to a workload</summary>
    private: std::size_t busyHardDrives;
    /// <summary>Memory block storing all the resource counter arrays</summary>
    private: std::uint8_t *allocatedMemoryBlock;

    //private: std::uint8_t builtInMemory[256];

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Videl::Tasks

#endif // NUCLEX_VIDEL_TASKS_RESOURCEBUDGET_H
