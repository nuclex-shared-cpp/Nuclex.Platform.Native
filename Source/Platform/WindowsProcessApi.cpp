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
