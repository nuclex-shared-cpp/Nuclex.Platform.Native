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

#include "WindowsDeviceApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/StringConverter.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //
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

    // Request the device number using the DeviceIoControl() method. While the name
    // has an air of a driver-level API, this method can be called unprivileged to fetch
    // perfectly normal informations from a storage device.
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
      bool isAcceptedError = (
        (lastErrorCode == ERROR_NOT_FOUND) ||
        (lastErrorCode == ERROR_FILE_NOT_FOUND) ||
        (lastErrorCode == ERROR_ACCESS_DENIED) ||
        (lastErrorCode == ERROR_BAD_COMMAND) ||
        (lastErrorCode == ERROR_NOT_SUPPORTED) ||
        (lastErrorCode == ERROR_NETWORK_ACCESS_DENIED)
      );
      if(likely(isAcceptedError)) {
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

    // Request the device number using the DeviceIoControl() method. While the name
    // has an air of a driver-level API, this method can be called unprivileged to fetch
    // perfectly normal informations from a storage device.
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
      
      bool isAcceptedError = (
        (lastErrorCode == ERROR_NOT_FOUND) ||
        (lastErrorCode == ERROR_FILE_NOT_FOUND) ||
        (lastErrorCode == ERROR_ACCESS_DENIED) ||
        (lastErrorCode == ERROR_BAD_COMMAND) ||
        (lastErrorCode == ERROR_NOT_SUPPORTED) ||
        (lastErrorCode == ERROR_NETWORK_ACCESS_DENIED)
      );
      if(likely(isAcceptedError)) {
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
