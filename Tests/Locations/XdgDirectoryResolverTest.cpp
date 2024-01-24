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

#include "../../Source/Locations/XdgDirectoryResolver.h"
#include "../../Source/Platform/LinuxFileApi.h" // for JoinPaths()
#include <gtest/gtest.h>

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Method that does not return the requested environment variable</summary>
  /// <returns>False, because the environment variable just isn't returned</summary>
  bool dontGetEnvironmentVariable(const std::string &, std::string &) {
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnOutOfTreeConfigHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_CONFIG_HOME") {
      value.assign(u8"/some/strange/directory");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnAlternateConfigHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_CONFIG_HOME") {
      value.assign(u8"$HOME/.conf2");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnOutOfTreeDataHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_DATA_HOME") {
      value.assign(u8"/another/strange/directory");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnAlternateDataHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_DATA_HOME") {
      value.assign(u8"$HOME/.roaming");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnOutOfTreeStateHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_STATE_HOME") {
      value.assign(u8"/yet/another/directory");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnAlternateStateHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_STATE_HOME") {
      value.assign(u8"$HOME/.state2");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnOutOfTreeCacheHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_CACHE_HOME") {
      value.assign(u8"/tmp/cache");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool returnAlternateCacheHomeDirectory(const std::string &name, std::string &value) {
    if(name == u8"XDG_CACHE_HOME") {
      value.assign(u8"$HOME/.tmp/cache");
      return true;
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateHomeDirectory) {
    std::string homeDirectory = XdgDirectoryResolver::GetHomeDirectory();
    EXPECT_FALSE(homeDirectory.empty());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateDefaultConfigHomeDirectory) {
    XdgDirectoryResolver resolver(&dontGetEnvironmentVariable);

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".config"
    );

    ASSERT_STREQ(resolver.GetConfigHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, ConfigHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(&returnOutOfTreeConfigHomeDirectory);
    std::string expected(u8"/some/strange/directory");
    ASSERT_STREQ(resolver.GetConfigHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, ConfigHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(&returnAlternateConfigHomeDirectory);
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".conf2"
    );
    ASSERT_STREQ(resolver.GetConfigHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateDefaultDataHomeDirectory) {
    XdgDirectoryResolver resolver(&dontGetEnvironmentVariable);

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".local/share"
    );

    ASSERT_STREQ(resolver.GetDataHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, DataHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(&returnOutOfTreeDataHomeDirectory);
    std::string expected(u8"/another/strange/directory");
    ASSERT_STREQ(resolver.GetDataHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, DataHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(&returnAlternateDataHomeDirectory);
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".roaming"
    );
    ASSERT_STREQ(resolver.GetDataHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateDefaultStateHomeDirectory) {
    XdgDirectoryResolver resolver(&dontGetEnvironmentVariable);

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".local/state"
    );

    ASSERT_STREQ(resolver.GetStateHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, StateHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(&returnOutOfTreeStateHomeDirectory);
    std::string expected(u8"/yet/another/directory");
    ASSERT_STREQ(resolver.GetStateHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, StateHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(&returnAlternateStateHomeDirectory);
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".state2"
    );
    ASSERT_STREQ(resolver.GetStateHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateDefaultCacheHomeDirectory) {
    XdgDirectoryResolver resolver(&dontGetEnvironmentVariable);

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".cache"
    );

    ASSERT_STREQ(resolver.GetCacheHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CacheHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(&returnOutOfTreeCacheHomeDirectory);
    std::string expected(u8"/tmp/cache");
    ASSERT_STREQ(resolver.GetCacheHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CacheHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(&returnAlternateCacheHomeDirectory);
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".tmp/cache"
    );
    ASSERT_STREQ(resolver.GetCacheHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
