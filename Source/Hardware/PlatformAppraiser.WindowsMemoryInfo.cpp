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

    // This should report the memory amount socketed into the motherboard. It's mostly
    // useful for reporting and logging, but may also give us an idea how far the Windows
    // system can be pressured for memory.
    result.InstalledMegabytes = (
      Platform::WindowsSysInfoApi::GetPhysicallyInstalledSystemMemory() / 1024
    );

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
