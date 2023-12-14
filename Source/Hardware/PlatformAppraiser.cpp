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

#include "Nuclex/Platform/Hardware/PlatformAppraiser.h"
#include "Nuclex/Platform/Tasks/CancellationWatcher.h"

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  std::future<std::vector<CpuInfo>> PlatformAppraiser::AnalyzeCpuTopology(
    const std::shared_ptr<const Tasks::CancellationWatcher> &canceller /* = (
      std::shared_ptr<const Tasks::CancellationWatcher>()
    ) */
  ) {
    return std::async(
      std::launch::async,
      &PlatformAppraiser::analyzeCpuTopologyAsync, canceller
    );
  }

  // ------------------------------------------------------------------------------------------- //

  std::future<MemoryInfo> PlatformAppraiser::AnalyzeMemory(
    const std::shared_ptr<const Tasks::CancellationWatcher> &canceller /* = (
      std::shared_ptr<const Tasks::CancellationWatcher>()
    ) */
  ) {
    return std::async(
      std::launch::async,
      &PlatformAppraiser::analyzeMemoryAsync, canceller
    );
  }

  // ------------------------------------------------------------------------------------------- //

  std::future<std::vector<VolumeInfo>> PlatformAppraiser::AnalyzeStorageVolumes(
    const std::shared_ptr<const Tasks::CancellationWatcher> &canceller /* = (
      std::shared_ptr<const Tasks::CancellationWatcher>()
    ) */
  ) {
    return std::async(
      std::launch::async,
      &PlatformAppraiser::analyzeStorageVolumesAsync, canceller
    );
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware