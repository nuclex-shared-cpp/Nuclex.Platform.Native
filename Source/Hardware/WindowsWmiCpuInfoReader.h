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

#ifndef NUCLEX_PLATFORM_HARDWARE_WINDOWSWMICPUINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_WINDOWSWMICPUINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string
#include <cstddef> // for std::size_t
#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class CancellationWatcher;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Reads processor names and base frequencies via WMI</summary>
  class WindowsWmiCpuInfoReader {

    /// <summary>
    ///   Signature for the callback function invoked by <see cref="TryQueryCpuInfos" />
    /// </summary>
    public: typedef void CallbackFunction(
      void *userPointer,
      std::size_t physicalCpuIndex,
      std::size_t coreCount,
      std::size_t threadCount,
      const std::string &name,
      double frequencyInMhz
    );

    /// <summary>Attempts to query informations about processors via WMI</summary>
    /// <param name="processorCount">Number of processors to query</param>
    /// <param name="userPointer">Will be passed to the callback unmodified</param>
    /// <param name="callback">
    ///   Callback that will be invoked for each processor, provided with the
    ///   user pointer, processor index, processor name and base frequency in GHz
    /// </param>
    /// <param name="canceller">
    ///   Cancellation watcher by which the query process can be aborted early
    /// </param>
    public: static void TryQueryCpuInfos(
      void *userPointer,
      CallbackFunction *callback,
      const std::shared_ptr<const Tasks::CancellationWatcher> &canceller
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_HARDWARE_WINDOWSWMICPUINFOREADER_H
