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

#include "WindowsFileApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/UnicodeHelper.h" // for UTF-16 <-> UTF-8 conversion
#include "Nuclex/Support/Text/StringConverter.h" // for StringConverter

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
      nullptr, // security attributes, null means use defaults
      OPEN_EXISTING, // creation disposition
      FILE_ATTRIBUTE_NORMAL,
      nullptr // template file from which attributes should be copied
    );
    if(unlikely(fileHandle == INVALID_HANDLE_VALUE)) {
      DWORD errorCode = ::GetLastError();

      bool errorIsDueToLackingConsole = (
        (errorCode == ERROR_INVALID_HANDLE) ||
        (errorCode == ERROR_FILE_NOT_FOUND)
      );
      if(throwIfNoneExists || unlikely(!errorIsDueToLackingConsole)) {
        std::string errorMessage(u8"Could not open active console screen buffer", 43);
        Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
      }
    }

    return fileHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  HANDLE WindowsFileApi::OpenExistingFileForSharedReading(
    const std::wstring &path
  ) {
    HANDLE volumeFileHandle = ::CreateFileW(
      path.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      0, // flags and attributes
      nullptr // template file to copy permissions from
    );
    if(unlikely(volumeFileHandle == INVALID_HANDLE_VALUE)) {
      using Nuclex::Support::Text::StringConverter;

      DWORD errorCode = ::GetLastError();
      std::string errorMessage(u8"Could not open file '", 21);
      errorMessage.append(StringConverter::Utf8FromWide(path));
      errorMessage.append(u8"' for shared reading", 20);
      Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
    }

    return volumeFileHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsFileApi::CloseFile(::HANDLE fileHandle, bool throwOnError /* = true */) {
    BOOL result = ::CloseHandle(fileHandle);
    if(throwOnError && unlikely(result == FALSE)) {
      DWORD errorCode = ::GetLastError();
      std::string errorMessage(u8"Could not close file handle");
      Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
