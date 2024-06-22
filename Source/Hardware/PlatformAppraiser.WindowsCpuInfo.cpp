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
#include <Nuclex/Support/Text/StringHelper.h> // for StringHelper
#include <Nuclex/Support/Threading/StopToken.h> // for StopToken

#include "./WindowsBasicCpuInfoReader.h"
#include "./WindowsRegistryCpuInfoReader.h"
#include "./WindowsWmiCpuInfoReader.h"

#include "./StringHelper.h" // for StringHelper

#include <vector> // for std::vector
#include <unordered_map> // for std::unordered_map

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

  /// <summary>
  ///   Callback for the registry-based CPU info reader that adds additional informations
  ///   to a processor already enumerated by the basic CPU info reader
  /// </summary>
  /// <param name="userPointer">
  ///   The basic CPU info reader storing the already collected CPU informations
  /// </param>
  /// <param name="processorIndex">Index of the processor information is provided for</param>
  /// <param name="name">Name of the processor as reported by the CPU itself</param>
  /// <param name="frequencyInMhz">The unboosted max frequency of the CPU in MHz</param>
  void enhanceProcessorInfoFromRegistry(
    void *userPointer,
    std::size_t processorIndex,
    const std::string &name,
    double frequencyInMhz
  ) {
    Nuclex::Platform::Hardware::WindowsBasicCpuInfoReader &reader = (
      *reinterpret_cast<Nuclex::Platform::Hardware::WindowsBasicCpuInfoReader *>(userPointer)
    );

    // We assume the processor index in the registry is linearly incrementing,
    // so systems with multiple processor groups or more than 64 processors will
    // simply have subkeys '65', '66' and so on rather than a different structure.
    std::size_t processorGroupCount = reader.GroupsOfProcessors.size();
    for(std::size_t index = 0; index < processorGroupCount; ++index) {
      std::size_t processorCountInGroup = reader.GroupsOfProcessors[index].size();
      if(processorIndex < processorCountInGroup) {
        reader.GroupsOfProcessors[index][processorIndex].Name = name;
        reader.GroupsOfProcessors[index][processorIndex].FrequencyInMhz = frequencyInMhz;
        break;
      } else {
        processorIndex -= processorCountInGroup;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Converts the data in the basic CPU info reader into the topology as it is returned
  ///   from the <see cref="PlatformAppraiser" />
  /// </summary>
  /// <param name="info">Basic CPU info reader holding the data</param>
  /// <param name="nameAndFrequencyPresent">
  ///   Whether we were able to enhance the information with CPU names and frequencies
  ///   (if the registry-based CPU info reader failed, these fields are undefined)
  /// </param>
  /// <returns>A list of CpuInfos, one for each physical CPU present</returns>
  std::vector<Nuclex::Platform::Hardware::CpuInfo> topologyFromBasicCpuInfo(
    Nuclex::Platform::Hardware::WindowsBasicCpuInfoReader &info,
    bool nameAndFrequencyPresent
  ) {

    // <summary>Maps a core ID reported via WinAPI to an index in our core list</summary>
    typedef std::unordered_map<std::size_t, std::size_t> CoreIndexMap;
    /// <summary>Stores the cores indices and index of a CPU in the topology list</summary>
    struct CpuInfoIndexAndCoreIndices {
      public: CpuInfoIndexAndCoreIndices(std::size_t cpuInfoIndex) :
        CpuInfoIndex(cpuInfoIndex) {}
      /// <summary>Index of the CPU in our final CpuInfo list</summary>
      public: std::size_t CpuInfoIndex;
      /// <summary>Maps core IDs from the Windows API to core indices</summary>
      public: CoreIndexMap CoreIndices;
    };
    /// <summary>Maps physical CPU IDs to indices in our CpuInfo list</summary>
    typedef std::unordered_map<std::size_t, CpuInfoIndexAndCoreIndices> CpuMap;

    std::vector<Nuclex::Platform::Hardware::CpuInfo> cpuInfos;
    CpuMap cpus;

    // Go through the processors in all groups. We just treat the groups as a flat
    // list as we're only interested in physical sockets and cores, not how they
    // have been put into processor groups by Windows.
    std::size_t processorGroupCount = info.GroupsOfProcessors.size();
    for(std::size_t groupIndex = 0; groupIndex < processorGroupCount; ++groupIndex) {
      using Nuclex::Platform::Hardware::WindowsBasicCpuInfoReader;
      using Nuclex::Platform::Hardware::CoreInfo;

      // Now iterate over all the processors in the processor group
      const std::vector<WindowsBasicCpuInfoReader::ProcessorInfo> &processors = (
        info.GroupsOfProcessors[groupIndex]
      );
      std::size_t processorCount = processors.size();
      for(std::size_t index = 0; index < processorCount; ++index) {

        // CoreIndex and PhysicalCpuIndex are intentionally one-based, so if either
        // is still zero, there is a gap in the processors reported by the Windows API.
        // Only here in the spirit of defensive programming and shouldn't ever occur.
        bool isValid = (
          (processors[index].PhysicalCpuIndex != 0) &&
          (processors[index].CoreIndex != 0)
        );
        if(isValid) {
          CpuMap::iterator cpuIterator = cpus.find(processors[index].PhysicalCpuIndex);

          // If this is the first time we're seeing this physical CPU, we need to
          // add it into the CpuInfo list we're generating.
          if(unlikely(cpuIterator == cpus.end())) {
            cpuIterator = cpus.emplace(
              processors[index].PhysicalCpuIndex,
              CpuInfoIndexAndCoreIndices(cpuInfos.size())
            ).first;
            
            Nuclex::Platform::Hardware::CpuInfo &newCpuInfo = cpuInfos.emplace_back();
            newCpuInfo.CoreCount = 0;
            if(nameAndFrequencyPresent) {
              newCpuInfo.ModelName = sanitizeCpuName(processors[index].Name);
            } else {
              newCpuInfo.ModelName.assign(u8"CPU #   ", 8);
              newCpuInfo.ModelName.resize(5);
              Nuclex::Support::Text::lexical_append(
                newCpuInfo.ModelName, processors[index].PhysicalCpuIndex
              );
            }
            newCpuInfo.ThreadCount = 0;
          }

          CpuInfoIndexAndCoreIndices &indices = cpuIterator->second;
          Nuclex::Platform::Hardware::CpuInfo &cpuInfo = cpuInfos[indices.CpuInfoIndex];

          // Each processor increments the thread count (that's what a processor is)
          ++cpuInfo.ThreadCount;

          // ...but we have to check if we visited this core already before we can
          // increment the core count of the CPU.
          std::pair<CoreIndexMap::iterator, bool> result = indices.CoreIndices.emplace(
            processors[index].CoreIndex, cpuInfo.Cores.size()
          );
          if(result.second) {
            ++cpuInfo.CoreCount;

            CoreInfo &newCoreInfo = cpuInfo.Cores.emplace_back();
            if(nameAndFrequencyPresent) {
              newCoreInfo.FrequencyInMHz = processors[index].FrequencyInMhz;
            } else {
              newCoreInfo.FrequencyInMHz = 0.0;
            }
            newCoreInfo.ThreadCount = 1;
          }

          // Update the core
          {
            CoreInfo &coreInfo = cpuInfo.Cores[result.first->second];
            ++coreInfo.ThreadCount;

            if(info.NonZeroEfficiencySpotted) {
              std::size_t distanceToHighestEfficiency = (
                info.HighestEfficiencySeen - processors[index].Efficiency
              );
              std::size_t distanceToLowestEfficiency = (
                processors[index].Efficiency - info.LowestEfficiencySeen
              );

              coreInfo.IsEcoCore = (distanceToHighestEfficiency >= distanceToLowestEfficiency);
              if(coreInfo.IsEcoCore) {
                if(cpuInfo.EcoCoreCount.has_value()) {
                  cpuInfo.EcoCoreCount = cpuInfo.EcoCoreCount.value() + 1;
                } else {
                  cpuInfo.EcoCoreCount = 1;
                }
              }
            } // if non-zero efficiency value (eco cores) spotted
          } // CPU core update beauty scope
        } // if processor package and core index valid

      } // for each processor in group
    } // for each group

    return cpuInfos;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Creates a new CPU info entry when the WMI reader callback is invoked</summary>
  /// <param name="cpuInfosAsVoid">Vector of CPU infos that will be filled</param>
  /// <param name="physicalCpuIndex">Index of the socket the CPU is installed in</param>
  /// <param name="coreCount">Number of cores the CPU has</param>
  /// <param name="threadCount">Number of threads the CPU has</param>
  /// <param name="name">Manufacturer and model name reported by WMI</param>
  /// <param name="frequencyInMhz">Unboosted maximum frequency of the CPU</param>
  void createCpuInfoFromWmiCallback(
    void *cpuInfosAsVoid,
    std::size_t physicalCpuIndex,
    std::size_t coreCount,
    std::size_t threadCount,
    const std::string &name,
    double frequencyInMhz
  ) {
    // We don't need this parameter. The WMI info reader does not invoke its callback
    // more than once per physical CPU and we're just putting them in a list one after another.
    (void)physicalCpuIndex;

    std::vector<Nuclex::Platform::Hardware::CpuInfo> &cpuInfos = (
      *reinterpret_cast<std::vector<Nuclex::Platform::Hardware::CpuInfo> *>(cpuInfosAsVoid)
    );

    // Add a new CPU info entry for this physical CPU we encountered
    cpuInfos.emplace_back();
    Nuclex::Platform::Hardware::CpuInfo &newCpuInfo = cpuInfos.back();

    // Basic information on the CPU level
    newCpuInfo.ModelName = name;
    newCpuInfo.CoreCount = coreCount;
    newCpuInfo.ThreadCount = threadCount;

    // Set up the cores. We assume that each has a minimum of 1 thread.
    if(threadCount < coreCount) {
      threadCount = coreCount;
    }
    newCpuInfo.Cores.resize(coreCount);
    for(std::size_t index = 0; index < coreCount; ++index) {
      newCpuInfo.Cores[index].FrequencyInMHz = frequencyInMhz;
      newCpuInfo.Cores[index].ThreadCount = 1;
      --threadCount;
    }

    // If there are more threads remaining, this may be a partial or full
    // HyperThreading CPU. Distribute these threads round-robin until we run
    // out. That's the best we can do with the information WMI provides us with.
    while(threadCount >= 1) {
      for(std::size_t index = 0; index < coreCount; ++index) {
        newCpuInfo.Cores[index].ThreadCount = 1;
        --threadCount;
        if(threadCount == 0) {
          break;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  std::vector<CpuInfo> PlatformAppraiser::analyzeCpuTopologyAsync(
    std::shared_ptr<const Nuclex::Support::Threading::StopToken> canceller
  ) {
    // We may have been canceled before the thread got a chance to start,
    // so it makes sense to check once before actually doing anything.
    canceller->ThrowIfCanceled();

    WindowsBasicCpuInfoReader cpuInfoReader;

    // Step 1: try to obtain the CPU info using the basic Windows API
    {
      // The newer API is broken for 32-bit systems, so we only use it when
      // we're running in 64-bit mode or higher.
      if constexpr(sizeof(std::size_t) >= 8) {
        cpuInfoReader.FetchViaWindowsSevenApi();
      } else {
        cpuInfoReader.FetchViaWindowsXpApi();
      }

      canceller->ThrowIfCanceled();
    }

    // Step 2: Enhance the information with data from the registry if possible
    bool cpuInformationFromRegistrySeemsPlausible;
    {
      std::size_t totalProcessorCount = 0;
      for(std::size_t index = 0; index < cpuInfoReader.GroupsOfProcessors.size(); ++index) {
        totalProcessorCount += cpuInfoReader.GroupsOfProcessors[index].size();
      }

      // Now try to add additional data into this structure by blindly assuming
      // that processors in the registry are listed in the same order as their index
      // in the processor masks provided by the Windows API. Also assume that if
      // multiple CPUs are involved, processors are sequentially numbered across
      // CPUs and the CPUs are in the order they are reported by the Windows API.
      //
      // Yes, a lot of assumptions, needed due to poor and thoughtless API design.
      cpuInformationFromRegistrySeemsPlausible = WindowsRegistryCpuInfoReader::TryReadCpuInfos(
        totalProcessorCount,
        &cpuInfoReader,
        &enhanceProcessorInfoFromRegistry,
        canceller
      );

      canceller->ThrowIfCanceled();
    }

    // If we got everything in this way and it seems okay, we package it up and return it.
    if(cpuInformationFromRegistrySeemsPlausible) {
      return topologyFromBasicCpuInfo(cpuInfoReader, true);
    }

    // Step 3: If the registry couldn't provide all the information, try WMI
    // (note that this is already the backup route and if it throws an exception,
    // the OS actually failed hard at issuing a WMI query - so treat our attempt
    // at returning incomplete information as a kind of error response :D)
    try {
      std::vector<CpuInfo> cpuInfos;

      WindowsWmiCpuInfoReader::TryQueryCpuInfos(
        &cpuInfos,
        &createCpuInfoFromWmiCallback,
        canceller
      );

      return cpuInfos;
    }
    catch(const std::exception &) {
      // If the WMI query failed for any reason, fall back to the basic cpu info
      // but ignore the enhanced info from the registry (those fields are in an
      // undefined state since the registry-source info enhancement failed)
      return topologyFromBasicCpuInfo(cpuInfoReader, false);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
