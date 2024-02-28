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

    DWORD bufferSize = static_cast<DWORD>(path.size());
    for(;;) {
      DWORD result = ::GetModuleFileNameW(moduleHandle, path.data(), bufferSize);
      if(likely((result >= 0) && (result < bufferSize))) {
        break;
      }

      DWORD lastErrorCode = ::GetLastError();

      if(likely(result >= bufferSize)) {
        if(lastErrorCode == ERROR_INSUFFICIENT_BUFFER) {
          bufferSize *= 4;
          path.resize(static_cast<std::size_t>(bufferSize));
          continue;
        }
      }

      WindowsApi::ThrowExceptionForSystemError(
        u8"Error retrieving path of loaded module handle",
        lastErrorCode
      );
    }

    return Nuclex::Support::Text::StringConverter::Utf8FromWide(path);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
