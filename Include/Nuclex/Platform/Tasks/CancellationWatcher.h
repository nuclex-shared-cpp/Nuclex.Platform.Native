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
