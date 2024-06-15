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

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/BitTricks.h> // for BitTricks
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

#include "../Platform/WindowsSysInfoApi.h"

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  MemoryInfo PlatformAppraiser::analyzeMemoryAsync(
    std::shared_ptr<const Tasks::CancellationWatcher> canceller
  ) {
    MemoryInfo result;

    // We may have been canceled before the thread got a chance to start,
    // so it makes sense to check once before actually doing anything.
    canceller->ThrowIfCanceled();

    // This should report the memory amount socketed into the motherboard. It's mostly
    // useful for reporting and logging, but may also give us an idea how far the Windows
    // system can be pressured for memory.
    result.InstalledMegabytes = (
      Platform::WindowsSysInfoApi::GetPhysicallyInstalledSystemMemory() / 1024
    );

    canceller->ThrowIfCanceled();

    ::MEMORYSTATUSEX memoryStatus;
    Platform::WindowsSysInfoApi::GetGlobalMemoryStatus(memoryStatus);
    
    // Windows in 64-bit mode will report garbage in the area of petabytes here,
    // regardless of installed ram and potentially available swap space, so we clip
    // this value since we merely want to know how tight we are if we run as
    // a 32-bit process (2 gigabytes normal, 3 gigabytes with extended memory patch)
    result.MaximumProgramMegabytes = (
      std::min(
        static_cast<std::uint64_t>(memoryStatus.ullTotalVirtual) / 1024,
        result.InstalledMegabytes
      )
    );

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
