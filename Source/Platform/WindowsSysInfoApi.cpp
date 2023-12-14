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

  ::HANDLE WindowsSysInfoApi::FindFirstVolume(std::wstring &volumeName) {
    volumeName.resize(MAX_PATH);

    // Use the Windows API to look for volumes. It will fill the first volume name
    // into the provided buffer and require us to count characters to figure out
    // the actual length. There is no ERROR_NO_MORE_FILES documented on MSDN, so
    // it appears that Windows systems are expected to always have at least one volume.
    ::HANDLE findHandle = ::FindFirstVolumeW(volumeName.data(), DWORD(MAX_PATH));
    if(unlikely(findHandle == INVALID_HANDLE_VALUE)) {
      DWORD lastErrorCode = ::GetLastError();
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
        u8"Could not begin storage volume enumeration via FindFirstVolume()",
        lastErrorCode
      );
    }

    // Figure out how long the string is and shrink it to that size. If this threw
    // an exception we'd leak the handle, but all string implementations I know of
    // won't actually perform a new, smaller allocation when the string gets shorter.
    for(std::string::size_type index = 0; index < MAX_PATH; ++index) {
      if(unlikely(volumeName[index] == 0)) {
        volumeName.resize(index);
        break;
      }
    }

    return findHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  bool WindowsSysInfoApi::FindNextVolume(::HANDLE findHandle, std::wstring &volumeName) {
    volumeName.resize(MAX_PATH);

    // Ask for the next storage volume name. The return value of this method indicates
    // only success/error and reaching the end of the volume enumeration is merely
    // a special error code that then comes out of GetLastError()
    BOOL success = ::FindNextVolumeW(findHandle, volumeName.data(), DWORD(MAX_PATH));
    if(unlikely(success == FALSE)) {
      DWORD lastErrorCode = ::GetLastError();

      if(likely(lastErrorCode == ERROR_NO_MORE_FILES)) {
        volumeName.clear();
        return false;
      } else {
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not enumerate next storage volume via FindNextVolume()",
          lastErrorCode
        );
      }
    }

    // Figure out how long the string is and shrink it to that size.
    for(std::string::size_type index = 0; index < MAX_PATH; ++index) {
      if(unlikely(volumeName[index] == 0)) {
        volumeName.resize(index);
        break;
      }
    }

    return true;
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsSysInfoApi::FindVolumeClose(::HANDLE findHandle, bool throwOnError /* = true */) {
    BOOL success = ::FindVolumeClose(findHandle);
    if(throwOnError && unlikely(success == FALSE)) {
      DWORD lastErrorCode = ::GetLastError();
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
        u8"Could not close storage volume enumeration handle via FindVolumeClose()",
        lastErrorCode
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsSysInfoApi::GetVolumePathNamesForVolumeName(
    const std::wstring &volumeName, std::vector<std::string> &pathNames
  ) {
    std::wstring pathNamesAsUtf16;
    pathNamesAsUtf16.resize(MAX_PATH);

    // Try to obtain the list of path names. The GetVolumePathNamesForVolumeNameW() can
    // ask for a larger buffer. We allow it to do that 3 times before we decide that it
    // is malfunctioning (to avoid an infinite loop). The buffer requirement might grow
    // if this method is called right when additional drive letters / paths are assigned.
    std::size_t attemptsRemaining = 3;
    for(;;) {
      DWORD actualOrRequiredLength = static_cast<DWORD>(pathNamesAsUtf16.size());
      BOOL success = ::GetVolumePathNamesForVolumeNameW(
        volumeName.c_str(), pathNamesAsUtf16.data(),
        actualOrRequiredLength, &actualOrRequiredLength
      );
      if(likely(success != FALSE)) {
        pathNamesAsUtf16.resize(static_cast<std::string::size_type>(actualOrRequiredLength));
        break;
      }

      // IF this point is reached, the method returned false and something went wrong,
      // figure out what that was (most likely, our buffer space was exhausted).
      DWORD lastErrorCode = ::GetLastError();
      if(likely(lastErrorCode == ERROR_MORE_DATA)) {
        --attemptsRemaining;
        if(unlikely(attemptsRemaining >= 1)) {
          pathNamesAsUtf16.resize(static_cast<std::string::size_type>(actualOrRequiredLength));
        } else { // Asking for a bigger buffer yet again
          Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
            u8"GetVolumePathNamesForVolumeName() keeps asking for larger buffers",
            lastErrorCode
          );
        }
      } else { // an error other than ERROR_MORE_DATA occurred
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not obtain paths for storage volume via GetVolumePathNamesForVolumeName()",
          lastErrorCode
        );
      }
    } // for(;;) until attempts run out

    pathNames.clear();
    {
      using Nuclex::Support::Text::StringConverter;

      // Step through the string and split it into substrings betreen each
      // zero terminator (so this is essentially a hand-coded string tokenize method...)
      std::wstring::size_type startIndex = 0;
      for(std::wstring::size_type index = 0; index < pathNamesAsUtf16.length(); ++index) {
        if(unlikely(pathNamesAsUtf16[index] == 0)) {
          if(startIndex + 1 < index) { // should be at least 1 character long
            pathNames.push_back(
              StringConverter::Utf8FromWide(
                pathNamesAsUtf16.substr(startIndex, index - startIndex)
              )
            );
          } // if at least 1 character between zero terminators
          startIndex = index + 1; // Next potential path begins one after the zero terminator
        } // if zero terminator was found
      }

      // If the returned path names string ended with text (and was not zero terminated),
      // also process the final bit of text. This branch will likely never be entered
      // on a Windows system since GetVolumePathNamesForVolumeNameW() always terminates
      // its returned string with a zero byte, even when no paths are mapped.
      if(startIndex < pathNamesAsUtf16.length()) {
        if(pathNamesAsUtf16[startIndex] != 0) { // If it's not just a lone zero terminator
          pathNames.push_back(
            StringConverter::Utf8FromWide(pathNamesAsUtf16.substr(startIndex))
          );
        } // if remainder isn't just a lone zero terminator
      } // if more characters follow the last discovered zero terminator
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
