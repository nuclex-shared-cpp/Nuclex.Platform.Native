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

#include "./WindowsVolumeStoreInfoReader.h"

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
  Nuclex::Platform::Hardware::StoreType StoreTypeFromDeviceType(DEVICE_TYPE deviceType) {
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

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicStoreInfoReader::EnumerateWindowsVolumes() {

    // Enumerate all the volumes the Windows system knows about. In the context of our
    // reported topology, these are on the level of partitions - mapped network shares,
    // disk drives and actual partitions on hard disks or SSD.
    std::wstring volumeName;
    ::HANDLE findVolumeHandle = Platform::WindowsSysInfoApi::FindFirstVolume(volumeName);
    {
      ON_SCOPE_EXIT {
        Platform::WindowsSysInfoApi::FindVolumeClose(findVolumeHandle, false);
      };

      // 
      std::vector<std::string> mappedPaths;

      // Keep enumerating until we have fetched each volume the Windows API is letting us see.
      bool nextVolumeEnumerated;
      do {
        using Nuclex::Support::Text::StringConverter;

        // Check the path names under which this volume is mounted. While this may return
        // no paths, a successful return at least tells us that the volume name is valid.
        bool wasValid = Platform::WindowsSysInfoApi::TryGetVolumePathNamesForVolumeName(
          volumeName, mappedPaths
        );
        if(wasValid) {

          // Query some basic information while we have the volume name with trailing slash.
          DWORD serialNumber;
          std::string fileSystem;
          std::string label;
          Platform::WindowsSysInfoApi::GetVolumeInformation(volumeName, serialNumber, label, fileSystem);

          // Remove the trailing slash, if any. If it is present, the CreateFileW() method will
          // open the root directory of the volume as a file handle, rather than opening
          // the volume itself. Yes, this API is very messed up and tries to budger functionality
          // together that would have deserved its own, separate API methods.
          removeTrailingSlash(volumeName);

          // Query for the device number, this contains the physical device index and
          // the type of the device which we also want to provide to the caller.
          ::STORAGE_DEVICE_NUMBER deviceNumber;
          std::optional<bool> isSolidStateDrive;
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
            } else { // If we're lacking permissions, fall back to some defaults
              deviceNumber.DeviceNumber = static_cast<DWORD>(-1);
              deviceNumber.DeviceType = 0;
            } // volumeFileHandle valid / invalid
          } // deviceNumber query beauty scope

          // All information collected, integrate the volume into our results list
          addVolumeToNewOrExistingStore(
            deviceNumber.DeviceNumber,
            deviceNumber.DeviceType,
            isSolidStateDrive,
            Nuclex::Support::Text::StringConverter::Utf8FromWide(volumeName),
            serialNumber,
            label,
            fileSystem,
            mappedPaths
          );
        } // if volume name was valid

        // Advance to the next volume in the enumeration
        nextVolumeEnumerated = Platform::WindowsSysInfoApi::FindNextVolume(
          findVolumeHandle, volumeName
        );
      } while(nextVolumeEnumerated);
    } // find volume handle closing scope

  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicStoreInfoReader::addVolumeToNewOrExistingStore(
    DWORD deviceNumber,
    DEVICE_TYPE deviceType,
    std::optional<bool> isSolidStateDrive,
    const std::string &volumeName,
    DWORD serialNumber,
    const std::string &label,
    const std::string &fileSystem,
    const std::vector<std::string> &mappedPaths
  ) {
    DeviceNumberToStoreIndexMap::iterator mapIterator = this->deviceNumberToStoreIndex.find(
      deviceNumber
    );
    if(mapIterator == this->deviceNumberToStoreIndex.end()) {
      StoreType storeType = StoreTypeFromDeviceType(deviceType);
      


      
      this->stores.emplace_back();

      //StoreInfo &v = this->stores.back();
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
