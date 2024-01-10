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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "./WindowsBasicVolumeInfoReader.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/ScopeGuard.h> // for ScopeGuard
#include <Nuclex/Support/Text/StringConverter.h> // for StringConverter

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

#include "../Platform/WindowsSysInfoApi.h" // for WindowsSysInfoApi
#include "../Platform/WindowsFileApi.h" // for WindowsFileApi

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Removes the trailing slash from a string, if present</summary>
  /// <param name="textWithSlash">String from which the trailing slash will be removed</param>
  void removeTrailingSlash(std::wstring &textWithSlash) {
    std::wstring::size_type length = textWithSlash.length();
    if(length >= 1) {
      if((textWithSlash[length - 1] == u8'\\') || (textWithSlash[length - 1] == u8'/')) {
        textWithSlash.resize(length - 1);
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //
  
  /// <summary>Determines the reported store type from Windows' DEVICE_TYPE enum</summary>
  /// <param name="deviceType">Device type that has been reported by Windows</param>
  /// <returns>Store type as which this storage unit should be exposed</returns>
  Nuclex::Platform::Hardware::StoreType storeTypeFromDeviceType(DEVICE_TYPE deviceType) {
    switch(deviceType) {
      case FILE_DEVICE_CD_ROM:
      case FILE_DEVICE_CD_ROM_FILE_SYSTEM:
      case FILE_DEVICE_DVD:
      case FILE_DEVICE_TAPE_FILE_SYSTEM:
      case FILE_DEVICE_TAPE: {
        return Nuclex::Platform::Hardware::StoreType::LocalDiscDrive;
      }

      case FILE_DEVICE_CONTROLLER:
      case FILE_DEVICE_DISK:
      case FILE_DEVICE_DISK_FILE_SYSTEM:
      case FILE_DEVICE_FILE_SYSTEM:
      case FILE_DEVICE_VIRTUAL_DISK:
      case FILE_DEVICE_VIRTUAL_BLOCK:
      case FILE_DEVICE_NVDIMM:
      case FILE_DEVICE_PERSISTENT_MEMORY: {
        return Nuclex::Platform::Hardware::StoreType::LocalInternalDrive;
      }

      case FILE_DEVICE_MASS_STORAGE:
      case FILE_DEVICE_BLUETOOTH:
      case FILE_DEVICE_USBEX:
      case FILE_DEVICE_SMARTCARD: {
        return Nuclex::Platform::Hardware::StoreType::LocalInternalDrive;
      }

      case FILE_DEVICE_DFS:
      case FILE_DEVICE_DATALINK:
      case FILE_DEVICE_MULTI_UNC_PROVIDER:
      case FILE_DEVICE_NETWORK:
      case FILE_DEVICE_NETWORK_BROWSER:
      case FILE_DEVICE_NETWORK_FILE_SYSTEM:
      case FILE_DEVICE_SMB:
      case FILE_DEVICE_DFS_FILE_SYSTEM:
      case FILE_DEVICE_DFS_VOLUME:
      case FILE_DEVICE_VMBUS: {
        return Nuclex::Platform::Hardware::StoreType::NetworkServer;
      }

      default: {
        return Nuclex::Platform::Hardware::StoreType::Unknown;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Converts a serial number stored in a DWORD into a string</summary>
  /// <param name="serialNumber">Serial number that will be converted into a string</param>
  /// <returns>A string stating the serial number in the style of the DOS prompt</returns>
  std::string hexStringFromSerialNumber(DWORD serialNumber) {
    char hexadecimalNumbers[] = u8"0123456789ABCDEF";

    std::string result;
    result.resize(9);

    for(std::size_t index = 0; index < 9; ++index) {
      if(index == 4) {
        result[4] = '-';
      } else {
        std::size_t numberIndex = (serialNumber & 0xF);
        result[8 - index] = hexadecimalNumbers[numberIndex];
        serialNumber >>= 4;
      }
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicVolumeInfoReader::EnumerateWindowsVolumes() {

    // Enumerate all the volumes the Windows system knows about. In the context of our
    // reported topology, these are on the level of partitions - mapped network shares,
    // disk drives and actual partitions on hard disks or SSD.
    std::wstring volumeName;
    ::HANDLE findVolumeHandle = Platform::WindowsFileApi::FindFirstVolume(volumeName);
    {
      ON_SCOPE_EXIT {
        Platform::WindowsFileApi::FindVolumeClose(findVolumeHandle, false);
      };

      // Placed at this level so it isn't reallocated every loop.
      std::vector<std::string> mappedPaths;
      std::vector<::DISK_EXTENT> diskExtents;

      // Keep enumerating until we have fetched each volume the Windows API is letting us see.
      bool nextVolumeEnumerated;
      do {
        using Nuclex::Support::Text::StringConverter;

        // Check the path names under which this volume is mounted. While this may return
        // no paths, a successful return at least tells us that the volume name is valid.
        bool wasValid = Platform::WindowsFileApi::TryGetVolumePathNamesForVolumeName(
          volumeName, mappedPaths
        );
        if(wasValid) {

          // Query some basic information while we have the volume name with trailing slash.
          std::string fileSystem;
          std::string label;
          DWORD serialNumber;
          {
            bool volumeInformationAvailable = (
              Platform::WindowsFileApi::TryGetVolumeInformation(
                volumeName, serialNumber, label, fileSystem
              )
            );
            if(unlikely(!volumeInformationAvailable)) {
              serialNumber = 0;
            }
          }

          // Query the free and available disk space using the naive way
          #if 0
          std::uint64_t freeByteCount;
          std::uint64_t totalByteCount;
          bool freeSpaceQueried = Platform::WindowsSysInfoApi::TryGetDiskFreeSpace(
            volumeName, freeByteCount, totalByteCount
          );
          bool extentsUsable = false;
          #endif

          // Remove the trailing slash, if any. If it is present, the CreateFileW() method will
          // open the root directory of the volume as a file handle, rather than opening
          // the volume itself. Yes, this API is very messed up and tries to budger functionality
          // together that would have deserved its own, separate API methods.
          removeTrailingSlash(volumeName);

          // Query for the device number, this contains the physical device index and
          // the type of the device which we also want to provide to the caller.
          ::STORAGE_DEVICE_NUMBER deviceNumber;
          std::optional<bool> isSolidStateDrive;
          std::optional<std::size_t> capacityInMegabytes;
          {
            // Permissions:
            //   GENERIC_READ
            //     Requesting GENERIC_READ permissions succeeds on my Windows development VM,
            //     but fails on my dual-boot Windows system.
            //
            //   FILE_READ_ATTRIBUTES | SYNCHRONIZE | FILE_TRAVERSE,
            //     Found in a snippet (https://stackoverflow.com/questions/33615577),
            //     works for me but one user commented that it fails for him.
            //
            //   0
            //     No permissions seem to be required, so asking for none works.
            //     Microsoft has an example that also works with a zero for permissions:
            //     https://learn.microsoft.com/en-us/windows/win32/devio/calling-deviceiocontrol
            //
            HANDLE volumeFileHandle = (
              Platform::WindowsFileApi::TryOpenExistingFileForSharedReading(volumeName, 0)
            );
            if(likely(volumeFileHandle != INVALID_HANDLE_VALUE)) {
              ON_SCOPE_EXIT{
                Platform::WindowsFileApi::CloseFile(volumeFileHandle, false);
              };

              // Obtain the physical device number of the device the volume is on
              Platform::WindowsDeviceApi::DeviceIoControlStorageGetDeviceNumbers(
                volumeFileHandle, deviceNumber
              );

              // Our basic decider for whether this is a solid state drive is whether
              // this drive reports a seek penalty (meaning it has physical read/write
              // heads that need to mechanically move).
              ::DEVICE_SEEK_PENALTY_DESCRIPTOR seekPenaltyDescriptor;
              if(
                Platform::WindowsDeviceApi::TryDeviceIoControlStorageQuerySeekPenaltyProperty(
                  volumeFileHandle, seekPenaltyDescriptor
                )
              ) {
                isSolidStateDrive = (seekPenaltyDescriptor.IncursSeekPenalty == FALSE);
              }

              // But if a device supports the TRIM instruction (pre-erase blocks that
              // are unallocated), it is definitely an SSD, so we query that, too.
              ::DEVICE_TRIM_DESCRIPTOR trimDescriptor;
              if(
                Platform::WindowsDeviceApi::TryDeviceIoControlStorageQueryTrimProperty(
                  volumeFileHandle, trimDescriptor
                )
              ) {
                if(trimDescriptor.TrimEnabled != FALSE) {
                  isSolidStateDrive = true;
                }
              }

              // Now query the disk extents. A volume can span multiple disks,
              // but we disregard that for now because we're only interested in its size.
              if(
                Platform::WindowsDeviceApi::TryDeviceIoControlVolumeGetVolumeDiskExtents(
                  volumeFileHandle, diskExtents
                )
              ) {
                if(diskExtents.size() == 1) {
                  //extentsUsable = true;

                  std::uint64_t lengthInBytes = static_cast<std::uint64_t>(
                    diskExtents.back().ExtentLength.QuadPart
                  );
                  std::uint64_t lengthInKilobytes = (lengthInBytes + 512) / 1024;
                  capacityInMegabytes = static_cast<std::size_t>(
                    (lengthInKilobytes + 512) / 1024
                  );
                }
              }

            } else { // If we're lacking permissions, fall back to some defaults
              deviceNumber.DeviceNumber = static_cast<DWORD>(-1);
              deviceNumber.DeviceType = 0;
            } // volumeFileHandle valid / invalid
          } // deviceNumber query beauty scope

          #if 0
          if(freeSpaceQueried && (!extentsUsable)) {
            std::uint64_t lengthInKilobytes = (totalByteCount + 512) / 1024;
            capacityInMegabytes = static_cast<std::size_t>(
              (lengthInKilobytes + 512) / 1024
            );
          }
          #endif

          // All information collected, integrate the volume into our results list
          addVolumeToNewOrExistingStore(
            deviceNumber.DeviceNumber,
            deviceNumber.DeviceType,
            isSolidStateDrive,
            capacityInMegabytes,
            Nuclex::Support::Text::StringConverter::Utf8FromWide(volumeName),
            serialNumber,
            label,
            fileSystem,
            mappedPaths
          );
        } // if volume name was valid

        // Advance to the next volume in the enumeration
        nextVolumeEnumerated = Platform::WindowsFileApi::FindNextVolume(
          findVolumeHandle, volumeName
        );
      } while(nextVolumeEnumerated);
    } // find volume handle closing scope

  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicVolumeInfoReader::addVolumeToNewOrExistingStore(
    DWORD deviceNumber,
    DEVICE_TYPE deviceType,
    std::optional<bool> isSolidStateDrive,
    std::optional<std::size_t> capacityInMegabytes,
    const std::string &volumeName,
    DWORD serialNumber,
    const std::string &label,
    const std::string &fileSystem,
    const std::vector<std::string> &mappedPaths
  ) {
    StoreType storeType = storeTypeFromDeviceType(deviceType);

    // Get the index of the StoreInfo for the physical device. If this is the first time
    // we're seeing said physical device, create a new StoreInfo.
    std::size_t storeIndex;
    {
      // Always create a new store for local disc drives
      bool createNewStore = (storeType == StoreType::LocalDiscDrive);

      // If an existing store may be targeted, see if we already know this device number
      if(!createNewStore) {
        DeviceNumberToStoreIndexMap::iterator mapIterator = this->deviceNumberToStoreIndex.find(
          deviceNumber
        );
        if(mapIterator == this->deviceNumberToStoreIndex.end()) {
          createNewStore = true; // Device number unknown, new store needed
        } else {
          storeIndex = mapIterator->second; // Add partition to existing store
        }
      }

      // If we decided that a new store is needed, create it and set it up
      if(createNewStore) {
        storeIndex = this->stores.size();

        this->stores.emplace_back();
        StoreInfo &newStore = this->stores.back();

        newStore.Type = storeType;
        newStore.IsSolidState = isSolidStateDrive;

        this->deviceNumberToStoreIndex.emplace(deviceNumber, storeIndex);
      }
    }

    // Now we have a valid store index to either a new store or one that
    // already existed and to which the partition belongs
    StoreInfo &targetStore = this->stores[storeIndex];
    targetStore.Partitions.emplace_back();
    PartitionInfo &newPartition = targetStore.Partitions.back();

    // Enter the basic partition information we obtained
    newPartition.CapacityInMegabytes = capacityInMegabytes;
    newPartition.FileSystem = fileSystem;
    newPartition.Label = label;
    if(serialNumber != 0) {
      newPartition.Serial = hexStringFromSerialNumber(serialNumber);
    }

    std::size_t pathCount = mappedPaths.size();
    for(std::size_t index = 0; index < pathCount; ++index) {
      newPartition.MountPaths.push_back(mappedPaths[index]);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
