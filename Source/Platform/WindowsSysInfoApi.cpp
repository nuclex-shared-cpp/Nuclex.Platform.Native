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

    // This actually parses the SMBIOS/DMI information of the system and can fail with
    // ERROR_INVALID_DATA if said data is malformed. Which is the case for virtual machines
    // running inside VirtualBox.
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
