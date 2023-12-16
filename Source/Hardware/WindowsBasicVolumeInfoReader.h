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

#ifndef NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICVOLUMEINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICVOLUMEINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "Nuclex/Platform/Hardware/StoreInfo.h"

#include <string> // for std::string
#include <cstddef> // for std::size_t
#include <vector> // for std::vector
#include <map> // for std::map

#include "../Platform/WindowsDeviceApi.h" // for DEVICE_TYPE, WindowsApi

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class CancellationWatcher;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Queries the Windows API to determine drives and partitions</summary>
  class WindowsBasicVolumeInfoReader {

    public: WindowsBasicVolumeInfoReader() {}

    public: ~WindowsBasicVolumeInfoReader() {}

    /// <summary>Enumerates the volumes present in the system using the Windows API</summary>
    /// <remarks>
    ///   This should be fast and work on any Windows system. It uses DeviceIoControl() to
    ///   collect some of the vital informations and the permissions required are poorly
    ///   documented, so we work assuming minimal permissions and hope for the best.
    /// </remarks>
    public: void EnumerateWindowsVolumes();

    //private: void extractInformationFromVolume(const std::wstring &volumeName);

    /// <summary>
    ///   Called by the <see cref="EnumerateWindowsVolumes" /> method to integrate a discovered
    ///   partition into the list of stores and partitions.
    /// </summary>
    /// <param name="deviceNumber">
    ///   The physical device number. Each hardware storage unit (HDD/SSD/memory card)
    ///   is given one and it is never reused until the next reboot.
    /// </param>
    /// <param name="deviceType">Type of the storage unit</param>
    /// <param name="isSolidStateDrive">Whether the drive is an SSD, if known</param>
    /// <param name="capacityInMegabytes">Capacity of the partition, if known</param>
    /// <param name="volumeName">Name of the volume as used within the Windows API</param>
    /// <param name="serialNumber">Serial number reported by the Windows API</param>
    /// <param name="label">The user-given label of the volume</param>
    /// <param name="fileSystem">
    ///   Name of the file system the volume has been formatted with
    /// </param>
    /// <param name="mappedPaths">Root paths under which this volume can be accessed</param>
    private: void addVolumeToNewOrExistingStore(
      DWORD deviceNumber,
      DEVICE_TYPE deviceType,
      std::optional<bool> isSolidStateDrive,
      std::optional<std::size_t> capacityInMegabytes,
      const std::string &volumeName,
      DWORD serialNumber,
      const std::string &label,
      const std::string &fileSystem,
      const std::vector<std::string> &mappedPaths
    );

    /// <summary>Store descriptions the basic info reader is generating</summary>
    private: std::vector<StoreInfo> stores;

    /// <summary>Map from physical device numbers to store indices</summary>
    private: typedef std::map<DWORD, std::size_t> DeviceNumberToStoreIndexMap;

    /// <summary>Maps device numbers provided by Windows </summary>
    private: DeviceNumberToStoreIndexMap deviceNumberToStoreIndex;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICVOLUMEINFOREADER_H
