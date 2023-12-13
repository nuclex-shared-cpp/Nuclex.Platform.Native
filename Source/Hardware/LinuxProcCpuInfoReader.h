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

#ifndef NUCLEX_PLATFORM_HARDWARE_LINUXPROCCPUINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_LINUXPROCCPUINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_LINUX)

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

  /// <summary>Queries physical and logical CPUs using the /proc/cpuinfo pseudofile</summary>
  class LinuxProcCpuInfoReader {

    /// <summary>
    ///   Signature for the callback function invoked by <see cref="TryQueryCpuInfos" />
    /// </summary>
    public: typedef void CallbackFunction(
      void *userPointer,
      std::size_t processorIndex,
      std::size_t physicalCpuId,
      std::size_t coreId,
      const std::string &name,
      double frequencyInMhz,
      double bogoMips
    );

    /// <summary>Attempts to read informations about processors via /proc/cpuinfo</summary>
    /// <param name="processorCount">Number of processors to query</param>
    /// <param name="userPointer">Will be passed to the callback unmodified</param>
    /// <param name="callback">
    ///   Callback that will be invoked for each processor, provided with the
    ///   user pointer, processor index, processor name and base frequency in GHz
    /// </param>
    /// <param name="canceller">
    ///   Cancellation watcher by which the query process can be aborted early
    /// </param>
    public: static void TryReadCpuInfos(
      void *userPointer,
      CallbackFunction *callback,
      const std::shared_ptr<const Tasks::CancellationWatcher> &canceller
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_LINUX)

#endif // NUCLEX_PLATFORM_HARDWARE_LINUXPROCCPUINFOREADER_H
