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

#include "Nuclex/Platform/Locations/StandardDirectoryResolver.h"
#include <gtest/gtest.h>

namespace {

  // ------------------------------------------------------------------------------------------- //

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //

  TEST(StandardDirectoryResolverTest, CanDetermineExecutableDirectory) {
    StandardDirectoryResolver resolver;
    std::string executableDirectory = resolver.GetExecutableDirectory();
    EXPECT_FALSE(executableDirectory.empty());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(StandardDirectoryResolverTest, CanDetermineSettingsDirectory) {
    StandardDirectoryResolver resolver;
    std::string settingsDirectory = resolver.GetSettingsDirectory();
    EXPECT_FALSE(settingsDirectory.empty());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(StandardDirectoryResolverTest, CanDetermineStateDirectory) {
    StandardDirectoryResolver resolver;
    std::string stateDirectory = resolver.GetStateDirectory();
    EXPECT_FALSE(stateDirectory.empty());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(StandardDirectoryResolverTest, CanDetermineCacheDirectory) {
    StandardDirectoryResolver resolver;
    std::string cacheDirectory = resolver.GetCacheDirectory();
    EXPECT_FALSE(cacheDirectory.empty());
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
