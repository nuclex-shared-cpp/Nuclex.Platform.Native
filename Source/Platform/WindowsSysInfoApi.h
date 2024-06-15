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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSSYSINFOAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSSYSINFOAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h" // for WindowsAPI and error handling

#include <cstdint> // for std::uint64_t
#include <vector> // for std::vector

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps system information functions from the Windows desktop API</summary>
  class WindowsSysInfoApi {

    /// <summary>Determines the amount of physical memory in the system</summary>
    /// <returns>The amount of physical memory installed in kilobytes</returns>
    public: static std::uint64_t GetPhysicallyInstalledSystemMemory();

    /// <summary>Queries the amount of accessible and free memory</summary>
    /// <param name="memoryStatus">Receives the memory status</param>
    public: static void GetGlobalMemoryStatus(::MEMORYSTATUSEX &memoryStatus);

    /// <summary>Obtains information about the system's online processors</summary>
    /// <returns>
    ///   A memory buffer into which a number of
    ///   <see cref="SYSTEM_LOGICAL_PROCESSOR_INFORMATION" /> structures are stored
    ///   (so it should be a multiple of that size, but the API works with bytes)
    /// </returns>
    public: static std::vector<std::uint8_t> GetLogicalProcessorInformation();

    /// <summary>Obtains better information about the system's online processors</summary>
    /// <returns>
    ///   A memory buffer into which a number of
    ///   <see cref="SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX" /> structures are stored
    ///   (so it should be a multiple of that size, but the API works with bytes)
    /// </returns>
    public: static std::vector<std::uint8_t> GetLogicalProcessorInformationEx();

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSSYSINFOAPI_H
