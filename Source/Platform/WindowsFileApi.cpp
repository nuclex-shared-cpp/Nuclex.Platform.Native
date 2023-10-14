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

#include "WindowsFileApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/UnicodeHelper.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  ::HANDLE WindowsFileApi::OpenActiveConsoleScreenBuffer(bool throwIfNoneExists /* = true */) {
    const wchar_t consoleScreenBufferName[] = L"CONOUT$\0";

    HANDLE fileHandle = ::CreateFileW(
      consoleScreenBufferName,
      GENERIC_READ | GENERIC_WRITE, // desired access
      FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode,
      nullptr,
      OPEN_EXISTING, // creation disposition
      FILE_ATTRIBUTE_NORMAL,
      nullptr
    );
    if(unlikely(fileHandle == INVALID_HANDLE_VALUE)) {
      DWORD errorCode = ::GetLastError();
      if(throwIfNoneExists || (errorCode != ERROR_FILE_NOT_FOUND)) {
        std::string errorMessage(u8"Could not open active console screen buffer");
        Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
      }
    }

    return fileHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsFileApi::CloseFile(::HANDLE fileHandle, bool throwOnError /* = true */) {
    BOOL result = ::CloseHandle(fileHandle);
    if(throwOnError && (result == FALSE)) {
      DWORD errorCode = ::GetLastError();
      std::string errorMessage(u8"Could not close file handle");
      Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
