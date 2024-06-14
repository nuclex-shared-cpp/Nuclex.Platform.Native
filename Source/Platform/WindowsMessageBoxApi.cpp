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

#include "WindowsMessageBoxApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/StringConverter.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  int WindowsMessageBoxApi::ShowMessageBox(
    ::HWND ownerWindowHandle,
    const std::string &title,
    const std::string &message,
    ::UINT flags
  ) {
    using Nuclex::Support::Text::StringConverter;

    std::wstring utf16Title = StringConverter::WideFromUtf8(title);
    std::wstring utf16Message = StringConverter::WideFromUtf8(message);
    int result = ::MessageBox(
      ownerWindowHandle,
      utf16Message.c_str(),
      utf16Title.c_str(),
      flags
    );
    if(unlikely(result == 0)) {
      DWORD errorCode = ::GetLastError();
      std::string errorMessage(u8"Could not display a message box to the user");
      Platform::WindowsApi::ThrowExceptionForSystemError(errorMessage, errorCode);
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
