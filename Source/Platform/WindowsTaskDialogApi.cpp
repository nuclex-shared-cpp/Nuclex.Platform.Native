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

#include "WindowsTaskDialogApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include <Nuclex/Support/Text/StringConverter.h> // for UTF-16 <-> UTF-8 conversion

// The TaskDialog entry points (_TaskDialog() and _TaskDialogIndirect() functions) are
// located in comctl32.dll (aka Microsoft.Windows.Common-Controls) version 6.0.
//
// All Windows systems ship with two versions of this DLL, version 5.0 and version 6.0.
// A Windows developer can happily include <CommCtrl.h> and use the version 6.0 functions,
// even compiling and linking with comctl32.lib will work, but at runtime, Windows will
// load comctl32.dll in version 5.0 and smack you with an unhelpful error message:
//
//   "The ordinal 344 could not be located in the dynamic link library" (_TaskDialog)
//   "The ordinal 345 could not be located in the dynamic link library" (_TaskDialogIndirect)
//
// To actually make Windows load comctl32.dll in version 6.0, you need to place an XML
// manifest inside the executable or dll that will be inspected by the dynamic linker.
//
// This is one way of getting the manifest in there, documented in:
//   Windows App Development -> Desktop App User Interface -> Enabling Visual Styles
//   https://learn.microsoft.com/en-us/windows/win32/controls/cookbook-overview
//
#pragma comment( \
  linker, \
  "/manifestdependency:\"" \
    "type = 'win32' " \
    "name = 'Microsoft.Windows.Common-Controls' " \
    "version = '6.0.0.0' " \
    "processorArchitecture = '*' " \
    "publicKeyToken = '6595b64144ccf1df' " \
    "language = '*'\" \
  " \
)

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  int WindowsTaskDialogApi::TaskDialog(
    ::HWND ownerWindowHandle,
    ::HINSTANCE resourceInstanceHandle,
    const std::string &windowTitle,
    const std::string &mainInstruction,
    const std::string &content,
    ::TASKDIALOG_COMMON_BUTTON_FLAGS commonButtonFlags,
    const std::wstring::value_type *iconResource /* = nullptr */
  ) {
    using Nuclex::Support::Text::StringConverter;

    std::wstring utf16WindowTitle = StringConverter::WideFromUtf8(windowTitle);
    std::wstring utf16Instruction = StringConverter::WideFromUtf8(mainInstruction);
    std::wstring utf16Content = StringConverter::WideFromUtf8(content);

    int clickedButtonIndex = -1;
    ::HRESULT result = ::TaskDialog(
      ownerWindowHandle,
      resourceInstanceHandle,
      utf16WindowTitle.c_str(),
      utf16Instruction.c_str(),
      utf16Content.c_str(),
      commonButtonFlags,
      iconResource,
      &clickedButtonIndex
    );
    if(unlikely(FAILED(result))) {
      std::string errorMessage(u8"Could not display a task dialog to the user");
      Platform::WindowsApi::ThrowExceptionForHResult(errorMessage, result);
    }

    return clickedButtonIndex;
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsTaskDialogApi::TaskDialog(
    const ::TASKDIALOGCONFIG& configuration,
    int *clickedButtonId,
    int *selectedRadioButtonId,
    bool *verificationCheckBoxChecked
  ) {
    ::HRESULT result;

    if(verificationCheckBoxChecked == nullptr) {
      result = ::TaskDialogIndirect(
        &configuration,
        clickedButtonId,
        selectedRadioButtonId,
        nullptr
      );
    } else {
      BOOL verificationFlag = FALSE;
      result = ::TaskDialogIndirect(
        &configuration,
        clickedButtonId,
        selectedRadioButtonId,
        &verificationFlag
      );
      *verificationCheckBoxChecked = (verificationFlag != FALSE);
    }
    if(unlikely(FAILED(result))) {
      std::string errorMessage(u8"Could not display a task dialog to the user");
      Platform::WindowsApi::ThrowExceptionForHResult(errorMessage, result);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
