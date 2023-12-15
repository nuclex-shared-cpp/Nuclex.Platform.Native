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

#ifndef NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICSTOREINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICSTOREINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "Nuclex/Platform/Hardware/StoreInfo.h"

#include <string> // for std::string
#include <cstddef> // for std::size_t
#include <vector> // for std::vector
#include <map> // for std::map

#include "../Platform/WindowsSysInfoApi.h" // for WindowsSysInfoApi, WindowsApi

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class CancellationWatcher;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Queries the Windows API to determine drives and partitions</summary>
  class WindowsBasicStoreInfoReader {

    public: WindowsBasicStoreInfoReader() {}

    public: ~WindowsBasicStoreInfoReader() {}

    public: void EnumerateVolumes();

    private: void addVolumeToNewOrExistingStore(
      DWORD deviceNumber,
      DEVICE_TYPE deviceType,
      const std::string &volumeName,
      DWORD serialNumber,
      const std::string &label,
      const std::string &fileSystem,
      const std::vector<std::string> &mappedPaths
    );

    /// <summary>Store descriptions the basic info reader is generating</summary>
    private: std::vector<VolumeInfo> stores;

    /// <summary>Map from physical device numbers to store indices</summary>
    private: typedef std::map<DWORD, std::size_t> DeviceNumberToStoreIndexMap;

    /// <summary>Maps device numbers provided by Windows </summary>
    private: DeviceNumberToStoreIndexMap deviceNumberToStoreIndex;

    /*
    /// <summary>
    /// 
    /// </summary>
    private: typedef std::map<std::size_t, std::size_t> PartitionToDriveMap;

    private: PartitionToDriveMap partitionToDrive;
    //private: std::vector<PartitionInfo

    private: typedef std::map<std::string, std::size_t> PathToDriveMap;

    private: PathToDriveMap paths;
    */

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_HARDWARE_WINDOWSBASICSTOREINFOREADER_H
