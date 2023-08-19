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

// This is sort of a nice idea, but also a bit cumbersome to use
//
// .NET task use this concept, but the only advantage this gives is that lambdas and
// any random junk can be executed as a task ad-hoc without having to inherit from
// a task base class to support cancellation.
//
// I think requiring classes to be types gives more advantages than externalizing
// the concept of cancellation. For ad-hoc junk, you can still make a task that ignores
// cancellation (or passes it to the ad-hoc junk) and wrap that.
//
#ifndef NUCLEX_PLATFORM_TASKS_CANCELLATIONTRIGGER_H
#define NUCLEX_PLATFORM_TASKS_CANCELLATIONTRIGGER_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Tasks/CancellationWatcher.h"

#include <cassert> // for assert()

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Allows cancelling all tasks holding the trigger's cancellation watcher</summary>
  /// <remarks>
  ///   <para>
  ///     This is very similar to Microsoft's concept of &quot;cancellation tokens&quot;
  ///     found in their PPL, C++ REST SDK and in .NET
  ///   </para>
  ///   <para>
  ///     Basically, the initial launcher of a background task provides the task with
  ///     a cancellation watcher (as a parameter to the initiating method). The task is
  ///     then supposed to hold onto the cancellation watcher and stop running when
  ///     the cancellation watcher's <see cref="CancellationWatcher.IsCanceled" />
  ///     property is set to true (by sporadically checking it).
  ///   </para>
  /// </remarks>
  class CancellationTrigger : protected CancellationWatcher {

    /// <summary>
    ///   Builds a new cancellation trigger, required to prevent stack allocations
    /// </suimmary>
    /// <returns>The new cancellation trigger</returns>
    public: static std::shared_ptr<CancellationTrigger> Create() {
      struct ConstructibleCancellationTrigger : public CancellationTrigger {
        ConstructibleCancellationTrigger() = default;
        virtual ~ConstructibleCancellationTrigger() override = default;
      };
      std::shared_ptr<CancellationTrigger> result(
        std::move(std::make_shared<ConstructibleCancellationTrigger>())
      );

      #if 0
      // This seems to be a spot where compilers may report an error.
      // The reason being that, unlike the CancellationTrigger::Create() method,
      // std::static_pointer_cast() does not have access to protected members of
      // this class, and that includes the protected base class, unfortunately.
      result->watcher = (
        std::static_pointer_cast<CancellationWatcher, CancellationTrigger>(result)
      );
      #else
      // This is pure shared_ptr villainy, but for single-inheritance,
      // the pointer to base and derived will be at the same address
      // on all supported compilers, so we can do this and avoid having
      // to declare the base class as public (which would suck!)
      //
      // Make a weak_ptr to a CancellationWatcher that's actually us in disguise
      result->watcher = *reinterpret_cast<std::shared_ptr<CancellationWatcher> *>(&result);
      #endif

      return result;
    }

    /// <summary>Initializes a new cancellation trigger</summary>
    protected: CancellationTrigger() = default;

    /// <summary>Frees all resources used by the cancellation trigger</summary>
    public: virtual ~CancellationTrigger() override = default;

    /// <summary>Returns the trigger's cancellation watcher</summary>
    /// <returns>The cancellation watcher responding to the trigger</returns>
    public: std::shared_ptr<const CancellationWatcher> GetWatcher() const {
      return this->watcher.lock();
    }

    /// <summary>Triggers the cancellation, signaling the watcher</summary>
    /// <param name="reason">
    ///   Optional reason for the cancellation, included in exception message when
    ///   <see cref="CancellationWatcher.ThrowIfCanceled" /> is used.
    /// </param>
    public: void Cancel(const std::string &reason = std::string()) {
      assert((IsCanceled() == false) && u8"Cancellation is triggered only once");

      this->CancellationReason = reason;
      std::atomic_thread_fence(std::memory_order::memory_order_release);
      this->Canceled.store(true, std::memory_order::memory_order_relaxed);
    }

    /// <summary>Watcher handed out by the <see cref="GetWatcher" /> method</summary>
    /// <remarks>
    ///   This is actually a weak_ptr to this instance itself under its base class.
    ///   It cannot be a shared_ptr since then, the instance would self-reference,
    ///   preventing the reference counter from ever reaching zero, causing a memory leak.
    /// </remarks>
    private: std::weak_ptr<CancellationWatcher> watcher;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_CANCELLATIONTRIGGER_H
