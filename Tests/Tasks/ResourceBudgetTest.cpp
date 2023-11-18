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

#include "../../Source/Tasks/ResourceBudget.h"
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

  TEST(ResourceBudgetTest, HasDefaultConstructor) {
    ResourceBudget budget;
    ASSERT_EQ(budget.QueryResourceMaximum(ResourceType::CpuCores), 0U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, ResourcesCanBeAdded) {
    ResourceBudget budget;
    ASSERT_EQ(budget.CountResourceUnits(ResourceType::CpuCores), 0U);
    budget.AddResource(ResourceType::CpuCores, 8U);
    ASSERT_EQ(budget.CountResourceUnits(ResourceType::CpuCores), 1U);
    budget.AddResource(ResourceType::CpuCores, 8U);
    ASSERT_EQ(budget.CountResourceUnits(ResourceType::CpuCores), 2U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, ResourceMaximumIsHighestIndividualUnit) {
    ResourceBudget budget;
    ASSERT_EQ(budget.QueryResourceMaximum(ResourceType::VideoMemory), 0U);
    budget.AddResource(ResourceType::VideoMemory, 6U * 1024U * 1024U);
    ASSERT_EQ(budget.QueryResourceMaximum(ResourceType::VideoMemory), 6U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);
    ASSERT_EQ(budget.QueryResourceMaximum(ResourceType::VideoMemory), 8U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 4U * 1024U * 1024U);
    ASSERT_EQ(budget.QueryResourceMaximum(ResourceType::VideoMemory), 8U * 1024U * 1024U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, PicksAdequateResourceUnits) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 8U);
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 4U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 24U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;
    assignedUnits.fill(std::size_t(-1));
    
    bool wasPicked = budget.Pick(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 16U * 1024U * 1024U,
        ResourceType::CpuCores, 8U
      )
    );

    EXPECT_TRUE(wasPicked);
    std::size_t cpuCoreUnitIndex = static_cast<std::size_t>(ResourceType::CpuCores);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 0U);
    std::size_t videoMemoryUnitIndex = static_cast<std::size_t>(ResourceType::VideoMemory);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 2U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, PickFailsOnInsufficientResources) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 4U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;
    assignedUnits.fill(std::size_t(-1));
    
    bool wasPicked = budget.Pick(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 10U * 1024U * 1024U,
        ResourceType::CpuCores, 2U
      )
    );

    EXPECT_FALSE(wasPicked);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, AllocatesAdequateResourceUnits) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 8U);
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 4U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 24U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;
    assignedUnits.fill(std::size_t(-1));
    
    bool wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 16U * 1024U * 1024U,
        ResourceType::CpuCores, 8U
      )
    );

    EXPECT_TRUE(wasAllocated);
    std::size_t cpuCoreUnitIndex = static_cast<std::size_t>(ResourceType::CpuCores);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 0U);
    std::size_t videoMemoryUnitIndex = static_cast<std::size_t>(ResourceType::VideoMemory);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 2U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, AllocateFailsOnInsufficientResources) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 4U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;
    assignedUnits.fill(std::size_t(-1));
    
    bool wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 10U * 1024U * 1024U,
        ResourceType::CpuCores, 2U
      )
    );

    EXPECT_FALSE(wasAllocated);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, AllocateBlocksAssignedResources) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;

    std::size_t cpuCoreUnitIndex = static_cast<std::size_t>(ResourceType::CpuCores);
    std::size_t videoMemoryUnitIndex = static_cast<std::size_t>(ResourceType::VideoMemory);

    // First allocation, should pick units 0, 0
    assignedUnits.fill(std::size_t(-1));
    bool wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 0U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 0U);

    // Second allocation, should pick units 1, 1
    assignedUnits.fill(std::size_t(-1)); // to request unit selection
    wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 1U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 1U);

    // Third allocation should fail because resources ran out
    assignedUnits.fill(std::size_t(-1)); // to request unit selection
    wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_FALSE(wasAllocated);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, AllocateHonorsExplicitUnitSelection) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;

    std::size_t cpuCoreUnitIndex = static_cast<std::size_t>(ResourceType::CpuCores);
    std::size_t videoMemoryUnitIndex = static_cast<std::size_t>(ResourceType::VideoMemory);

    // First allocation, should pick units 0, 0
    assignedUnits.fill(std::size_t(-1));
    assignedUnits.at(videoMemoryUnitIndex) = 1;
    bool wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 0U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 1U);

    // First allocation, should pick units 0, 0
    assignedUnits.fill(std::size_t(-1));
    assignedUnits.at(videoMemoryUnitIndex) = 1;
    wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_FALSE(wasAllocated);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, ReleaseFreesResources) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;

    std::size_t cpuCoreUnitIndex = static_cast<std::size_t>(ResourceType::CpuCores);
    std::size_t videoMemoryUnitIndex = static_cast<std::size_t>(ResourceType::VideoMemory);

    // First allocation, should pick units 0, 0
    assignedUnits.fill(std::size_t(-1));
    bool wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 0U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 0U);

    // Free the resources again
    budget.Release(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );

    // Second allocation, should pick units 0, 0 again
    assignedUnits.fill(std::size_t(-1));
    wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 0U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 0U);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, CanCheckIfAllocationIsPossible) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);

    std::shared_ptr<ResourceManifest> resources = ResourceManifest::Create(
      ResourceType::VideoMemory, 6U * 1024U * 1024U,
      ResourceType::CpuCores, 3U
    );

    // At the beginning, there should be enough resources left
    EXPECT_TRUE(budget.CanExecuteNow(resources));
    EXPECT_TRUE(budget.CanEverExecute(resources));

    // Allocate most of the available resources
    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;
    assignedUnits.fill(std::size_t(-1));
    bool wasAllocated = budget.Allocate(assignedUnits, resources);
    EXPECT_TRUE(wasAllocated);

    // Now the resource budget should report a lack of resources
    EXPECT_FALSE(budget.CanExecuteNow(resources));
    EXPECT_TRUE(budget.CanEverExecute(resources));

    // Now see if resouces that can never be available are detected
    bool canExecute = budget.CanEverExecute(
      ResourceManifest::Create(
        ResourceType::VideoMemory, 10U * 1024U * 1024U,
        ResourceType::CpuCores, 6U
      )
    );
    EXPECT_FALSE(canExecute);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ResourceBudgetTest, CanBeCopied) {
    ResourceBudget budget;
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::CpuCores, 4U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);
    budget.AddResource(ResourceType::VideoMemory, 8U * 1024U * 1024U);

    std::array<std::size_t, MaximumResourceType + 1> assignedUnits;

    std::size_t cpuCoreUnitIndex = static_cast<std::size_t>(ResourceType::CpuCores);
    std::size_t videoMemoryUnitIndex = static_cast<std::size_t>(ResourceType::VideoMemory);

    // First allocation, should pick units 0, 0
    assignedUnits.fill(std::size_t(-1));
    bool wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 0U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 0U);

    ResourceBudget copy = budget;

    // Second allocation, should pick units 1, 1
    assignedUnits.fill(std::size_t(-1)); // to request unit selection
    wasAllocated = budget.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 1U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 1U);

    // Second allocation on copy, should also pick units 1, 1
    assignedUnits.fill(std::size_t(-1)); // to request unit selection
    wasAllocated = copy.Allocate(
      assignedUnits,
      ResourceManifest::Create(
        ResourceType::VideoMemory, 6U * 1024U * 1024U,
        ResourceType::CpuCores, 3U
      )
    );
    EXPECT_TRUE(wasAllocated);
    EXPECT_EQ(assignedUnits.at(cpuCoreUnitIndex), 1U);
    EXPECT_EQ(assignedUnits.at(videoMemoryUnitIndex), 1U);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
