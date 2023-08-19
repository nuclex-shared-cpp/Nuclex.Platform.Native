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
      ResourceType::NetworkAdapter, 2
    );

    ASSERT_EQ(manifest->Count, 2U);
    EXPECT_EQ(manifest->Resources[0].Type, ResourceType::SystemMemory);
    EXPECT_EQ(manifest->Resources[0].Amount, 512U * 1024U * 1024U);
    EXPECT_EQ(manifest->Resources[1].Type, ResourceType::NetworkAdapter);
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
      ResourceType::NetworkAdapter, 1
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
    EXPECT_EQ(combined->Resources[3].Type, ResourceType::NetworkAdapter);
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
