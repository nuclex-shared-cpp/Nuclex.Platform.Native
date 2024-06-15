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

#ifndef NUCLEX_PLATFORM_TASKS_TASKENVIRONMENT_H
#define NUCLEX_PLATFORM_TASKS_TASKENVIRONMENT_H

#include "Nuclex/Platform/Config.h"

#include <Nuclex/Support/Errors/CanceledError.h>

#include <string> // for std::string
#include <chrono> // for std::chrono::microseconds

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class ResourceManifest;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Environment required for a task to be executed</summary>
  /// <remarks>
  ///   <para>
  ///     Some tasks may require set-up work that needs to be performed once but applies
  ///     to multiple tasks. This can be represented as a task environment and will make
  ///     the task coordinator attempt to batch tasks that rely on the same environment.
  ///   </para>
  ///   <para>
  ///     Whether tasks reference the same environment is determined by the instance they
  ///     point to (so you'll have to actually store the environment and pass the exact same
  ///     instance to the task processor).
  ///   </para>
  /// </remarks>
  class NUCLEX_PLATFORM_TYPE TaskEnvironment {

    /// <summary>How long it will take to activate this task environment</summary>
    /// <remarks>
    ///   <para>
    ///     Estimate of the time it would take to call the <see cref="Activate" /> method.
    ///     This is intended for environments that need to perform a non-trivial setup
    ///     before tasks using them can be executed. This could be uploading a neural network
    ///     to the GPU or loading some data set.
    ///   </para>
    ///   <para>
    ///     The task processor will try to form series of tasks using the same environment
    ///     if they have a setup duration. If the duration is now known, an educated guess
    ///     is appropriate, to be refined after setting up the environment once.
    ///   </para>
    /// </remarks>
    public: std::chrono::microseconds ActivationDuration;

    /// <summary>How long it will take to shut down this task environment</summary>
    /// <remarks>
    ///   If resources are sufficient to activate multiple task environments, the task
    ///   processor will do so. Otherwise, it adds the shutdown duration to the switching
    ///   cost of activating another environment to better decide the order in which
    ///   tasks should be run.
    /// </remarks>
    public: std::chrono::microseconds ShutdownDuration;

    /// <summary>Resources that this task environment will consume while active</summary>
    public: std::shared_ptr<ResourceManifest> Resources;

    /// <summary>Activates the task environment</summary>
    /// <remarks>
    ///   Here the environment can upload neural networks to the GPU, read supporting files
    ///   into memory or start external processes needed to perform its work.
    /// </remarks>
    public: NUCLEX_PLATFORM_API virtual void Activate() {}

    /// <summary>Shuts the task environment down</summary>
    /// <remarks>
    ///   This is called either when the environment has to be shut down in order to load
    ///   another environment or when all tasks are done or when the application is quitting.
    ///   When called, the environment should free all resources to make them available
    ///   for other environments or other applications the user wishes to.
    /// </remarks>
    public: NUCLEX_PLATFORM_API virtual void Shutdown() {}

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_TASKENVIRONMENT_H
