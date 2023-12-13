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

#ifndef NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICCPUINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICCPUINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string
#include <cstddef> // for std::size_t
#include <memory> // for std::shared_ptr
#include <vector> // for std::vector

#include "../Platform/WindowsSysInfoApi.h" // for WindowsSysInfoApi, WindowsApi

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class CancellationWatcher;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Queries physical and logical CPUs using the clasic Windows API</summary>
  class WindowsBasicCpuInfoReader {

    #pragma region struct ProcessorInfo

    /// <summary>Captures a summary of informations about one processor</summary>
    /// <remarks>
    ///   A processor is the term used in WBEM/WMI and the Windows API to refer to
    ///   one hardware-integrated code execution unit, i.e. one CPU core without
    ///   HyperThreading or one HyperThread in a CPU with HyperThreading.
    /// </remarks>
    public: struct ProcessorInfo {

      /// <summary>Initializes a new processor info structure</summary>
      public: ProcessorInfo() :
        PhysicalCpuIndex(0),
        CoreIndex(0) {} // other members can start out with undefined values

      /// <summary>Index of the physical CPU the processor belongs to</summary>
      /// <remarks>
      ///   This is 1-based. If any entries have CPU index 0, it means Window did report
      ///   only the processors as sharing a core, but not which physical CPU they belong to.
      /// <remarks>
      public: std::size_t PhysicalCpuIndex;
      /// <summary>Index of a core this processor shares with other processors</summary>
      /// <remarks>
      ///   This is 1-based. If any entries have core index 0, it means Windows did report
      ///   only the processors on a physical CPU, but not which core they are on.
      /// <remarks>
      public: std::size_t CoreIndex;
      /// <summary>Name of the CPU this processor is a part of</summary>
      public: std::string Name;
      /// <summary>Default clock frequency the processor runs at</summary>
      public: double FrequencyInMhz;
      /// <summary>Value provided by the Windows 7 API to identify Eco cores</summary>
      public: std::uint8_t Efficiency;

    };

    #pragma endregion // struct ProcessorInfo

    /// <summary>Initializes the collected informations in the CPU info reader</summary>
    public: WindowsBasicCpuInfoReader();

    /// <summary>Frees all data stored in the CPU info reader</summary>
    public: ~WindowsBasicCpuInfoReader() = default;

    /// <summary>Fetches the informations using API methods introduced by Windows XP</summary>
    /// <remarks>
    ///   This is the only method that works reliably on 32-bit Windows systems.
    /// </remarks>
    public: void FetchViaWindowsXpApi();

    /// <summary>Fetches the informations using API methods introduced by Windows 7</summary>
    /// <remarks>
    ///   On 64-bit Windows systems with more than 32 cores, this method will return garbage
    ///   because Microsoft messed up when designing the extended API (see the Remarks section
    ///   of <see cref="GetLogicalProcessorInformationEx" /> on MSDN).
    /// </remarks>
    public: void FetchViaWindowsSevenApi();

    /// <summary>
    ///   Ingests a logical processor information record generated by Windows'
    ///   <see cref="GetLogicalProcessorInformation" /> method
    /// </summary>
    /// <param name="logicalProcessor">Logical processor information to ingest</param>
    private: void ingestLogicalProcessor(
      const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION &logicalProcessor
    );

    /// <summary>
    ///   Ingests a logical processor information record generated by Windows'
    ///   <see cref="GetLogicalProcessorInformationEx" /> method
    /// </summary>
    /// <param name="logicalProcessor">Logical processor information to ingest</param>
    private: void ingestLogicalProcessor(
      const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX &logicalProcessor
    );

    /// <summary>Whether there are any cores using Hyper-Threading in the system</summary>
    public: bool UsesHyperThreading;
    /// <summary>True if an efficiency value other than zero was seen</summary>
    public: bool NonZeroEfficiencySpotted;
    /// <summary>Lowest efficiency value seen on any processor</summary>
    public: std::uint8_t LowestEfficiencySeen;
    /// <summary>Highest efficiency value seen on any processor</summary>
    public: std::uint8_t HighestEfficiencySeen;
    /// <summary>Number of physical CPUs reported</summary>
    public: std::size_t PhysicalCpuCount;
    /// <summary>Number of cores (shared by processors) present in the system</summary>
    public: std::size_t CoreCount;
    /// <summary>Number of threads (over all processors) the system runs simultaneously</summary>
    public: std::size_t ThreadCount;
    /// <summary>Processors reported by the Windows API</summary>
    /// <remarks>
    ///   Processors are split into groups (there could be one group per physical package
    ///   or one group per 64 cores since that's the limit of bits the mask can store),
    ///   all properties in the ProcessorInfo group are indices within its group.
    /// </remarks>
    public: std::vector<std::vector<ProcessorInfo>> GroupsOfProcessors;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICCPUINFOREADER_H