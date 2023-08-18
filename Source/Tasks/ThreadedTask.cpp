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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "Nuclex/Platform/Tasks/ThreadedTask.h"

#include <Nuclex/Support/Threading/ThreadPool.h> // for ThreadPool

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Calls .get() on a set of 'std::future' instances in an array</summary>
  class FutureJoiner {

    /// <summary>Initializes a new future joiner using the specified array</summary>
    /// <param name="futures">Array of futures on which to call the .get() method</param>
    public: FutureJoiner(std::future<void> *futures) :
      FutureCount(0),
      futures(futures) {}

    /// <summary>
    ///   Calls the .get() method on all futures that have been set up for the joiner
    /// </summary>
    public: ~FutureJoiner() {
      while(this->FutureCount >= 1) {
        --this->FutureCount;
        this->futures[this->FutureCount].get();
        this->futures[this->FutureCount].~future();
      }
    }
    
    /// <summary>Number of futures that have been constructed in the array</summary>
    public: std::size_t FutureCount;

    /// <summary>Array of futures that will be freed on destruction</summary>
    private: std::future<void> *futures;

  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  void ThreadedTask::Run(
    const std::array<std::size_t, MaximumResourceType + 1> &resourceUnitIndices,
    const CancellationWatcher &cancellationWatcher
  ) noexcept {
    if(maximumThreadCount * sizeof(std::future<void>[2]) >= 256) { // Stack-allocated
      const std::size_t requiredMemory = (
        sizeof(std::future<void>[2]) * this->maximumThreadCount / 2
      );
      std::uint8_t *memory = reinterpret_cast<std::uint8_t *>(alloca(requiredMemory));

      FutureJoiner joiner(reinterpret_cast<std::future<void> *>(memory));

      // Launch all threads and remember their 'std::future's they return
      for(std::size_t index = 0; index < this->maximumThreadCount; ++index) {
        new(reinterpret_cast<std::future<void> *>(memory + index)) std::future(
          std::move(
            this->threadPool.Schedule(
              &ThreadedTask::invokeThreadedRun, this, &resourceUnitIndices, &cancellationWatcher
            )
          )
        );
        ++joiner.FutureCount;
      }
    } else if(maximumThreadCount >= 2) { // Heap-allocated
      const std::size_t requiredMemory = (
        sizeof(std::future<void>[2]) * this->maximumThreadCount / 2
      );
      std::unique_ptr<std::uint8_t[]> memory(new std::uint8_t[requiredMemory]);

      FutureJoiner joiner(reinterpret_cast<std::future<void> *>(memory.get()));

      // Launch all threads and remember their 'std::future's they return
      for(std::size_t index = 0; index < this->maximumThreadCount; ++index) {
        new(reinterpret_cast<std::future<void> *>(memory.get() + index)) std::future(
          std::move(
            this->threadPool.Schedule(
              &ThreadedTask::invokeThreadedRun, this, &resourceUnitIndices, &cancellationWatcher
            )
          )
        );
        ++joiner.FutureCount;
      }
    } else if(maximumThreadCount == 1) { // Single task (I'll slap you if this happens)
      ThreadedRun(resourceUnitIndices, cancellationWatcher);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void ThreadedTask::invokeThreadedRun(
    ThreadedTask *self,
    const std::array<std::size_t, MaximumResourceType + 1> *resourceUnitIndices,
    const CancellationWatcher *cancellationWatcher
  ) {
    self->ThreadedRun(*resourceUnitIndices, *cancellationWatcher);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
