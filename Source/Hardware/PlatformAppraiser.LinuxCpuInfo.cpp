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

#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>
#include <Nuclex/Support/Text/StringHelper.h> // for StringHelper
#include <Nuclex/Support/Threading/StopToken.h> // for StopToken

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
    using Nuclex::Support::Text::StringHelper;

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

    StringHelper::CollapseDuplicateWhitespace(sanitized);

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
    std::shared_ptr<const Support::Threading::StopToken> canceller
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
