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

#ifndef NUCLEX_PLATFORM_TASKS_TASKCOORDINATOR_H
#define NUCLEX_PLATFORM_TASKS_TASKCOORDINATOR_H

#include "Nuclex/Platform/Config.h"

#include "Nuclex/Platform/Tasks/ResourceType.h"

#include <string> // for std::string
#include <atomic> // for std::atomic, std::atomic_thread_fence
#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class Task;
  class TaskEnvironment;
  class ResourceManifest;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Coordinates background tasks based on their usage of system resouces</summary>
  class NUCLEX_PLATFORM_TYPE TaskCoordinator {

    /// <summary>Frees all resources owned by the task coordinator</summary>
    public: NUCLEX_PLATFORM_API virtual ~TaskCoordinator() = default;

    /// <summary>Queries the amount of a resource the system has in total</summary>
    /// <param name="resourceType">Type of resource that will be queried</param>
    /// <returns>The total amount of the queried resource in the system</returns>
    /// <remarks>
    ///   If there are multiple resource units, for example on a system with multiple GPUs,
    ///   querying for video memory will return the highest amount of video memory available
    ///   on any single GPU. The behavior is the same for all resource units.
    /// </remarks>
    public: NUCLEX_PLATFORM_API virtual std::size_t QueryResourceMaximum(
      ResourceType resourceType
    ) const = 0;

    /// <summary>Schedules the specified task for execution</summary>
    /// <param name="task">Task that will be executed as soon as resources permit</param>
    /// <param name="requiredResources">Resources that the task will occupy</param>
    public: NUCLEX_PLATFORM_API virtual void Schedule(const std::shared_ptr<Task> &task) = 0;

    /// <summary>Schedules the specified task for execution</summary>
    /// <param name-"environment">
    ///   Environment that needs to be active while the task executes
    /// </param>
    /// <param name="task">Task that will be executed as soon as resources permit</param>
    public: NUCLEX_PLATFORM_API virtual void Schedule(
      const std::shared_ptr<TaskEnvironment> &environment,
      const std::shared_ptr<Task> &task
    ) = 0;

    /// <summary>Schedules a task for execution with an alternative task</summary>
    /// <param name="preferredTask">
    ///   Task that will be executed if the resources are available
    /// </param>
    /// <param name="alternativeTask">
    ///   Task that can be executed instead of the preferred resources are not available
    /// </param>
    public: NUCLEX_PLATFORM_API virtual void ScheduleWithAlternative(
      const std::shared_ptr<Task> &preferredTask,
      const std::shared_ptr<Task> &alternativeTask
    ) = 0;

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
    public: NUCLEX_PLATFORM_API virtual void ScheduleWithAlternative(
      const std::shared_ptr<TaskEnvironment> &environment,
      const std::shared_ptr<Task> &preferredTask,
      const std::shared_ptr<Task> &alternativeTask
    ) = 0;

    /// <summary>Gives priority to the specified task</summary>
    /// <param name="task">Already scheduled task that will be given priority</param>
    /// <returns>True if the task was found in the non-priority queue and prioritized</returns>
    /// <remarks>
    ///   This may be ignored but gives a hint of the task coordinator that the given task
    ///   should be executed out of order as soon as possible. Normally used for tasks that
    ///   set preconditions required to queue other tasks, i.e. extracting a batch of frames
    ///   via ffmpeg so that it can be processed.
    /// </remarks>
    public: NUCLEX_PLATFORM_API virtual bool Prioritize(const std::shared_ptr<Task> &task) {
      (void)task;
      return false; // By default, an implementation ignores prioritization
    }

    /// <summary>Cancels a waiting task</summary>
    /// <param name="task">Task that will be cancelled</param>
    /// <returns>
    ///   True if the task was still waiting and has been canceled, false if it wasn't found
    /// </returns>
    /// <remarks>
    ///   If the task has an alternative, that one will be cancelled, too. Specifying
    ///   the alternative for cancellation is not allowed.
    /// </remarks>
    public: NUCLEX_PLATFORM_API virtual bool Cancel(const std::shared_ptr<Task> &task) = 0;

    /// <summary>Cancels all waiting tasks</summary>
    /// <param name="forever">Whether to cancel all future tasks, too</param>
    /// <remarks>
    ///   Is usually called when the task coordinator shuts down to cancel all waiting tasks
    /// </remarks>
    public: NUCLEX_PLATFORM_API virtual void CancelAll(bool forever = true) = 0;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_TASKCOORDINATOR_H
