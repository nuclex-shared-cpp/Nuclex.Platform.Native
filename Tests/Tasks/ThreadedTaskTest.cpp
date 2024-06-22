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
#include "Nuclex/Platform/Tasks/ResourceType.h"

#include <Nuclex/Support/Threading/StopSource.h> // for StopSource
#include <Nuclex/Support/Threading/ThreadPool.h>

#include <gtest/gtest.h>

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Mock task used to test the ThreadedTask wrapper</summary>
  class TestTask : public Nuclex::Platform::Tasks::ThreadedTask {

    /// <summary>Initializes a new mock task</summary>
    /// <param name="threadPool">Thread pool that will run the task's workload</param>
    /// <param name="maximumThreadCount">Maximum number of threads to use</param>
    public: TestTask(
      Nuclex::Support::Threading::ThreadPool &threadPool,
      std::size_t maximumThreadCount = std::size_t(-1)
    ) :
      ThreadedTask(threadPool, maximumThreadCount),
      RunCounter(0) {}

    /// <summary>
    ///   Called in parallel on the specified number of threads to perform the task's work
    /// </summary>
    /// <summary>Executes the task, using the specified resource units</summary>
    /// <param name="resourceUnitIndices">
    ///   Indices of the resource units the task coordinator has assigned this task
    /// </param>
    /// <param name="stopToken">
    ///   Lets the task detect when it is requested to cancel its processing
    /// </param>
    protected: void ThreadedRun(
      const Nuclex::Platform::Tasks::ResourceUnitArray &resourceUnitIndices,
      const Nuclex::Support::Threading::StopToken &stopToken
    ) noexcept override {
      (void)resourceUnitIndices;
      (void)stopToken;
      this->RunCounter.fetch_add(1, std::memory_order_acq_rel);
    }

    /// <summary>Number of times the <see cref="ThreadedRun" /> method has been called</summary>
    public: std::atomic<std::size_t> RunCounter;

  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  TEST(ThreadedTaskTest, CanRunSingleThreaded) {
    Nuclex::Support::Threading::ThreadPool tp(1, 1);

    std::shared_ptr<Nuclex::Support::Threading::StopSource> source = (
      Nuclex::Support::Threading::StopSource::Create()
    );
    {
      TestTask test(tp, 1);

      std::array<std::size_t, MaximumResourceType + 1> units;
      const Nuclex::Support::Threading::StopToken &token = *source->GetToken().get();
      test.Run(units, token);

      EXPECT_EQ(test.RunCounter.load(std::memory_order::memory_order_acquire), 1U);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ThreadedTaskTest, CanRunLowThreaded) {
    Nuclex::Support::Threading::ThreadPool tp(4, 4);

    std::shared_ptr<Nuclex::Support::Threading::StopSource> source = (
      Nuclex::Support::Threading::StopSource::Create()
    );
    {
      TestTask test(tp, 4);

      std::array<std::size_t, MaximumResourceType + 1> units;
      const Nuclex::Support::Threading::StopToken &token = *source->GetToken().get();
      test.Run(units, token);

      EXPECT_EQ(test.RunCounter.load(std::memory_order::memory_order_acquire), 4U);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ThreadedTaskTest, CanRunHighThreaded) {
    Nuclex::Support::Threading::ThreadPool tp;

    std::shared_ptr<Nuclex::Support::Threading::StopSource> source = (
      Nuclex::Support::Threading::StopSource::Create()
    );
    {
      TestTask test(tp, 32);

      std::array<std::size_t, MaximumResourceType + 1> units;
      const Nuclex::Support::Threading::StopToken &token = *source->GetToken().get();
      test.Run(units, token);

      EXPECT_EQ(test.RunCounter.load(std::memory_order::memory_order_acquire), 32U);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
