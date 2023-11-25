#pragma region CPL License
/*
Nuclex Native Framework
Copyright (C) 2002-2021 Nuclex Development Labs

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

#include "WindowsSysInfoApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/StringConverter.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  std::uint64_t WindowsSysInfoApi::GetPhysicallyInstalledSystemMemory() {
    ULONGLONG memoryInKilobytes;

    BOOL result = ::GetPhysicallyInstalledSystemMemory(&memoryInKilobytes);
    if(unlikely(result != TRUE)) {
      DWORD lastErrorCode = ::GetLastError();
      WindowsApi::ThrowExceptionForSystemError(
        u8"Could not query physically installed memory", lastErrorCode
      );
    }

    return static_cast<std::uint64_t>(memoryInKilobytes);
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsSysInfoApi::GetGlobalMemoryStatus(::MEMORYSTATUSEX &memoryStatus) {
    memoryStatus.dwLength = sizeof(memoryStatus);

    BOOL result = ::GlobalMemoryStatusEx(&memoryStatus);
    if(unlikely(result == FALSE)) {
      DWORD lastErrorCode = ::GetLastError();
      WindowsApi::ThrowExceptionForSystemError(
        u8"Error querying global memory status", lastErrorCode
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::vector<std::uint8_t> WindowsSysInfoApi::GetLogicalProcessorInformation() {

    // The process information structure works with buffer that's treated as a plain
    // byte array and so its 'bufferSize' argument is in bytes, too. Allocate some
    // space so we've got a chance to fetch all processor information on the first call.
    std::vector<std::uint8_t> logicalProcessorInformationBuffer;
    DWORD bufferSize = DWORD(1024);
    logicalProcessorInformationBuffer.resize(bufferSize);

    // Try to fetch the processor information, first with the pre-allocated buffer and,
    // should that prove too small, once more with the required size reported by
    // the failed call to the ::GetLogicalProcessorInformationEx() function.
    for(bool bufferSizeKnown = false;;) {
      BOOL result = ::GetLogicalProcessorInformation(
        reinterpret_cast<::SYSTEM_LOGICAL_PROCESSOR_INFORMATION *>(
          logicalProcessorInformationBuffer.data()
        ),
        &bufferSize
      );

      // Did the call to ::GetLogicalProcessorInformation() fail?
      if(unlikely(result == FALSE)) {
        DWORD lastErrorCode = ::GetLastError();

        // If this is the first attempt, see if the buffer was too small and, if so,
        // try again using the required buffer size reported by the failed call
        if(!bufferSizeKnown) { // First attempt?
          if(lastErrorCode == ERROR_INSUFFICIENT_BUFFER) { // Buffer too small?
            logicalProcessorInformationBuffer.resize(bufferSize);
            bufferSizeKnown = true;
            continue;
          }
        }

        // Second attempt (using requested buffer size) or other error, nothing we can do
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not query CPU information via GetLogicalProcessorInformation()",
          lastErrorCode
        );
      }

      // If we were still working with the initial 1024 byte buffer, shrink it down
      // to the exact number of bytes that have been placed into it.
      if(!bufferSizeKnown) {
        logicalProcessorInformationBuffer.resize(static_cast<std::size_t>(bufferSize));
      }

      // Success! If this point is reached, no error occurred.
      break;
    }

    return logicalProcessorInformationBuffer;

  }

  // ------------------------------------------------------------------------------------------- //

  std::vector<std::uint8_t> WindowsSysInfoApi::GetLogicalProcessorInformationEx() {

    // The process information structure works with buffer that's treated as a plain
    // byte array and so its 'bufferSize' argument is in bytes, too. Allocate some
    // space so we've got a chance to fetch all processor information on the first call.
    std::vector<std::uint8_t> logicalProcessorInformationBuffer;
    DWORD bufferSize = DWORD(1024);
    logicalProcessorInformationBuffer.resize(bufferSize);

    // Try to fetch the processor information, first with the pre-allocated buffer and,
    // should that prove too small, once more with the required size reported by
    // the failed call to the ::GetLogicalProcessorInformationEx() function.
    for(bool bufferSizeKnown = false;;) {
      BOOL result = ::GetLogicalProcessorInformationEx(
        RelationAll,
        reinterpret_cast<::SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(
          logicalProcessorInformationBuffer.data()
        ),
        &bufferSize
      );

      // Did the call to ::GetLogicalProcessorInformation() fail?
      if(unlikely(result == FALSE)) {
        DWORD lastErrorCode = ::GetLastError();

        // If this is the first attempt, see if the buffer was too small and, if so,
        // try again using the required buffer size reported by the failed call
        if(!bufferSizeKnown) { // First attempt?
          if(lastErrorCode == ERROR_INSUFFICIENT_BUFFER) { // Buffer too small?
            logicalProcessorInformationBuffer.resize(bufferSize);
            bufferSizeKnown = true;
            continue;
          }
        }

        // Second attempt (using requested buffer size) or other error, nothing we can do
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not query CPU information via GetLogicalProcessorInformationEx()",
          lastErrorCode
        );
      }

      // If we were still working with the initial 1024 byte buffer, shrink it down
      // to the exact number of bytes that have been placed into it.
      if(!bufferSizeKnown) {
        logicalProcessorInformationBuffer.resize(static_cast<std::size_t>(bufferSize));
      }

      // Success! If this point is reached, no error occurred.
      break;
    }

    return logicalProcessorInformationBuffer;

  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
