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

#ifndef NUCLEX_PLATFORM_TASKS_TASK_H
#define NUCLEX_PLATFORM_TASKS_TASK_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Tasks/ResourceType.h" // for ResourceType enum

#include <array> // for std:;array
#include <memory> // for std::shared_ptr

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
  class NUCLEX_PLATFORM_TYPE Task {

    /// <summary>Frees all resources owned by the task</summary>
    /// <remarks>
    ///   The task must be either finished or cancelled before it may be destroyed.
    /// </remarks>
    public: NUCLEX_PLATFORM_API virtual ~Task() = default;

    /// <summary>Resources that this task will consume while it runs</summary>
    public: std::shared_ptr<ResourceManifest> Resources;

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
    public: NUCLEX_PLATFORM_API virtual void Run(
      const std::array<std::size_t, MaximumResourceType + 1> &resourceUnitIndices,
      const CancellationWatcher &cancellationWatcher
    ) noexcept = 0;

  };

  // ------------------------------------------------------------------------------------------- //

  typedef std::array<std::size_t, MaximumResourceType + 1> ResourceUnitArray;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_TASK_H
