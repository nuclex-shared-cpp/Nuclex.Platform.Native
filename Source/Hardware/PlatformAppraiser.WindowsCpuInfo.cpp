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
    std::vector<Nuclex::Platform::Hardware::CpuInfo> topology;

    // Map for counting cores. First index is the core index, second index
    // stores the counter. For a CPU with HyperThreading, we expect to see each
    // core twice, thus each core's counter would be incremented to two.
    typedef std::unordered_map<std::size_t, std::size_t> CoreCountMap;

    // 
    struct TopologyIndexAndCoreCounts {
      public: std::size_t TopologyIndex;
      public: CoreCountMap CoreCounts;
    };
    typedef std::unordered_map<std::size_t, TopologyIndexAndCoreCounts> CpuMap;

    CpuMap cpus;

    // Go through the processors in all groups. We just treat the groups as a flat
    // list as we're only interested in physical sockets and cores, not how they
    // have been put into processor groups by Windows.
    std::size_t processorGroupCount = info.GroupsOfProcessors.size();
    for(std::size_t groupIndex = 0; groupIndex < processorGroupCount; ++groupIndex) {
      using Nuclex::Platform::Hardware::WindowsBasicCpuInfoReader;

      // Now iterate over all the processors in the processor group
      const std::vector<WindowsBasicCpuInfoReader::ProcessorInfo> &processors = (
        info.GroupsOfProcessors[groupIndex]
      );
      std::size_t processorCount = processors.size();
      for(std::size_t index = 0; index < processorCount; ++index) {

        // CoreIndex and PhysicalCpuIndex are intentionally one-based, so if it remained
        // zero on any processor, there is a gap in the processors reported by the Windows API.
        bool isValid = (
          (processors[index].CoreIndex != 0) &&
          (processors[index].PhysicalCpuIndex != 0)
        );
        if(isValid) {
          CpuMap::iterator cpuIterator = cpus.find(processors[index].PhysicalCpuIndex);
          if(cpuIterator == cpus.end()) { // CPU seen for the first time

            TopologyIndexAndCoreCounts cpu;
            cpu.TopologyIndex = topology.size();
            cpu.CoreCounts.insert(CoreCountMap::value_type(processors[index].CoreIndex, 1));
            cpus.insert(CpuMap::value_type(processors[index].PhysicalCpuIndex, cpu));

            Nuclex::Platform::Hardware::CpuInfo cpuInfo;
            cpuInfo.ThreadCount = 1;
            if(nameAndFrequencyPresent) {
              cpuInfo.ModelName = processors[index].Name;
            } else {
              cpuInfo.ModelName.assign(u8"CPU #   ", 8);
              cpuInfo.ModelName.resize(5);
              Nuclex::Support::Text::lexical_append(
                cpuInfo.ModelName, processors[index].PhysicalCpuIndex
              );
            }
            topology.push_back(cpuInfo);

          } else { // CPU was already present in our list

            TopologyIndexAndCoreCounts &cpu = cpuIterator->second;
            CoreCountMap::iterator coreIterator = (
              cpu.CoreCounts.find(processors[index].CoreIndex)
            );
            if(coreIterator == cpu.CoreCounts.end()) {
              cpu.CoreCounts.insert(
                CoreCountMap::value_type(processors[index].CoreIndex, 1)
              );
            } else {
              ++coreIterator->second;
            }

            ++topology[cpu.TopologyIndex].ThreadCount;

          } // if cpu found
        } // if currently enumerated processor valid
      } // for each reported processor
    } // for each reported processor group

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

    return topology;
  }

  // ------------------------------------------------------------------------------------------- //

  void createCpuInfoFromWmiCallback(
    void *cpuInfosAsVoid,
    std::size_t physicalCpuIndex,
    std::size_t coreCount,
    std::size_t threadCount,
    const std::string &name,
    double frequencyInMhz
  ) {
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
    std::shared_ptr<const Tasks::CancellationWatcher> canceller
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

    cpuInformationFromRegistrySeemsPlausible = false; // TESTING, REMOVE THIS!

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
    catch(const std::exception &e) {
      // If the WMI query failed for any reason, fall back to the basic cpu info
      // but ignore the enhanced info from the registry (those fields are in an
      // undefined state since the registry-source info enhancement failed)
      return topologyFromBasicCpuInfo(cpuInfoReader, false);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
