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

#if defined(NUCLEX_PLATFORM_LINUX)

#include <Nuclex/Support/BitTricks.h> // for BitTricks
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>
#include <Nuclex/Support/Threading/StopToken.h> // for StopToken

#include "./LinuxProcMemInfoReader.h" // for LinuxProcMemInfoReader
#include "./StringHelper.h" // for StringHelper

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  MemoryInfo PlatformAppraiser::analyzeMemoryAsync(
    std::shared_ptr<const Support::Threading::StopToken> canceller
  ) {

    // We may have been canceled before the thread got a chance to start,
    // so it makes sense to check once before actually doing anything.
    canceller->ThrowIfCanceled();

    // We can get all the information we need from the /proc/meminfo pseudofile.
    MemoryInfo result = LinuxProcMemInfoReader::TryReadMemInfo(canceller);

    canceller->ThrowIfCanceled();

    // The value we get usually is a little bit below the installed memory (maybe the kernel
    // subtracts off-limits memory or certain resources?). So we make the following assumption:
    // There are no more than 16 memory modules on one motherboard. That means we can guess
    // the size of the installed memory modules and thus, guess the actually installed memory.
    // If we parsed the SMBIOS/DMI data, we could know for sure, but I'd like to avoid that.
    {
      const std::size_t MaximumMemoryModuleCount = 16;

      std::size_t memoryModuleSize = result.InstalledMegabytes / MaximumMemoryModuleCount;
      if(memoryModuleSize == 0) {
        memoryModuleSize = 1; // No memory modules below 1 MiB exist. Done.
      } else {
        memoryModuleSize = Nuclex::Support::BitTricks::GetUpperPowerOfTwo(memoryModuleSize);
      }

      // Add half the module size for rounding (otherwise integer division always rounds down),
      // then shrink it down to the next lower multiple of the memory module size.
      std::size_t reportedMegabytes = result.InstalledMegabytes + (memoryModuleSize / 2);
      reportedMegabytes -= reportedMegabytes % memoryModuleSize;
      result.InstalledMegabytes = reportedMegabytes;
    }

    return result;

  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_LINUX)
