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
