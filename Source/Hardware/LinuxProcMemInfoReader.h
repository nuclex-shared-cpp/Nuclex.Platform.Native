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

#ifndef NUCLEX_PLATFORM_HARDWARE_LINUXPROCMEMINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_LINUXPROCMEMINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include "Nuclex/Platform/Hardware/MemoryInfo.h"

#include <memory>

namespace Nuclex { namespace Support { namespace Threading {

  // ------------------------------------------------------------------------------------------- //

  class StopToken;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Support::Threading

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Queries physical and logical CPUs using the /proc/cpuinfo pseudofile</summary>
  class LinuxProcMemInfoReader {

    /// <summary>Attempts to read informations about processors via /proc/cpuinfo</summary>
    /// <param name="processorCount">Number of processors to query</param>
    /// <param name="userPointer">Will be passed to the callback unmodified</param>
    /// <param name="callback">
    ///   Callback that will be invoked for each processor, provided with the
    ///   user pointer, processor index, processor name and base frequency in GHz
    /// </param>
    /// <param name="canceller">
    ///   Stop token by which the query process can be aborted early
    /// </param>
    public: static MemoryInfo TryReadMemInfo(
      const std::shared_ptr<const Support::Threading::StopToken> &canceller
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_LINUX)

#endif // NUCLEX_PLATFORM_HARDWARE_LINUXPROCMEMINFOREADER_H
