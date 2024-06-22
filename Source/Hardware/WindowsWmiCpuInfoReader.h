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

#ifndef NUCLEX_PLATFORM_HARDWARE_WINDOWSWMICPUINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_WINDOWSWMICPUINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string
#include <cstddef> // for std::size_t
#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Support { namespace Threading {

  // ------------------------------------------------------------------------------------------- //

  class StopToken;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Support::Threading

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
    ///   Stop token by which the query process can be aborted early
    /// </param>
    public: static void TryQueryCpuInfos(
      void *userPointer,
      CallbackFunction *callback,
      const std::shared_ptr<const Support::Threading::StopToken> &canceller
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_HARDWARE_WINDOWSWMICPUINFOREADER_H
