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

  struct GuiMessageService::PrivateImplementationData {};

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  GuiMessageService::GuiMessageService(
    const std::shared_ptr<ActiveWindowTracker> &activeWindowTracker /* = (
      std::shared_ptr<ActiveWindowTracker>()
    ) */
  ) :
    activeWindowTracker(activeWindowTracker) {}

  // ------------------------------------------------------------------------------------------- //

  GuiMessageService::~GuiMessageService() {}

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