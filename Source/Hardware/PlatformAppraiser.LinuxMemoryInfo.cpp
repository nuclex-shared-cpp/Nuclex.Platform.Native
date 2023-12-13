#pragma region CPL License
/*
Nuclex Native Framework
Copyright (C) 2002-2021 Nuclex Development Labs

This library is free software; you can redistribute it and/or
modify it under the terms of the IBM Common Public License as
published by the IBM Corporation; either version 1.0 of the
License, or (at your option) any later version.q

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

#if defined(NUCLEX_PLATFORM_LINUX)

#include <Nuclex/Support/BitTricks.h> // for BitTricks
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

#include "./LinuxProcMemInfoReader.h" // for LinuxProcMemInfoReader
#include "./StringHelper.h" // for StringHelper

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  MemoryInfo PlatformAppraiser::analyzeMemoryAsync(
    std::shared_ptr<const Tasks::CancellationWatcher> canceller
  ) {

    // We may have been canceled before the thread got a chance to start,
    // so it makes sense to check once before actually doing anything.
    canceller->ThrowIfCanceled();

    MemoryInfo result = LinuxProcMemInfoReader::TryReadMemInfo(canceller);

    // The value we get usually is a little bit below the installed memory (maybe the kernel
    // subtracts off-limits memory or certain resources?). So we make the following assumption:
    // There are no more than 16 memory modules on a motherboard. That means we can guess
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

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
