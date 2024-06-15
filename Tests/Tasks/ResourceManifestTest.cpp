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

#include "Nuclex/Platform/Tasks/ResourceManifest.h"

#include <Nuclex/Support/Threading/Thread.h>
#include <Nuclex/Support/Threading/Gate.h>

#include <gtest/gtest.h>

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceManifestTest, CanBeConstructedWithOneResource) {
    std::shared_ptr<ResourceManifest> manifest = ResourceManifest::Create(
      ResourceType::CpuCores, 4
    );

    ASSERT_EQ(manifest->Count, 1U);
    EXPECT_EQ(manifest->Resources[0].Type, ResourceType::CpuCores);
    EXPECT_EQ(manifest->Resources[0].Amount, 4U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceManifestTest, CanBeConstructedWithTwoResources) {
    std::shared_ptr<ResourceManifest> manifest = ResourceManifest::Create(
      ResourceType::SystemMemory, 512 * 1024 * 1024,
      ResourceType::WebRequests, 2
    );

    ASSERT_EQ(manifest->Count, 2U);
    EXPECT_EQ(manifest->Resources[0].Type, ResourceType::SystemMemory);
    EXPECT_EQ(manifest->Resources[0].Amount, 512U * 1024U * 1024U);
    EXPECT_EQ(manifest->Resources[1].Type, ResourceType::WebRequests);
    EXPECT_EQ(manifest->Resources[1].Amount, 2U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceManifestTest, CanBeConstructedWithThreeResources) {
    std::shared_ptr<ResourceManifest> manifest = ResourceManifest::Create(
      ResourceType::VideoMemory, 128 * 1024 * 1024,
      ResourceType::SystemMemory, 1024 * 1024 * 1024,
      ResourceType::CpuCores, 8
    );

    ASSERT_EQ(manifest->Count, 3U);
    EXPECT_EQ(manifest->Resources[0].Type, ResourceType::VideoMemory);
    EXPECT_EQ(manifest->Resources[0].Amount, 128U * 1024U * 1024U);
    EXPECT_EQ(manifest->Resources[1].Type, ResourceType::SystemMemory);
    EXPECT_EQ(manifest->Resources[1].Amount, 1024U * 1024U * 1024U);
    EXPECT_EQ(manifest->Resources[2].Type, ResourceType::CpuCores);
    EXPECT_EQ(manifest->Resources[2].Amount, 8U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceManifestTest, CanCombineResourcesWithoutOverlap) {
    std::shared_ptr<ResourceManifest> first = ResourceManifest::Create(
      ResourceType::VideoMemory, 128 * 1024 * 1024,
      ResourceType::CpuCores, 6
    );
    std::shared_ptr<ResourceManifest> second = ResourceManifest::Create(
      ResourceType::SystemMemory, 1536U * 1024 * 1024,
      ResourceType::WebRequests, 1
    );

    std::shared_ptr<ResourceManifest> combined = ResourceManifest::Combine(
      first, second
    );

    ASSERT_EQ(combined->Count, 4U);
    EXPECT_EQ(combined->Resources[0].Type, ResourceType::VideoMemory);
    EXPECT_EQ(combined->Resources[0].Amount, 128U * 1024U * 1024U);
    EXPECT_EQ(combined->Resources[1].Type, ResourceType::CpuCores);
    EXPECT_EQ(combined->Resources[1].Amount, 6U);
    EXPECT_EQ(combined->Resources[2].Type, ResourceType::SystemMemory);
    EXPECT_EQ(combined->Resources[2].Amount, 1536U * 1024U * 1024U);
    EXPECT_EQ(combined->Resources[3].Type, ResourceType::WebRequests);
    EXPECT_EQ(combined->Resources[3].Amount, 1U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceManifestTest, CanCombineResourcesWithPartialOverlap) {
    std::shared_ptr<ResourceManifest> first = ResourceManifest::Create(
      ResourceType::SystemMemory, 256 * 1024 * 1024,
      ResourceType::CpuCores, 6
    );
    std::shared_ptr<ResourceManifest> second = ResourceManifest::Create(
      ResourceType::VideoMemory, 32 * 1024 * 1024,
      ResourceType::CpuCores, 1
    );

    std::shared_ptr<ResourceManifest> combined = ResourceManifest::Combine(
      first, second
    );

    ASSERT_EQ(combined->Count, 3U);
    EXPECT_EQ(combined->Resources[0].Type, ResourceType::SystemMemory);
    EXPECT_EQ(combined->Resources[0].Amount, 256U * 1024U * 1024U);
    EXPECT_EQ(combined->Resources[1].Type, ResourceType::CpuCores);
    EXPECT_EQ(combined->Resources[1].Amount, 7U);
    EXPECT_EQ(combined->Resources[2].Type, ResourceType::VideoMemory);
    EXPECT_EQ(combined->Resources[2].Amount, 32U * 1024U * 1024U);
  }


  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceManifestTest, CanCombineResourcesWithFullOverlap) {
    std::shared_ptr<ResourceManifest> first = ResourceManifest::Create(
      ResourceType::SystemMemory, 256 * 1024 * 1024,
      ResourceType::CpuCores, 6
    );
    std::shared_ptr<ResourceManifest> second = ResourceManifest::Create(
      ResourceType::SystemMemory, 128 * 1024 * 1024,
      ResourceType::CpuCores, 2
    );

    std::shared_ptr<ResourceManifest> combined = ResourceManifest::Combine(
      first, second
    );

    ASSERT_EQ(combined->Count, 2U);
    EXPECT_EQ(combined->Resources[0].Type, ResourceType::SystemMemory);
    EXPECT_EQ(combined->Resources[0].Amount, 384U * 1024U * 1024U);
    EXPECT_EQ(combined->Resources[1].Type, ResourceType::CpuCores);
    EXPECT_EQ(combined->Resources[1].Amount, 8U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceManifestTest, HardDriveMaskIsInitializedToZero) {
    std::shared_ptr<ResourceManifest> manifest = ResourceManifest::Create(
      ResourceType::SystemMemory, 10 * 1024 * 1024
    );
    EXPECT_EQ(manifest->AccessedHardDriveMask, 0U);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
