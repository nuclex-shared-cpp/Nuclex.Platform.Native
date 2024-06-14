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

#ifndef NUCLEX_PLATFORM_TASKS_NAIVETASKCOORDINATOR_H
#define NUCLEX_PLATFORM_TASKS_NAIVETASKCOORDINATOR_H

#include "Nuclex/Platform/Config.h"

#include "Nuclex/Platform/Tasks/TaskCoordinator.h"
#include <Nuclex/Support/Threading/ThreadPool.h> // for ThreadPool
#include <Nuclex/Support/Threading/Semaphore.h> // for Semaphore

#include <optional> // for std::optional
#include <memory> // for std::unique_ptr
#include <mutex> // for std::mutex
#include <deque> // for std::deque
#include <array> // for std::array
#include <atomic> // for std::atomic

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class ResourceBudget;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Coordinates background tasks based on their usage of system resouces</summary>
  class NUCLEX_PLATFORM_TYPE NaiveTaskCoordinator : public TaskCoordinator {

    /// <summary>Initializes a new task coordinator</summary>
    public: NUCLEX_PLATFORM_API NaiveTaskCoordinator();

    /// <summary>Frees all resources owned by the task coordinator</summary>
    public: NUCLEX_PLATFORM_API ~NaiveTaskCoordinator() override;

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
    public: NUCLEX_PLATFORM_API void AddResource(
      Tasks::ResourceType resourceType, std::size_t amountAvailable
    );

    /// <summary>Begins execution of scheduled tasks</summary>
    /// <remarks>
    ///   After this method is called, the <see cref="AddResources" /> method must not be
    ///   called anymore. It is okay to schedule tasks before calling Start(), however.
    ///   These would simply sit in a queue and begin execution right after Start() is called.
    /// </remarks>
    public: NUCLEX_PLATFORM_API void Start();

    /// <summary>Queries the amount of a resource the system has in total</summary>
    /// <param name="resourceType">Type of resource that will be queried</param>
    /// <returns>The total amount of the queried resource in the system</returns>
    /// <remarks>
    ///   If there are multiple resource units, for example on a system with multiple GPUs,
    ///   querying for video memory will return the highest amount of video memory available
    ///   on any single GPU. The behavior is the same for all resource units.
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::size_t QueryResourceMaximum(
      ResourceType resourceType
    ) const override;

    /// <summary>Schedules the specified task for execution</summary>
    /// <param name="task">Task that will be executed as soon as resources permit</param>
    /// <param name="requiredResources">Resources that the task will occupy</param>
    public: NUCLEX_PLATFORM_API void Schedule(const std::shared_ptr<Task> &task) override;

    /// <summary>Schedules the specified task for execution</summary>
    /// <param name-"environment">
    ///   Environment that needs to be active while the task executes
    /// </param>
    /// <param name="task">Task that will be executed as soon as resources permit</param>
    public: NUCLEX_PLATFORM_API void Schedule(
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
    public: NUCLEX_PLATFORM_API void ScheduleWithAlternative(
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
    public: NUCLEX_PLATFORM_API void ScheduleWithAlternative(
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
    public: NUCLEX_PLATFORM_API bool Cancel(const std::shared_ptr<Task> &task) override;

    /// <summary>Cancels all waiting tasks</summary>
    /// <param name="forever">Whether to cancel all future tasks, too</param>
    /// <remarks>
    ///   Is usually called when the task coordinator shuts down to cancel all waiting tasks
    /// </remarks>
    public: NUCLEX_PLATFORM_API void CancelAll(bool forever = true) override;

    /// <summary>Fast check whether the coordination thread needs to be waken up</summary>
    /// <param name="task">Task the task coordinator has added to the queue</param>
    /// <param name="environment">Environment required by the task to run</param>
    /// <returns>True if the coordination thread should be woken up</returns>
    protected: virtual bool IsCoordinationThreadWakeUpNeeded(
      const std::shared_ptr<Task> &task,
      const std::shared_ptr<TaskEnvironment> &environment = std::shared_ptr<TaskEnvironment>()
    ) const { return true; }

    /// <summary>Looks for runnable tasks and launches them</summary>
    protected: virtual void KickOffRunnableTasks();

    /// <summary>Thread that launches incoming tasks acoording to available resources</summary>
    private: void coordinationThread();

    /// <summary>
    ///   Helper that calls the <see cref="coordinateAndKickOffIncomingTasks" /> method
    /// </summary>
    /// <param name="self">The 'this' pointer of the task coordinator instance</param>
    private: static void invokeCoordinationThread(NaiveTaskCoordinator *self);

    #pragma region class ScheduledTask

    /// <summary>Task that is waiting to be executed</summary>
    private: class ScheduledTask {

      /// <summary>Initializes a new scheduled task</summary>
      /// <param name="task">Task that will be wrapped as a scheduled task</param>
      /// <param name="environment">Environment that is needed for the task for run</param>
      public: ScheduledTask(
        const std::shared_ptr<Task> &task,
        const std::shared_ptr<TaskEnvironment> &environment = std::shared_ptr<TaskEnvironment>()
      ) :
        PrimaryEnvironment(environment),
        PrimaryTask(task),
        AssignedResourceIndices() {}

      /// <summary>Environment that needs to be active for the task, can be empty</summary>
      public: std::shared_ptr<TaskEnvironment> PrimaryEnvironment;
      /// <summary>Task to be executed</summary>
      public: std::shared_ptr<Task> PrimaryTask;
      /// <summary>The indices of the resource units assigned to this task</summary>
      /// <remarks>
      ///   When there are multiple units providing a resource (for example, multiple GPUs),
      ///   then the task coordinator has to decide which one to run the task on. This list
      ///   will be filled when the task is launched to remember which of the units the task
      ///   has been told to use so it can be freed again correctly.
      /// </remarks>
      public: std::array<std::size_t, MaximumResourceType + 1> AssignedResourceIndices;

      //public: std::uint8_t awaitedFuture[(sizeof(std::future<void>[2]) / 2)];

    };

    #pragma endregion // class ScheduledTask

    #pragma region struct ActiveEnvironment

    /// <summary>Environment that has been activated by the task coordinator</summary>
    private: struct ActiveEnvironment {

      /// <summary>Task environment that is currently active</summary>
      public: std::shared_ptr<TaskEnvironment> Environment;
      /// <summary>Units that have been selected to provide the resources</summary>
      public: std::array<std::size_t, MaximumResourceType + 1> SelectedUnits;
      /// <summary>Number of tasks that are using this environment right now</summary>
      public: std::size_t ActiveTaskCount;

    };

    #pragma endregion // struct ActiveEnvironment

    /// <summary>Tracks the resources available on the system</summary>
    private: std::unique_ptr<ResourceBudget> availableResources;
    /// <summary>Number of CPU cores that have been added as resources in total</summary>
    private: std::size_t totalCpuCoreCount;
    
    /// <summary>Thread pool used to start off the scheduled tasks</summary>
    /// <remarks>
    ///   Only optional so it can be constructed at a later time. Is set when
    ///   the <see cref="Start" /> method is called. Contains as many ready threads
    ///   as there are cpu cores added to the task coordinator.
    /// </remarks>
    private: std::optional<Nuclex::Support::Threading::ThreadPool> threadPool;

    /// <summary>Set after the coordination thread was started</summary>
    private: std::atomic<bool> coordinationThreadRunningFlag;
    /// <summary>Memory for the std::future that tracks the coordination thread</summary>
    private: std::uint8_t coordinationThreadFuture[sizeof(std::future<void>)];
    /// <summary>Set to shut down the coordination thread</summary>
    private: std::atomic<bool> coordinationThreadShutdownFlag;

    /// <summary>Mutex that must be held when accessing the task queues</summary>
    private: std::mutex queueAccessMutex;
    /// <summary>Tasks that are waiting to be executed by the task coordinator</summary>
    private: std::deque<ScheduledTask> waitingTasks;
    /// <summary>Semaphore that gets posted once for each available task</summary>
    /// <remarks>
    ///   Also gets posted for a silly number of tasks when a shutdown is requested.
    /// </remarks>
    private: Nuclex::Support::Threading::Semaphore tasksAvailableSemaphore;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_NAIVETASKCOORDINATOR_H
