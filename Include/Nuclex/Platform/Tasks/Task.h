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

#ifndef NUCLEX_PLATFORM_TASKS_TASK_H
#define NUCLEX_PLATFORM_TASKS_TASK_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Tasks/ResourceType.h" // for ResourceType enum

#include <array> // for std:;array

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class CancellationWatcher;
  class ResourceManifest;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps work that can be scheduled on a task coordinator</summary>
  /// <remarks>
  ///   <para>
  ///     Tasks are the low-level pieces of work the application executes internally.
  ///     The main purpose of tasks is to control usage of computer resources from
  ///     individual tasks (so that GPU memory isn't used by two expensive tasks at
  ///     the same time and disk accesses aren't sequentialised for classical hard drives).
  ///   </para>
  /// </remarks>
  class Task {

    /// <summary>Frees all resources owned by the task</summary>
    /// <remarks>
    ///   The task must be either finished or cancelled before it may be destroyed.
    /// </remarks>
    public: virtual ~Task() = default;

    /// <summary>Reports the resources that the task will occupiy while it runs</summary>
    /// <returns>A resource manifest with the resources the task will occupy</returns>
    public: virtual ResourceManifest &GetUsedResources() const = 0;

    /// <summary>Executes the task, using the specified resource units</summary>
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
    /// <remarks>
    ///   <para>
    ///     When used with a task coordinator, this method is, of course, called in a thread
    ///     and is expected to block until the task has finished. It is okay if you perform
    ///     single-threaded processing in this thread, but then you should set your resource
    ///     manifest accordingly (1 CPU core occupied).
    ///   </para>
    ///   <para>
    ///     If the task's resource manifest states that it uses no CPU cores, then you must
    ///     only use the thread in which the <see cref="Run" /> method is  called for
    ///     managerial purposes, i.e. firing off a task on a GPU and waiting for completion.
    ///   </para>
    ///   <para>
    ///     If your tasks need any reusable data or wish to keep something in memory,
    ///     consider using a <see cref="TaskEnvironment" /> which can be activated or
    ///     shut down under the management of a task coordinate and provide persistent
    ///     data to any number of tasks.
    ///   </para>
    /// </remarks>
    public: virtual void Run(
      const std::array<std::size_t, MaximumResourceType + 1> &resourceUnitIndices,
      const CancellationWatcher &cancellationWatcher
    ) noexcept = 0;

  };

  // ------------------------------------------------------------------------------------------- //

  typedef std::array<std::size_t, MaximumResourceType + 1> ResourceUnitArray;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_TASK_H
