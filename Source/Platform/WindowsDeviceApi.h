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
