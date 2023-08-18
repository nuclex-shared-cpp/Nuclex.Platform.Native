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

#ifndef NUCLEX_PLATFORM_TASKS_THREADEDTASK_H
#define NUCLEX_PLATFORM_TASKS_THREADEDTASK_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Tasks/Task.h" // for Task

#include <atomic> // for std::atomic

namespace Nuclex { namespace Support { namespace Threading {

  // ------------------------------------------------------------------------------------------- //

  class ThreadPool;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Support::Threading

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Task that uses multiple threads via the thread pool</summary>
  /// <remarks>
  ///   <para>
  ///     Use this class if you create tasks that perform work on multiple threads by
  ///     themselves (if, instead, you have a third party library that does its own
  ///     threading, use normal task and just set the resource manifest to the number
  ///     of CPU cores that the third-party library will use).
  ///   </para>
  ///   <para>
  ///     Your RunThreaded() method will be called on the number of threads you specify.
  ///   </para>
  /// </remarks>
  class ThreadedTask : public Task {

    /// <summary>Initializes a threaded task using the specified number of threads</summary>
    /// <param name="threadPool">Thread pool that will run the task's workload</param>
    /// <param name="maximumThreadCount">
    ///   Maximum number of threads to use. This is the number of calls that will be made
    ///   to the <see cref="ThreadedRun" /> method. If the thread pool has fewer threads than
    ///   this number, then some of the <see cref="ThreadedRun" /> calls will happen
    ///   sequentially on the same thread (which )
    public: ThreadedTask(
      Nuclex::Support::Threading::ThreadPool &threadPool,
      std::size_t maximumThreadCount = std::size_t(-1)
    ) :
      threadPool(threadPool),
      maximumThreadCount(maximumThreadCount) {}

    /// <summary>Frees all resources owned by the task</summary>
    /// <remarks>
    ///   The task must be either finished or cancelled before it may be destroyed.
    /// </remarks>
    public: virtual ~ThreadedTask() = default;

    /// <summary>Executes the task, using the specified resource units</summary>
    /// <param name="resourceUnitIndices">
    ///   Indices of the resource units the task coordinator has assigned this task
    /// </param>
    /// <param name="cancellationWatcher">
    ///   Lets the task detect when it is requested to cancel its processing
    /// </param>
    public: void Run(
      const std::array<std::size_t, MaximumResourceType + 1> &resourceUnitIndices,
      const CancellationWatcher &cancellationWatcher
    ) override;

    /// <summary>
    ///   Called in parallel on the specified number of threads to perform the task's work
    /// </summary>
    /// <param name="resourceUnitIndices">
    ///   when you set up the task coordinator, you specify one or more &quot;units&quot;
    ///   that provide each resources (likely you'll only add one &quot;SystemMemory&quot;
    ///   unit but there may be one &quot;VideoMemory&quot; unit per GPU in the system).
    ///   This array tells your task which resource units the task coordinator wants it
    ///   to use. Feel free to ignore this if you only ever have one unit of each resource.
    /// </param>
    /// <param name="cancellationWatcher">
    ///   Can be used to figure out whether the task has been cancelled. This will happen
    ///   if the task coordinator is shut down or cleared while tasks are executing.
    ///   Any task that takes longer than a couple of milliseconds should check for
    ///   cancellation at regular intervals to ensure the task coordinator isn't clogged.
    /// </param>
    protected: virtual void ThreadedRun(
      const std::array<std::size_t, MaximumResourceType + 1> &resourceUnitIndices,
      const CancellationWatcher &cancellationWatcher
    ) = 0;

    /// <summary>
    ///   Used internally to calls the ThreadedRun() method the a thread pool thread
    /// </summary>
    /// <param name="self">The 'this' pointer of the threaded task instance</param>
    /// <param name="resourceUnitIndices">
    ///   Indices of the resource units the task coordinator has assigned this task
    /// </param>
    /// <param name="cancellationWatcher">
    ///   Lets the task detect when it is requested to cancel its processing
    /// </param>
    /// <remarks>
    ///   This method is used rather than std::bind() in order to not pollute the call stack
    ///   and avoid the use of needless lambda functors in the thread pool callbacks.
    /// </remarks>
    private: static void invokeThreadedRun(
      ThreadedTask *self,
      const std::array<std::size_t, MaximumResourceType + 1> *resourceUnitIndices,
      const CancellationWatcher *cancellationWatcher
    );

    /// <summary>Thread pool that will be used to run work in multiple threads</summary>
    private: Nuclex::Support::Threading::ThreadPool &threadPool;
    /// <summary>Maximum number of threads that the task will use</summary>
    private: std::size_t maximumThreadCount;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_THREADEDTASK_H
