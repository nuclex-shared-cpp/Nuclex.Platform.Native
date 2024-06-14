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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSDEVICEAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSDEVICEAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h" // for WindowsAPI and error handling

#include <cstdint> // for std::uint64_t
#include <vector> // for std::vector

#include <winioctl.h> // for DeviceIoControl() and structures

#undef GetVolumeInformation

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps device IO functions from the Windows desktop API</summary>
  class WindowsDeviceApi {

    /// <summary>Returns the physical device number of device type for a volume</summary>
    /// <param name="volumeFileHandle">
    ///   File handle for a volume that has been opened via <see cref="CreateFile" />
    /// </param>
    /// <param name="storageDeviceNumber">
    ///   Structure into which the queried information is placed
    /// </param>
    public: static void DeviceIoControlStorageGetDeviceNumbers(
      ::HANDLE volumeFileHandle,
      ::STORAGE_DEVICE_NUMBER &storageDeviceNumber
    );

    /// <summary>Tries to query a storage device for its extents</summary>
    /// <param name="volumeFileHandle">
    ///   File handle for a volume that has been opened via <see cref="CreateFile" />
    /// </param>
    /// <param name="diskEXtents">
    ///   Vector into which the retrieved disk extents are placed
    /// </param>
    /// <returns>True if the information was obtained, false otherwise</returns>
    public: static bool TryDeviceIoControlVolumeGetVolumeDiskExtents(
      ::HANDLE volumeFileHandle,
      std::vector<::DISK_EXTENT> &diskExtents
    );

    /// <summary>Tries to query a storage device for TRIM support</summary>
    /// <param name="volumeFileHandle">
    ///   File handle for a volume that has been opened via <see cref="CreateFile" />
    /// </param>
    /// <param name="deviceTrimDescriptor">
    ///   Structure into which the queried information is placed
    /// </param>
    /// <returns>True if the information was obtained, false otherwise</returns>
    public: static bool TryDeviceIoControlStorageQueryTrimProperty(
      ::HANDLE volumeFileHandle,
      ::DEVICE_TRIM_DESCRIPTOR &deviceTrimDescriptor
    );

    /// <summary>Tries to query a storage device whether it has a seek penalty</summary>
    /// <param name="volumeFileHandle">
    ///   File handle for a volume that has been opened via <see cref="CreateFile" />
    /// </param>
    /// <param name="deviceSeekPenaltyDescriptor">
    ///   Structure into which the queried information is placed
    /// </param>
    /// <returns>True if the information was obtained, false otherwise</returns>
    public: static bool TryDeviceIoControlStorageQuerySeekPenaltyProperty(
      ::HANDLE volumeFileHandle,
      ::DEVICE_SEEK_PENALTY_DESCRIPTOR &deviceSeekPenaltyDescriptor
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSDEVICEAPI_H
