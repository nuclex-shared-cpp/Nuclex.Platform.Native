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

  /// <summary>
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

  bool dontReadEntireXdgUserDirsFile(std::string &contents) {
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  bool provideDummyXdgUserDirsFile(std::string &contents) {
    contents.assign(
      u8"XDG_DATA_HOME=\"/all/my/data\"\n"
      u8"#XDG_DATA_HOME=\"/this/is/commented/out\"\n"
      u8"\n"
      u8"XDG_STATE_HOME=\"/var/lib/my-service/state\"\n"
      u8"# This is just a comment\n"
      u8"XDG_CACHE_HOME=\"/var/cache/my-service\"\n"
    );

    return true;
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
    XdgDirectoryResolver resolver(&dontGetEnvironmentVariable, &dontReadEntireXdgUserDirsFile);

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".config"
    );

    EXPECT_STREQ(resolver.GetConfigHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, ConfigHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(
      &returnOutOfTreeConfigHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected(u8"/some/strange/directory");
    EXPECT_STREQ(resolver.GetConfigHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, ConfigHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(
      &returnAlternateConfigHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".conf2"
    );
    EXPECT_STREQ(resolver.GetConfigHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateDefaultDataHomeDirectory) {
    XdgDirectoryResolver resolver(
      &dontGetEnvironmentVariable, &dontReadEntireXdgUserDirsFile
    );

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".local/share"
    );

    EXPECT_STREQ(resolver.GetDataHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, DataHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(
      &returnOutOfTreeDataHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected(u8"/another/strange/directory");
    EXPECT_STREQ(resolver.GetDataHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, DataHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(
      &returnAlternateDataHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".roaming"
    );
    EXPECT_STREQ(resolver.GetDataHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateDefaultStateHomeDirectory) {
    XdgDirectoryResolver resolver(
      &dontGetEnvironmentVariable, &dontReadEntireXdgUserDirsFile
    );

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".local/state"
    );

    EXPECT_STREQ(resolver.GetStateHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, StateHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(
      &returnOutOfTreeStateHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected(u8"/yet/another/directory");
    EXPECT_STREQ(resolver.GetStateHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, StateHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(
      &returnAlternateStateHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".state2"
    );
    EXPECT_STREQ(resolver.GetStateHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CanLocateDefaultCacheHomeDirectory) {
    XdgDirectoryResolver resolver(
      &dontGetEnvironmentVariable, &dontReadEntireXdgUserDirsFile
    );

    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".cache"
    );

    EXPECT_STREQ(resolver.GetCacheHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CacheHomeDirectoryCanBeOverriddenWithFreePath) {
    XdgDirectoryResolver resolver(
      &returnOutOfTreeCacheHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected(u8"/tmp/cache");
    EXPECT_STREQ(resolver.GetCacheHomeDirectory().c_str(), expected.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, CacheHomeDirectoryCanBeOverriddenWithHomePath) {
    XdgDirectoryResolver resolver(
      &returnAlternateCacheHomeDirectory, &dontReadEntireXdgUserDirsFile
    );
    std::string expected = Platform::LinuxFileApi::JoinPaths(
      XdgDirectoryResolver::GetHomeDirectory(), u8".tmp/cache"
    );
    EXPECT_STREQ(resolver.GetCacheHomeDirectory().c_str(), expected.c_str());
  }


  // ------------------------------------------------------------------------------------------- //

  TEST(XdgDirectoryResolverTest, DirectoriesCanBeOverridenViaUserDirsFile) {
    XdgDirectoryResolver resolver(
      &dontGetEnvironmentVariable, &provideDummyXdgUserDirsFile
    );

    std::string dataHome = resolver.GetDataHomeDirectory();
    std::string stateHome = resolver.GetStateHomeDirectory();
    std::string cacheHome = resolver.GetCacheHomeDirectory();

    EXPECT_STREQ(dataHome.c_str(), u8"/all/my/data");
    EXPECT_STREQ(stateHome.c_str(), u8"/var/lib/my-service/state");
    EXPECT_STREQ(cacheHome.c_str(), u8"/var/cache/my-service");
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
