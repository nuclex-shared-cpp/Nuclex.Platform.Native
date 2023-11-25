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

#include "./WindowsBasicCpuInfoReader.h"
#include "./WindowsRegistryCpuInfoReader.h"
#include "./WindowsWmiCpuInfoReader.h"

#include "./StringHelper.h" // for StringHelper
#include "../Platform/WindowsRegistryApi.h" // for WindowsRegistryApi, WindowsApi
#include "../Platform/WindowsSysInfoApi.h" // for WindowsSysInfoApi

#include <vector> // for std::vector
#include <limits> // for std::numeric_limits
#include <unordered_set> // for std::unordered_set
#include <unordered_map> // for std::unordered_map
#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

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

  std::vector<Nuclex::Platform::Hardware::CpuInfo> topologyFromBasicCpuInfo(
    Nuclex::Platform::Hardware::WindowsBasicCpuInfoReader &info,
    bool nameAndFrequencyPresent
  ) {
    std::vector<Nuclex::Platform::Hardware::CpuInfo> topology;

    typedef std::unordered_map<std::size_t, std::size_t> CoreCountMap;
    struct TopologyIndexAndCoreCounts {
      public: std::size_t TopologyIndex;
      public: CoreCountMap CoreCounts;
    };
    typedef std::unordered_map<std::size_t, TopologyIndexAndCoreCounts> CpuMap;

    CpuMap cpus;
#if 0
    std::size_t processorGroupCount = info.GroupsOfProcessors.size();
    for(std::size_t index = 0; index < processorGroupCount; ++index) {

    // Figure out how many individual cores each CPU has and pre-fill the topology list
    std::size_t processorCount = info.Processors.size();
    for(std::size_t index = 0; index < processorCount; ++index) {

      // CoreIndex and PhysicalCpuIndex are intentionally one-based, so if it remained
      // zero on any processor, there is a gap in the processors reported by the Windows API.
      bool isValid = (
        (info.Processors[index].CoreIndex != 0) &&
        (info.Processors[index].PhysicalCpuIndex != 0)
      );
      if(isValid) {
        CpuMap::iterator cpuIterator = cpus.find(info.Processors[index].PhysicalCpuIndex);
        if(cpuIterator == cpus.end()) { // CPU seen for the first time

          TopologyIndexAndCoreCounts cpu;
          cpu.TopologyIndex = topology.size();
          cpu.CoreCounts.insert(CoreCountMap::value_type(info.Processors[index].CoreIndex, 1));
          cpus.insert(CpuMap::value_type(info.Processors[index].PhysicalCpuIndex, cpu));

          Nuclex::Platform::Hardware::CpuInfo cpuInfo;
          cpuInfo.ProcessorCount = 1;
          if(nameAndFrequencyPresent) {
            cpuInfo.ModelName = info.Processors[index].Name;
          } else {
            cpuInfo.ModelName.assign(u8"CPU #   ", 8);
            cpuInfo.ModelName.resize(5);
            Nuclex::Support::Text::lexical_append(
              cpuInfo.ModelName, info.Processors[index].PhysicalCpuIndex
            );
          }
          topology.push_back(cpuInfo);

        } else { // CPU was already present in our list

          TopologyIndexAndCoreCounts &cpu = cpuIterator->second;
          CoreCountMap::iterator coreIterator = (
            cpu.CoreCounts.find(info.Processors[index].CoreIndex)
          );
          if(coreIterator == cpu.CoreCounts.end()) {
            cpu.CoreCounts.insert(
              CoreCountMap::value_type(info.Processors[index].CoreIndex, 1)
            );
          } else {
            ++coreIterator->second;
          }

          ++topology[cpu.TopologyIndex].ProcessorCount;

        } // if cpu found
      } // if currently enumerated processor valid
    } // for each reported processor

    // Now just fill out the number of unique cores we left empty above.
    // Normally this is either the same as the number of processors we've processed or
    // half that if each core has two processors due to HyperThreading.
    for(
      CpuMap::const_iterator cpuIterator = cpus.begin();
      cpuIterator != cpus.end();
      ++cpuIterator
    ) {
      topology[cpuIterator->second.TopologyIndex].CoreCount = (
        cpuIterator->second.CoreCounts.size()
      );
    }
#endif

    return topology;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  std::vector<CpuInfo> PlatformAppraiser::analyzeCpuTopologyAsync(
    std::shared_ptr<const Tasks::CancellationWatcher> canceller
  ) {
    std::vector<CpuInfo> result;

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
    {
      //WindowsWmiCpuInfoReader::TryQueryCpuInfos
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
