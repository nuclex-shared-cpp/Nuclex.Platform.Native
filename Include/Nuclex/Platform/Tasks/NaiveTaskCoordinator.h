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

#ifndef NUCLEX_PLATFORM_TASKS_NAIVETASKCOORDINATOR_H
#define NUCLEX_PLATFORM_TASKS_NAIVETASKCOORDINATOR_H

#include "Nuclex/Platform/Config.h"

#include "Nuclex/Platform/Tasks/TaskCoordinator.h"
#include <Nuclex/Support/Threading/ThreadPool.h> // for ThreadPool

#include <optional> // for std::optional
#include <memory> // for std::unique_ptr

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class ResourceBudget;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Coordinates background tasks based on their usage of system resouces</summary>
  class NaiveTaskCoordinator : public TaskCoordinator {

    /// <summary>Initializes a new task coordinator</summary>
    public: NaiveTaskCoordinator();

    /// <summary>Frees all resources owned by the task coordinator</summary>
    public: ~NaiveTaskCoordinator() override;

    /// <summary>Adds a resource that the task coordinator can allocate to tasks</summary>
    /// <param name="resourceType">Type of resource the task coordinator can allocate</param>
    /// <param name="amountAvailable">Amount of the resource available on the system</param>
    /// <remarks>
    ///   Calling this method multiple times with the same resource type will not accumulate
    ///   resources but instead handle it as an alternative resource unit (for example,
    ///   adds two times 16 GiB video memory does not allow the coordinator to run tasks
    ///   requiring 32 GiB video memory, but it will allow for two tasks requiring up to
    ///   16 GiB of video memory to run in parallel).
    /// </remarks>
    public: void AddResource(
      Tasks::ResourceType resourceType, std::size_t amountAvailable
    );

    /// <summary>Begins execution of scheduled tasks</summary>
    /// <remarks>
    ///   After this method is called, the <see cref="AddResources" /> method must not be
    ///   called anymore. It is okay to schedule tasks before calling Start(), however.
    ///   These would simply sit in a queue and begin execution right after Start() is called.
    /// </remarks>
    public: void Start();

    /// <summary>Queries the amount of a resource the system has in total</summary>
    /// <param name="resourceType">Type of resource that will be queried</param>
    /// <returns>The total amount of the queried resource in the system</returns>
    /// <remarks>
    ///   If there are multiple resource units, for example on a system with multiple GPUs,
    ///   querying for video memory will return the highest amount of video memory available
    ///   on any single GPU. The behavior is the same for all resource units.
    /// </remarks>
    public: std::size_t QueryResourceMaximum(ResourceType resourceType) const override;

    /// <summary>Schedules the specified task for execution</summary>
    /// <param name="task">Task that will be executed as soon as resources permit</param>
    /// <param name="requiredResources">Resources that the task will occupy</param>
    public: void Schedule(const std::shared_ptr<Task> &task) override;

    /// <summary>Schedules the specified task for execution</summary>
    /// <param name-"environment">
    ///   Environment that needs to be active while the task executes
    /// </param>
    /// <param name="task">Task that will be executed as soon as resources permit</param>
    public: void Schedule(
      const std::shared_ptr<TaskEnvironment> &environment,
      const std::shared_ptr<Task> &task
    ) override;

    /// <summary>Schedules a task for execution with an alternative task</summary>
    /// <param name="preferredTask">
    ///   Task that will be executed if the resources are available
    /// </param>
    /// <param name="alternativeTask">
    ///   Task that can be executed instead of the preferred resources are not available
    /// </param>
    public: void ScheduleWithAlternative(
      const std::shared_ptr<Task> &preferredTask,
      const std::shared_ptr<Task> &alternativeTask
    ) override;

    /// <summary>Schedules a task for execution with an alternative task</summary>
    /// <param name-"environment">
    ///   Environment that needs to be active while the task executes
    /// </param>
    /// <param name="preferredTask">
    ///   Task that will be executed if the resources are available
    /// </param>
    /// <param name="alternativeTask">
    ///   Task that can be executed instead of the preferred resources are not available
    /// </param>
    public: void ScheduleWithAlternative(
      const std::shared_ptr<TaskEnvironment> &environment,
      const std::shared_ptr<Task> &preferredTask,
      const std::shared_ptr<Task> &alternativeTask
    ) override;

    /// <summary>Cancels a waiting task</summary>
    /// <param name="task">Task that will be cancelled</param>
    /// <returns>
    ///   True if the task was still waiting and has been canceled, false if it wasn't found
    /// </returns>
    /// <remarks>
    ///   If the task has an alternative, that one will be cancelled, too. Specifying
    ///   the alternative for cancellation is not allowed.
    /// </remarks>
    public: bool Cancel(const std::shared_ptr<Task> &task) override;

    /// <summary>Cancels all waiting tasks</summary>
    /// <param name="forever">Whether to cancel all future tasks, too</param>
    /// <remarks>
    ///   Is usually called when the task coordinator shuts down to cancel all waiting tasks
    /// </remarks>
    public: void CancelAll(bool forever = true) override;

    /// <summary>Records  the resources available on the system</summary>
    private: std::unique_ptr<ResourceBudget> availableResources;

    /// <summary>Thread pool used to start off the scheduled tasks</summary>
    /// <remarks>
    ///   Only optional so it can be constructed at a later time. Is set when
    ///   the <see cref="Start" /> method is called. Contains as many ready threads
    ///   as there are cpu cores added to the task coordinator.
    /// </remarks>
    private: std::optional<Nuclex::Support::Threading::ThreadPool> threadPool;
    /// <summary>Number of CPU cores that have been added as resources in total</summary>
    private: std::size_t totalCpuCoreCount;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_NAIVETASKCOORDINATOR_H
