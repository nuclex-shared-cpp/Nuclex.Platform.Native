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

#include "WindowsConsoleApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/UnicodeHelper.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  ::HWND WindowsConsoleApi::GetConsoleWindow() {
    ::HWND consoleWindowHandle = ::GetConsoleWindow();
    if(consoleWindowHandle == static_cast<::HWND>(nullptr)) {
      DWORD errorCode = ::GetLastError();
      if(errorCode != ERROR_SUCCESS) {
        std::string errorMessage(u8"Unable to check of process has a console window");
        Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
      }
    }
    
    return consoleWindowHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsConsoleApi::GetConsoleScreenBufferInfo(
    ::HANDLE consoleScreenBufferHandle, ::CONSOLE_SCREEN_BUFFER_INFO &consoleScreenBufferInfo
  ) {
    BOOL result = ::GetConsoleScreenBufferInfo(
      consoleScreenBufferHandle, &consoleScreenBufferInfo
    );
    if(result == FALSE) {
      DWORD errorCode = ::GetLastError();
      if(errorCode != ERROR_SUCCESS) {
        std::string errorMessage(u8"Unable to obtain dimensions of console screen buffer");
        Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  ::CONSOLE_SCREEN_BUFFER_INFO WindowsConsoleApi::GetConsoleScreenBufferInfo(
    ::HANDLE consoleScreenBufferHandle
  ) {
    ::CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(consoleScreenBufferHandle, screenBufferInfo);
    return screenBufferInfo;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Support::Platform

#endif // defined(NUCLEX_SUPPORT_WINDOWS)
