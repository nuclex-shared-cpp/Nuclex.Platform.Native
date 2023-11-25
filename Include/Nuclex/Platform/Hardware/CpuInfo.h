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

#ifndef NUCLEX_PLATFORM_HARDWARE_CPUINFO_H
#define NUCLEX_PLATFORM_HARDWARE_CPUINFO_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std::vector
#include <optional> // for std::optional

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  class CoreInfo; // declared further down in this file

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a physical CPU installed in the system</summary>
  class NUCLEX_PLATFORM_TYPE CpuInfo {

    /// <summary>Manufacturer and model name of the CPU, if available</summary>
    /// <remarks>
    ///   This is only for display and plausibility checking to spot when the hardware
    ///   querying code reports numbers that do not match the CPU's specifications.
    ///   The model name will be something like &quot;Intel Xeon E5-2680&quot;,
    ///   &quot;Intel Core i5-4300U&&quot; or &quot;AMD Ryzen X5900&quot; if it can be
    ///   determined. On Windows systems it can also state &quote;<unknown>&quote; because
    ///   Windows hardware APIs are all undocumented, broken or simply too slow to query.
    /// </remarks>
    public: std::string ModelName;

    /// <summary>Number of cores on the CPU</summary>
    /// <remarks>
    ///   On plain CPUs, one core equals one unit capable of independent, parallel execution
    ///   of code. In this case, the number of cores and processors reported will be the same.
    ///   If HyperThreading or SMT is supported and enabled, each core may be split into
    ///   multiple processors, so the number of processors may, for example, be double
    ///   the number of cores.
    /// </remarks>
    public: std::size_t CoreCount;

    /// <summary>Number of 'eco' cores on the CPU</summary>
    /// <remarks>
    ///   Some newer CPUs have a mix of performance cores ("p-cores") which provide excellent
    ///   per-thread performance and eco cores ("e-cores") which are slower but more energy
    ///   efficient. If this value is present, the current system uses such a CPU and we were
    ///   able to detect it as such. If empty, the core types could not be determined.
    /// </remarks>
    public: std::optional<std::size_t> EcoCoreCount;

    /// <summary>Number of independent units able to execute code in this CPU</summary>
    /// <remarks>
    ///   This indicates the number of threads that can run independently and in parallel on
    ///   hardware on this CPU. It can be used to tune a thread pool or to control the number
    ///   of tasks that will be launched in parallel.
    /// </remarks>
    public: std::size_t ProcessorCount; // aka thread count

    /// <summary>Detailed information about the CPU's cores</summary>
    public: std::vector<CoreInfo> Cores;

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a physical CPU installed in the system</summary>
  class NUCLEX_PLATFORM_TYPE CoreInfo {

    /// <summary>Frequency of the CPU in Megahertz</summary>
    /// <remarks>
    ///   This may or may not include opportunistic overclocking (but if the hardware
    ///   detection has a choice, it will be without). It is for display and possibly
    ///   performance-prioritized scheduling (if a CPU has performance + eco cores).
    /// </remarks>
    public: std::size_t FrequencyInMHz;

    /// <summary>Estimated average instructions per second this core can execute</summary>
    /// <remarks>
    ///   This value is not an accurate benchmark in any way, it just provides a vague idea of
    ///   what performance can be expected from this CPU. You can use it to warn about seriously
    ///   underpowered systems, for complex multi-system/-socket task balancing and perhaps to
    ///   estimate how much there is to gain from running on a P-core versus an E-core.
    /// </remarks>
    public: std::optional<std::size_t> BogoMips;

    /// <summary>Whether this core is a slower but power-efficient eco core</summary>
    /// <remarks>
    ///   Modern CPUs can contain different types of cores, usually split between
    ///   &quot;performance cores&quot; and &quot;eco cores&quot;. P-cores are intended to
    ///   offer good single-thread performance for old and/or low-threaded applications
    ///   while the additional E-cores can be used for non-time-critical tasks or recruited
    ///   by high-threaded appications to achieve maximum throughput.
    /// </remarks>
    public: std::optional<bool> IsEcoCore;

    /// <summary>Number of processors provided by this core</summary>
    /// <remarks>
    ///   With HyperThreading and similar techniques, CPU cores are treated as two or more
    ///   processors (aka hardware threads). While such processors share some or all of
    ///   the underlying circuits, the hardware itself can be able to run some instructions
    ///   in parallel or seamlessly continue running the other processor's instructions when
    ///   on of them is waiting (i.e. for a memory access, thread signal or such).
    /// </remarks>
    public: std::size_t ProcessorCount;

  };

  // ------------------------------------------------------------------------------------------- //

  // QueryTemperature() as separate method
  // QueryCurrentFrequency() as separate method

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_CPUINFO_H
