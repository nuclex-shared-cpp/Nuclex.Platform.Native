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

#include "Nuclex/Platform/Tasks/CancellationTrigger.h"
#include <gtest/gtest.h>

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationTriggerTest, IsConstructedViaConstructorMethod) {
    EXPECT_NO_THROW(
      std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationTriggerTest, TriggerStartsUncancelled) {
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();
    std::shared_ptr<const CancellationWatcher> watcher = trigger->GetWatcher();
    bool watcherIsCanceled = watcher->IsCanceled();
    EXPECT_FALSE(watcherIsCanceled);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationTriggerTest, CancellationSignalsWatcher) {
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();

    std::shared_ptr<const CancellationWatcher> watcher = trigger->GetWatcher();
    EXPECT_FALSE(watcher->IsCanceled());

    trigger->Cancel();
    EXPECT_TRUE(watcher->IsCanceled());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(CancellationTriggerTest, ExceptionMessageCanBeProvided) {
    std::shared_ptr<CancellationTrigger> trigger = CancellationTrigger::Create();

    std::shared_ptr<const CancellationWatcher> watcher = trigger->GetWatcher();
    EXPECT_FALSE(watcher->IsCanceled());

    trigger->Cancel(u8"This is my custom cancellation message");
    EXPECT_TRUE(watcher->IsCanceled());

    bool exceptionThrown = false;
    try {
      watcher->ThrowIfCanceled();
    }
    catch(const std::future_error &error) {
      exceptionThrown = true;
      EXPECT_STREQ(error.what(), u8"This is my custom cancellation message");
    }
    EXPECT_TRUE(exceptionThrown);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
