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

#include "Nuclex/Platform/Hardware/PlatformAppraiser.h"

#include <Nuclex/Support/Threading/StopToken.h>

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  std::future<std::vector<CpuInfo>> PlatformAppraiser::AnalyzeCpuTopology(
    const std::shared_ptr<const Support::Threading::StopToken> &canceller /* = (
      std::shared_ptr<const Tasks::StopToken>()
    ) */
  ) {
    return std::async(
      std::launch::async,
      &PlatformAppraiser::analyzeCpuTopologyAsync, canceller
    );
  }

  // ------------------------------------------------------------------------------------------- //

  std::future<MemoryInfo> PlatformAppraiser::AnalyzeMemory(
    const std::shared_ptr<const Support::Threading::StopToken> &canceller /* = (
      std::shared_ptr<const Tasks::StopToken>()
    ) */
  ) {
    return std::async(
      std::launch::async,
      &PlatformAppraiser::analyzeMemoryAsync, canceller
    );
  }

  // ------------------------------------------------------------------------------------------- //

  std::future<std::vector<StoreInfo>> PlatformAppraiser::AnalyzeStorageVolumes(
    const std::shared_ptr<const Support::Threading::StopToken> &canceller /* = (
      std::shared_ptr<const Tasks::StopToken>()
    ) */
  ) {
    return std::async(
      std::launch::async,
      &PlatformAppraiser::analyzeStorageVolumesAsync, canceller
    );
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware