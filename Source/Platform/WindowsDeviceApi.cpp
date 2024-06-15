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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "WindowsDeviceApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/StringConverter.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Checks whether the error code is one that indicates an acceptable problem
  ///   (lacking permissions, missing device or empty disc drive)
  /// </summary>
  /// <param name="errorCode">Error code that will be checked</param>
  /// <returns>True if the error code indicates an acceptable problem</returns>
  bool isAcceptableProblem(DWORD errorCode) {
    return (
      (errorCode == ERROR_INVALID_FUNCTION) || // querying seek penalty in VirtualBox VM
      (errorCode == ERROR_NOT_FOUND) || // unproven: if the volume was disconnected?
      (errorCode == ERROR_FILE_NOT_FOUND) || // unproven: if the volume was disconnected?
      (errorCode == ERROR_ACCESS_DENIED) || // unproven: out of bounds system partitions?
      (errorCode == ERROR_BAD_COMMAND) || // unproven: if IO control code unsupported?
      (errorCode == ERROR_NOT_SUPPORTED) || // querying properties via device IO under Wine
      (errorCode == ERROR_NETWORK_ACCESS_DENIED) || // unproven: if IO control issued on share?
      (errorCode == ERROR_INVALID_PARAMETER) // querying empty CD/DVD drive
    );
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  void WindowsDeviceApi::DeviceIoControlStorageGetDeviceNumbers(
    ::HANDLE volumeFileHandle,
    ::STORAGE_DEVICE_NUMBER &storageDeviceNumber
  ) {
    DWORD returnedByteCount = 0;

    // Request the device number using the DeviceIoControl() method. While the name
    // has an air of a driver-level API, this method can be called unprivileged to fetch
    // perfectly normal informations from a storage device.
    BOOL successful = ::DeviceIoControl(
      volumeFileHandle,
      IOCTL_STORAGE_GET_DEVICE_NUMBER,
      nullptr,
      0,
      reinterpret_cast<LPVOID>(&storageDeviceNumber),
      static_cast<DWORD>(sizeof(storageDeviceNumber)),
      &returnedByteCount,
      nullptr
    );
    if(unlikely(successful == FALSE)) {
      DWORD lastErrorCode = ::GetLastError();
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
        u8"Could not query storage device number via DeviceIoControl()",
        lastErrorCode
      );
    }

    // Additional check in case the structure size changes or incomplete information
    // is provided from the API method (leaving us with partially undefined values)
    if(unlikely(returnedByteCount != static_cast<DWORD>(sizeof(storageDeviceNumber)))) {
      throw std::runtime_error(
        u8"DeviceIoControl() for storage device number returned an unexpected number of bytes"
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool WindowsDeviceApi::TryDeviceIoControlVolumeGetVolumeDiskExtents(
    ::HANDLE volumeFileHandle,
    std::vector<::DISK_EXTENT> &diskExtents
  ) {
    DWORD returnedByteCount = 0;

    // This method fills a variable-sized buffer and even uses part of the buffer
    // to indicate how much more buffer it needs, so start with the default buffer
    // size sufficient for the count and one extent.
    std::vector<std::uint8_t> buffer;
    buffer.resize(sizeof(::VOLUME_DISK_EXTENTS));

    // We do two tries. One with the default buffer size, one with a larger buffer
    // of the size requested by the DeviceIoControl() call
    for(bool firstTry = false;;) {
      BOOL successful = ::DeviceIoControl(
        volumeFileHandle,
        IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
        nullptr,
        0,
        reinterpret_cast<LPVOID>(buffer.data()),
        static_cast<DWORD>(buffer.size()),
        &returnedByteCount,
        nullptr
      );
      if(likely(successful != FALSE)) {
        break; // Success!
      }

      // If this point is reached, something went wrong. Likely, it's only
      // the DeviceIoControl() method asking us for a larger buffer, so check
      // for the exact error code we're presented with.
      DWORD lastErrorCode = ::GetLastError();
      if(likely(lastErrorCode == ERROR_MORE_DATA)) { // buffer too small?
        if(firstTry) {
          std::size_t requiredExtentCount = static_cast<std::size_t>(
            reinterpret_cast<::VOLUME_DISK_EXTENTS *>(buffer.data())->NumberOfDiskExtents
          );
          buffer.resize(
            sizeof(::VOLUME_DISK_EXTENTS) +
            sizeof(::DISK_EXTENT) * requiredExtentCount -
            sizeof(::DISK_EXTENT)
          );
          firstTry = false;
        } else { // We already tried a larger buffer, so complain instead of looping
          Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
            u8"DeviceIoControl() querying disk extents keeps asking for larger buffers",
            lastErrorCode
          );
        }
      } else { // Problem was something other than the buffer size
        if(likely(isAcceptableProblem(lastErrorCode))) {
          return false; // Problem is acceptable, just fail the call
        } else { // Problem is not acceptable, throw an exception
          Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
            u8"Could not query volume disk extents via DeviceIoControl()",
            lastErrorCode
          );
        } // if problem acceptable / inacceptable
      } // if buffer too small / other problem
    }

    // Shovel the disk extents from the typeless buffer into
    // the std::vector our caller provided us with.
    diskExtents.clear();
    {
      ::VOLUME_DISK_EXTENTS *volumeDiskExtents = reinterpret_cast<::VOLUME_DISK_EXTENTS *>(
        buffer.data()
      );
      std::size_t extentCount = static_cast<std::size_t>(
        volumeDiskExtents->NumberOfDiskExtents 
      );
      for(std::size_t index = 0; index < extentCount; ++index) {
        diskExtents.push_back(volumeDiskExtents->Extents[index]);
      }
    }

    return true;
  }

  // ------------------------------------------------------------------------------------------- //

  bool WindowsDeviceApi::TryDeviceIoControlStorageQueryTrimProperty(
    ::HANDLE volumeFileHandle,
    ::DEVICE_TRIM_DESCRIPTOR &deviceTrimDescriptor
  ) {
    DWORD returnedByteCount = 0;

    ::STORAGE_PROPERTY_QUERY query;
    query.PropertyId = StorageDeviceTrimProperty;
    query.QueryType = PropertyStandardQuery;

    deviceTrimDescriptor.Version = 0;
    deviceTrimDescriptor.Size = sizeof(deviceTrimDescriptor);

    // Query for the DeviceTrim property using the STORAGE_QUERY_PROPERTY control code
    BOOL successful = ::DeviceIoControl(
      volumeFileHandle,
      IOCTL_STORAGE_QUERY_PROPERTY,
      reinterpret_cast<LPVOID>(&query),
      static_cast<DWORD>(sizeof(query)),
      reinterpret_cast<LPVOID>(&deviceTrimDescriptor),
      static_cast<DWORD>(sizeof(deviceTrimDescriptor)),
      &returnedByteCount,
      nullptr
    );
    if(unlikely(successful == FALSE)) {
      DWORD lastErrorCode = ::GetLastError();
      if(likely(isAcceptableProblem(lastErrorCode))) {
        return false;
      } else {
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not query TRIM support via DeviceIoControl()",
          lastErrorCode
        );
      }
    }

    // Additional check in case the structure size changes or incomplete information
    // is provided from the API method (leaving us with partially undefined values)
    if(unlikely(returnedByteCount < static_cast<DWORD>(sizeof(deviceTrimDescriptor)))) {
      throw std::runtime_error(
        u8"DeviceIoControl() for TRIM support returned an unexpected number of bytes"
      );
    }

    return true;
  }

  // ------------------------------------------------------------------------------------------- //

  bool WindowsDeviceApi::TryDeviceIoControlStorageQuerySeekPenaltyProperty(
    ::HANDLE volumeFileHandle,
    ::DEVICE_SEEK_PENALTY_DESCRIPTOR &deviceSeekPenaltyDescriptor
  ) {
    DWORD returnedByteCount = 0;

    ::STORAGE_PROPERTY_QUERY query;
    query.PropertyId = StorageDeviceSeekPenaltyProperty;
    query.QueryType = PropertyStandardQuery;

    deviceSeekPenaltyDescriptor.Version = 0;
    deviceSeekPenaltyDescriptor.Size = sizeof(deviceSeekPenaltyDescriptor);

    // Query for the SeekPenalty property using the STORAGE_QUERY_PROPERTY control code
    BOOL successful = ::DeviceIoControl(
      volumeFileHandle,
      IOCTL_STORAGE_QUERY_PROPERTY,
      reinterpret_cast<LPVOID>(&query),
      static_cast<DWORD>(sizeof(query)),
      reinterpret_cast<LPVOID>(&deviceSeekPenaltyDescriptor),
      static_cast<DWORD>(sizeof(deviceSeekPenaltyDescriptor)),
      &returnedByteCount,
      nullptr
    );
    if(unlikely(successful == FALSE)) {
      DWORD lastErrorCode = ::GetLastError();
      if(likely(isAcceptableProblem(lastErrorCode))) {
        return false;
      } else {
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not query seek penalty via DeviceIoControl()",
          lastErrorCode
        );
      }
    }

    // Additional check in case the structure size changes or incomplete information
    // is provided from the API method (leaving us with partially undefined values)
    if(unlikely(returnedByteCount < static_cast<DWORD>(sizeof(deviceSeekPenaltyDescriptor)))) {
      throw std::runtime_error(
        u8"DeviceIoControl() for seek penalty returned an unexpected number of bytes"
      );
    }

    return true;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
