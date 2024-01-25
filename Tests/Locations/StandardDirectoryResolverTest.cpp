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
