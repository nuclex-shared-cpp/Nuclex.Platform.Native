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

#include <cstdint> // for std::uint64_t
#include <vector> // for std::vector

#include "WindowsApi.h" // for WindowsAPI and error handling

#undef GetVolumeInformation

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

    /// <summary>
    ///   Starts a volume enumeration and provides the name of the first volume
    /// </summary>
    /// <param name="volumeName">Receives the name of the first volume</param>
    /// <returns>
    ///   The volume enumeration handle which needs to be closed again using
    ///   the <see cref="FindVolumeClose" /> method
    /// </returns>
    public: static ::HANDLE FindFirstVolume(std::wstring &volumeName);

    /// <summary>Advances to the next volume in an active volume enumeration</summary>
    /// <param name="findHandle">
    ///   Handle returned by <see cref="FindFirstVolume" /> that will be advanced
    ///   to the next storage volume on the system
    /// </param>
    /// <param name="volumeName">
    ///   Receives the name of the next storage volume unless the enumeration ended
    /// </param>
    /// <returns>
    ///   True if the next volume name was written into the <paramref name="volumeName" />
    ///   parameter, false if the enumeration had reached the last volume
    /// </returns>
    public: static bool FindNextVolume(::HANDLE findHandle, std::wstring &volumeName);

    /// <summary>Closes a storage volume numeration handle</summary>
    /// <param name="fileHandle">
    ///   Volume enumeration handle originally obtained via
    ///   the <see cref="FindFirstVolume" /> method.
    /// </param>
    /// <param name="throwOnError">
    ///   Whether to throw an exception if the handle could not be closed.
    ///   Can be set to false if this method is used in RAII-like scopes.
    /// </param>
    public: static void FindVolumeClose(::HANDLE findHandle, bool throwOnError = true);

    /// <summary>Retrieves the paths mapped to a volume</summary>
    /// <param name="volumeName">
    ///   Name of the volume, this must be a volume name in the form that is returend by
    ///   <see cref="FindFirstVolume" />.
    /// </param>
    /// <param name="pathNames">
    ///   Receives the path names that are mapped to the volume
    /// </param>
    /// <returns>
    ///   True if the volume name was valid and could be checked (the number of paths
    ///   returned may still be zero)
    /// </returns>
    public: static bool TryGetVolumePathNamesForVolumeName(
      const std::wstring &volumeName, std::vector<std::string> &pathNames
    );

    /// <summary>Retrieves informations about a storage volume</summary>
    /// <param name="volumeName">
    ///   Name of the volume for which informations will be queried
    /// </param>
    /// <param name="serialNumber">Receives the serial number of the volume</param>
    /// <param name="label">Label the user has given the volume</param>
    /// <param name="fileSystem">The name of the file system used on the volume</param>
    public: static void GetVolumeInformation(
      const std::wstring &volumeName,
      DWORD &serialNumber, std::string &label, std::string &fileSystem
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSSYSINFOAPI_H
