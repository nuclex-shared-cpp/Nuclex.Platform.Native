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

#include "Nuclex/Platform/Interaction/GuiMessageService.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "Nuclex/Platform/Interaction/ActiveWindowTracker.h"

#include "../Platform/WindowsMessageBoxApi.h"

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Retrieves the handle of the currently active top-level window</summary>
  /// <param name="activeWindowTracker">
  ///   Window tracker that (may be null) that knows the current top-level window
  /// </param>
  /// <returns>The window handle of the current top-level window or NULL</returns>
  ::HWND getActiveTopLevelWindow(
    const std::shared_ptr<Nuclex::Platform::Interaction::ActiveWindowTracker> &activeWindowTracker
  ) {
    if(static_cast<bool>(activeWindowTracker)) {
      std::any windowHandleAsAny = activeWindowTracker->GetActiveWindow();
      if(windowHandleAsAny.has_value()) {
        return std::any_cast<::HWND>(windowHandleAsAny);
      }
    }

    return static_cast<::HWND>(nullptr);
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  GuiMessageService::GuiMessageService(
    const std::shared_ptr<ActiveWindowTracker> &activeWindowTracker /* = (
      std::shared_ptr<ActiveWindowTracker>()
    ) */
  ) :
    activeWindowTracker(activeWindowTracker) {}

  // ------------------------------------------------------------------------------------------- //

  void GuiMessageService::Inform(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    ::HWND activeWindowHandle = getActiveTopLevelWindow(this->activeWindowTracker);
    Platform::WindowsMessageBoxApi::ShowMessageBox(
      activeWindowHandle,
      topic,
      message,
      (
        (activeWindowHandle == static_cast<::HWND>(nullptr)) ?
        (MB_ICONINFORMATION | MB_OK | MB_TASKMODAL) :
        (MB_ICONINFORMATION | MB_OK | MB_APPLMODAL)
      )
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void GuiMessageService::Warn(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    ::HWND activeWindowHandle = getActiveTopLevelWindow(this->activeWindowTracker);
    Platform::WindowsMessageBoxApi::ShowMessageBox(
      activeWindowHandle,
      topic,
      message,
      (
        (activeWindowHandle == static_cast<::HWND>(nullptr)) ?
        (MB_ICONWARNING | MB_OK | MB_TASKMODAL) :
        (MB_ICONWARNING | MB_OK | MB_APPLMODAL)
      )
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void GuiMessageService::Complain(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    ::HWND activeWindowHandle = getActiveTopLevelWindow(this->activeWindowTracker);
    Platform::WindowsMessageBoxApi::ShowMessageBox(
      activeWindowHandle,
      topic,
      message,
      (
        (activeWindowHandle == static_cast<::HWND>(nullptr)) ?
        (MB_ICONERROR | MB_OK | MB_TASKMODAL) :
        (MB_ICONERROR | MB_OK | MB_APPLMODAL)
      )
    );
  }

  // ------------------------------------------------------------------------------------------- //

  bool GuiMessageService::AskYesNo(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    ::HWND activeWindowHandle = getActiveTopLevelWindow(this->activeWindowTracker);
    int choice = Platform::WindowsMessageBoxApi::ShowMessageBox(
      activeWindowHandle,
      topic,
      message,
      (
        (activeWindowHandle == static_cast<::HWND>(nullptr)) ?
        (MB_ICONQUESTION | MB_YESNO | MB_TASKMODAL) :
        (MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL)
      )
    );
    if((choice == IDNO) || (choice == IDCANCEL) || (choice == IDABORT)) {
      return false;
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool GuiMessageService::AskOkCancel(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    ::HWND activeWindowHandle = getActiveTopLevelWindow(this->activeWindowTracker);
    int choice = Platform::WindowsMessageBoxApi::ShowMessageBox(
      activeWindowHandle,
      topic,
      message,
      (
        (activeWindowHandle == static_cast<::HWND>(nullptr)) ?
        (MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL) :
        (MB_ICONQUESTION | MB_OKCANCEL | MB_APPLMODAL)
      )
    );
    if((choice == IDNO) || (choice == IDCANCEL) || (choice == IDABORT)) {
      return false;
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<bool> GuiMessageService::AskYesNoCancel(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    ::HWND activeWindowHandle = getActiveTopLevelWindow(this->activeWindowTracker);
    int choice = Platform::WindowsMessageBoxApi::ShowMessageBox(
      activeWindowHandle,
      topic,
      message,
      (
        (activeWindowHandle == static_cast<::HWND>(nullptr)) ?
        (MB_ICONQUESTION | MB_YESNOCANCEL | MB_TASKMODAL) :
        (MB_ICONQUESTION | MB_YESNOCANCEL | MB_APPLMODAL)
      )
    );
    if(choice == IDNO) {
      return false;
    } else if((choice == IDCANCEL) || (choice == IDABORT)) {
      return std::optional<bool>();
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // defined(NUCLEX_PLATFORM_WINDOWS)