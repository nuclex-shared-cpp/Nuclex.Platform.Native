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
#include "Nuclex/Platform/Tasks/ResourceType.h"
#include "Nuclex/Platform/Tasks/CancellationTrigger.h"

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
    /// <param name="cancellationWatcher">
    ///   Lets the task detect when it is requested to cancel its processing
    /// </param>
    protected: void ThreadedRun(
      const Nuclex::Platform::Tasks::ResourceUnitArray &resourceUnitIndices,
      const Nuclex::Platform::Tasks::CancellationWatcher &cancellationWatcher
    ) noexcept override {
      (void)resourceUnitIndices;
      (void)cancellationWatcher;
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
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    {
      TestTask test(tp, 1);

      std::array<std::size_t, MaximumResourceType + 1> units;
      const CancellationWatcher &watcher = *trigger->GetWatcher().get();
      test.Run(units, watcher);

      EXPECT_EQ(test.RunCounter.load(std::memory_order::memory_order_acquire), 1U);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ThreadedTaskTest, CanRunLowThreaded) {
    Nuclex::Support::Threading::ThreadPool tp(4, 4);
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    {
      TestTask test(tp, 4);

      std::array<std::size_t, MaximumResourceType + 1> units;
      const CancellationWatcher &watcher = *trigger->GetWatcher().get();
      test.Run(units, watcher);

      EXPECT_EQ(test.RunCounter.load(std::memory_order::memory_order_acquire), 4U);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ThreadedTaskTest, CanRunHighThreaded) {
    Nuclex::Support::Threading::ThreadPool tp;
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    {
      TestTask test(tp, 32);

      std::array<std::size_t, MaximumResourceType + 1> units;
      const CancellationWatcher &watcher = *trigger->GetWatcher().get();
      test.Run(units, watcher);

      EXPECT_EQ(test.RunCounter.load(std::memory_order::memory_order_acquire), 32U);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
