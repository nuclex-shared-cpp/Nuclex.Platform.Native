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

#include "Nuclex/Platform/Tasks/CancellationWatcher.h"
#include "Nuclex/Platform/Tasks/CancellationTrigger.h"
#include <gtest/gtest.h>

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationWatcherTest, TriggerProvidesWatcher) {
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    std::shared_ptr<const CancellationWatcher> watcher = trigger->GetWatcher();
    EXPECT_NE(watcher.get(), nullptr);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationWatcherTest, WatcherIsSignalledByTrigger) {
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    std::shared_ptr<const CancellationWatcher> watcher = trigger->GetWatcher();
    EXPECT_FALSE(watcher->IsCanceled());
    trigger->Cancel();
    EXPECT_TRUE(watcher->IsCanceled());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationWatcherTest, WatcherCanLifePastTrigger) {
    std::shared_ptr<const CancellationWatcher> watcher;
    {
      std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
      watcher = trigger->GetWatcher();
      trigger->Cancel();
      trigger.reset();
    }

    EXPECT_TRUE(watcher->IsCanceled());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationWatcherTest, WatcherThrowsCanceledErrorWhenCanceled) {
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    std::shared_ptr<const CancellationWatcher> watcher = trigger->GetWatcher();
    trigger->Cancel();

    EXPECT_THROW(
      watcher->ThrowIfCanceled(),
      Nuclex::Support::Errors::CanceledError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationWatcherTest, CanceledErrorInheritsFutureError) {
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    std::shared_ptr<const CancellationWatcher> watcher = trigger->GetWatcher();
    trigger->Cancel();

    EXPECT_THROW(
      watcher->ThrowIfCanceled(),
      std::future_error
    );
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
