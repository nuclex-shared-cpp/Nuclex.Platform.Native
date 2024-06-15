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
    constexpr const std::size_t MaximumStackMemoryAllowed = 256; // bytes

    if(maximumThreadCount >= 2) {
      const std::size_t requiredMemory = (
        sizeof(std::future<void>[2]) * this->maximumThreadCount / 2
      );
      if(requiredMemory >= MaximumStackMemoryAllowed) { // Heap-allocated
        std::unique_ptr<std::uint8_t[]> memory(new std::uint8_t[requiredMemory]);
        FutureJoiner joiner(reinterpret_cast<std::future<void> *>(memory.get()));

        // Launch all threads and remember their 'std::future's they return
        for(std::size_t index = 0; index < this->maximumThreadCount; ++index) {
          new(reinterpret_cast<std::future<void> *>(memory.get()) + index) std::future(
            this->threadPool.Schedule(
              &ThreadedTask::invokeThreadedRun, this, &resourceUnitIndices, &cancellationWatcher
            )
          );
          ++joiner.FutureCount;
        }
      } else { // Stack-allocated
        std::uint8_t *memory = reinterpret_cast<std::uint8_t *>(alloca(requiredMemory));
        FutureJoiner joiner(reinterpret_cast<std::future<void> *>(memory));

        // Launch all threads and remember their 'std::future's they return
        for(std::size_t index = 0; index < this->maximumThreadCount; ++index) {
          new(reinterpret_cast<std::future<void> *>(memory) + index) std::future(
            this->threadPool.Schedule(
              &ThreadedTask::invokeThreadedRun, this, &resourceUnitIndices, &cancellationWatcher
            )
          );
          ++joiner.FutureCount;
        }
      }
    } else { // Single task (I'll slap you if this runs in production code!)
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
