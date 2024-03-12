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

#include "WindowsProcessApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/StringConverter.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  ::HMODULE WindowsProcessApi::GetModuleHandle() {
    ::HMODULE moduleHandle = ::GetModuleHandleW(nullptr);
    if(unlikely(moduleHandle == ::HMODULE(nullptr))) {
      DWORD lastErrorCode = ::GetLastError();
      WindowsApi::ThrowExceptionForSystemError(
        u8"Could not obtain module handle of runnnning executable file",
        lastErrorCode
      );
    }

    return moduleHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string WindowsProcessApi::GetModuleFileName(::HMODULE moduleHandle) {
    std::wstring path(MAX_PATH, L'\0');

    DWORD lastErrorCode;

    // If the path is particularly long, we may need to re-try with larger buffer
    // sizes, so we'll query for the path in a loop.
    DWORD bufferSize = static_cast<DWORD>(path.size());
    for(std::size_t attempt = 0; attempt < 4; ++attempt) {

      // Try to obtain the path. If the function returns a positive length that
      // is less than the buffer size, we know it succeeded.
      DWORD result = ::GetModuleFileNameW(moduleHandle, path.data(), bufferSize);
      if(likely((result >= 0) && (result < bufferSize))) {
        return Nuclex::Support::Text::StringConverter::Utf8FromWide(path);
      }

      // Something went wrong! Immediately capture the error code before any
      // other call may reset or replace it.
      lastErrorCode = ::GetLastError();

      // If the returned length was the buffer size and the last error value is
      // ERROR_INSUFFICIENT_BUFFER, indicating that our buffer was too small,
      // quintuple the size of the buffer and try again. This will result in buffer
      // sizes of 260, 1300, 6500 and 32500 bytes before giving up.
      if(likely(result >= bufferSize)) {
        if(likely(lastErrorCode == ERROR_INSUFFICIENT_BUFFER)) {
          bufferSize *= 5;
          path.resize(static_cast<std::size_t>(bufferSize));
          continue;
        } else if(unlikely(lastErrorCode == ERROR_SUCCESS)) {
          // This could be a success with the length perfectly matching the buffer
          // size, but MSDN docs aren't explicit about this, so better try again
          // with a larger buffer to be safe.
          continue;
        }
      }

      // Error was something other than insufficient buffer space
      WindowsApi::ThrowExceptionForSystemError(
        u8"Error retrieving path of loaded module handle",
        lastErrorCode
      );
    }

    // We gave it 4 tries, but even a 32000+ character buffer didn't
    // make the GetModuleFileNameW() method happy, so we give up.
    WindowsApi::ThrowExceptionForSystemError(
      u8"GetModuleFileNameW() keeps asking for larger buffers",
      lastErrorCode
    );
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
