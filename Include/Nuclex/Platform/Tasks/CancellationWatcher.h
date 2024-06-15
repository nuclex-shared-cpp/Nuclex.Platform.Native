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

#ifndef NUCLEX_PLATFORM_TASKS_CANCELLATIONWATCHER_H
#define NUCLEX_PLATFORM_TASKS_CANCELLATIONWATCHER_H

#include "Nuclex/Platform/Config.h"

#include <Nuclex/Support/Errors/CanceledError.h>

#include <memory> // for std::enable_shared_from_this, std::shared_ptr
#include <atomic> // for std::atomic

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Passed to background processes to tell when when they should cancel</summary>
  class NUCLEX_PLATFORM_TYPE CancellationWatcher {

    /// <summary>Initializes a new cancellation watcher</summary>
    protected: NUCLEX_PLATFORM_API CancellationWatcher() :
      Canceled(false), CancellationReason() {}

    /// <summary>Frees all resources owned by the cancellation watcher</summary>
    public: NUCLEX_PLATFORM_API virtual ~CancellationWatcher() = default;

    /// <summary>Checks whether a cancellation has occured</summary>
    public: NUCLEX_PLATFORM_API bool IsCanceled() const {
      return this->Canceled.load(std::memory_order::memory_order_relaxed);
    }

    /// <summary>Throws an exception if a cancellation has occured</summary>
    public: NUCLEX_PLATFORM_API void ThrowIfCanceled() const {
      if(IsCanceled()) {
        std::atomic_thread_fence(std::memory_order::memory_order_acquire);
        throw Nuclex::Support::Errors::CanceledError(this->CancellationReason);
      }
    }

    /// <summary>Cancellation reason, doubles are cancellation flag if set</summary>
    protected: std::atomic<bool> Canceled;
    /// <summary>Why cancellation happened, optionally provided by the canceling side</summary>
    protected: std::string CancellationReason;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_CANCELLATIONWATCHER_H
