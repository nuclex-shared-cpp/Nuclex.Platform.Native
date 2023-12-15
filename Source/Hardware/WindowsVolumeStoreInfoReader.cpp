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

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicStoreInfoReader::EnumerateVolumes() {

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
          {
            HANDLE volumeFileHandle = (
              Platform::WindowsFileApi::OpenExistingFileForSharedReading(volumeName)
            );
            {
              ON_SCOPE_EXIT{
                Platform::WindowsFileApi::CloseFile(volumeFileHandle, false);
              };
              Platform::WindowsSysInfoApi::DeviceIoControlStorageGetDeviceNumbers(
                volumeFileHandle, deviceNumber
              );
            } // volume file handle closing scope
          } // deviceNumber query beauty scope

          addVolumeToNewOrExistingStore(
            deviceNumber.DeviceNumber,
            deviceNumber.DeviceType,
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
    const std::string& volumeName,
    DWORD serialNumber,
    const std::string& label,
    const std::string& fileSystem,
    const std::vector<std::string>& mappedPaths
  ) {

  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
