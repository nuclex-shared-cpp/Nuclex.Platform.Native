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
    EXPECT_FALSE(watcher->IsCanceled());
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
      EXPECT_EQ(
        std::string(error.what()), u8"This is my custom cancellation message"
      );
    }
    EXPECT_TRUE(exceptionThrown);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
