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

#if defined(NUCLEX_PLATFORM_LINUX)

#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

#include "./LinuxProcCpuInfoReader.h" // for LinuxProcCpuInfoReader
#include "./StringHelper.h" // for StringHelper

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Trys to parse the CPU frequency from the CPU name string or takes the reported
  ///   frequency and convert it into GHz (guessing the unit if none is provided)
  /// </summary>
  /// <param name="cpuName">CPU name string reported by the CPU itself</param>
  /// <param name="maxMhzSeen">Maximum MHz seen for the CPU on any core</param>
  /// <returns>The CPU frequency in GHz</returns>
  double sanitizeCpuFrequency(
    const std::string_view &cpuName, double maxMhzSeen
  ) {
    using Nuclex::Support::Text::lexical_cast;

    std::string::size_type atIndex = cpuName.find(u8'@');
    if(atIndex != std::string::npos) {
      std::string frequencyPart(
        Nuclex::Platform::Hardware::StringHelper::FindNextFloat(cpuName, atIndex + 1)
      );
      if(!frequencyPart.empty()) {
        double frequency = Nuclex::Support::Text::lexical_cast<double>(frequencyPart);
        if(cpuName.find(u8"GHz", atIndex) != std::string::npos) {
          return frequency;
        } else if(cpuName.find(u8"MHz", atIndex) != std::string::npos) {
          return frequency / 1000.0;
        } else if(cpuName.find(u8"KHz", atIndex) != std::string::npos) {
          return frequency / 1000000.0; // lol :)
        } else {
          return frequency / 1000000000.0; // extra lol :)
        }
      }
    }

    // Fallback if we cannot parse the frequency from the CPU make and model
    std::uint32_t tenthGhz = static_cast<std::uint32_t>(maxMhzSeen / 100.0 + 0.5);
    return static_cast<double>(tenthGhz) / 10.0;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Removes boilerplate contents from a CPUs make and model string</summary>
  /// <param name="cpuName">CPU make and model string that will be sanitized</param>
  /// <returns>The CPU name minus any trademark signs and filler words</returns>
  std::string sanitizeCpuName(const std::string_view &cpuName) {
    using Nuclex::Platform::Hardware::StringHelper;

    std::string sanitized(cpuName);
    StringHelper::EraseSubstrings(sanitized, u8"(R)"); // Registered
    StringHelper::EraseSubstrings(sanitized, u8"(TM)"); // Trademark
    StringHelper::EraseSubstrings(sanitized, u8"CPU"); // Filler word
    StringHelper::EraseSubstrings(sanitized, u8" 0 "); // Meaningless
                                                                               // Frequency delimiter
    std::string::size_type atIndex = sanitized.find(u8'@');
    if(atIndex != std::string::npos) {
      sanitized.erase(atIndex);
    }

    StringHelper::EraseDuplicateWhitespace(sanitized);

    return sanitized;
  }

  // ------------------------------------------------------------------------------------------- //

  void addProcessorFromProcCpuInfo(
    void *userPointer,
    std::size_t processorIndex,
    std::size_t physicalCpuId,
    std::size_t coreId,
    const std::string &name,
    double frequencyInMhz,
    double bogoMips
  ) {
    std::string saneName = sanitizeCpuName(name);
    double saneFrequency = sanitizeCpuFrequency(name, frequencyInMhz);

  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  std::vector<CpuInfo> PlatformAppraiser::analyzeCpuTopologyAsync(
    std::shared_ptr<const Tasks::CancellationWatcher> canceller
  ) {
    // We may have been canceled before the thread got a chance to start,
    // so it makes sense to check once before actually doing anything.
    canceller->ThrowIfCanceled();

    {
      LinuxProcCpuInfoReader::TryReadCpuInfos(
        nullptr, &addProcessorFromProcCpuInfo, canceller
      );
    }

    return std::vector<CpuInfo>();
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_LINUX)
