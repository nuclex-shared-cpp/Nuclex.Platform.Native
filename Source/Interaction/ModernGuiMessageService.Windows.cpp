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

#include "Nuclex/Platform/Interaction/ModernGuiMessageService.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "Nuclex/Platform/Interaction/ActiveWindowTracker.h"

#include "../Platform/WindowsTaskDialogApi.h"

#include <Nuclex/Support/Text/StringConverter.h> // for UTF-16 <-> UTF-8 conversion
#include <vector> // for std::vector

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Tracks the state of a task dialog used for confirmation</summary>
  struct ConfirmationTaskDialogState {

    /// <summary>Number of milliseconds after which the Ok button will be enabled</summary>
    public: std::size_t ConfirmationButtonEnableDelayMilliseconds;

    /// <summary>Whether the Ok button has been enabled already</summary>
    public: bool WasEnableMessageSent;

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Tracks the state of a task dialog used for optional cancellation</summary>
  struct CancellationTaskDialogState {

    /// <summary>Number of milliseconds after which the dialog will be confirmed</summary>
    public: std::size_t AutoConfirmationDelayMilliseconds;

  };

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

  /// <summary>Callback procedure for the confirmation task dialog</summary>
  /// <param name="taskDialogWindowHandle">Window handle of the task dialog</param>
  /// <param name="messageType">Type of notification the callback is invoked for</param>
  /// <param name="firstArgument">First notification-dependent argument</param>
  /// <param name="secondArgument">Second notification-dependent argument</param>
  /// <param name="applicationUserData">User data that was provided to the task dialog</param>
  /// <returns>A result handle that indicates if the callback executed successfully</returns>
  HRESULT ConfirmationTaskDialogCallback(
    ::HWND taskDialogWindowHandle,
    ::UINT messageType,
    ::WPARAM firstArgument,
    ::LPARAM secondArgument,
    ::LONG_PTR applicationUserData
  ) {
    switch(messageType) {

      // Sent by the task dialog after the dialog has been created but before it
      // is displayed to the user
      case TDN_CREATED: {
        // Disable the 'Ok' button on the dialog initially.
        // The return value of this message is ignored according to
        // https://learn.microsoft.com/en-us/windows/win32/controls/tdm-enable-button
        ::SendMessageW(
          taskDialogWindowHandle,
          TDM_ENABLE_BUTTON,
          IDOK,
          0 // Disable button
        );

        break;
      }

      // Sent by the task dialog when the TDF_CALLBACK_TIMER flag is set,
      // roughly every 200 milliseconds
      case TDN_TIMER: {
        ConfirmationTaskDialogState &state = *(
          reinterpret_cast<ConfirmationTaskDialogState *>(applicationUserData)
        );
        if(!state.WasEnableMessageSent) {
          std::size_t totalElapsedMilliseconds = static_cast<std::size_t>(firstArgument);
          if(totalElapsedMilliseconds >= state.ConfirmationButtonEnableDelayMilliseconds) {
            // Enable the 'Ok' button on the dialog.
            // The return value of this message is ignored according to
            // https://learn.microsoft.com/en-us/windows/win32/controls/tdm-enable-button
            ::SendMessageW(
              taskDialogWindowHandle,
              TDM_ENABLE_BUTTON,
              IDOK,
              1 // Enable button
            );
            state.WasEnableMessageSent = true;
          }
        }

        break;
      }

    }

    return S_OK;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Callback procedure for the confirmation task dialog</summary>
  /// <param name="taskDialogWindowHandle">Window handle of the task dialog</param>
  /// <param name="messageType">Type of notification the callback is invoked for</param>
  /// <param name="firstArgument">First notification-dependent argument</param>
  /// <param name="secondArgument">Second notification-dependent argument</param>
  /// <param name="applicationUserData">User data that was provided to the task dialog</param>
  /// <returns>A result handle that indicates if the callback executed successfully</returns>
  HRESULT CancellationTaskDialogCallback(
    ::HWND taskDialogWindowHandle,
    ::UINT messageType,
    ::WPARAM firstArgument,
    ::LPARAM secondArgument,
    ::LONG_PTR applicationUserData
  ) {
    switch(messageType) {

      // Sent by the task dialog when the TDF_CALLBACK_TIMER flag is set,
      // roughly every 200 milliseconds
      case TDN_TIMER: {
        CancellationTaskDialogState &state = *(
          reinterpret_cast<CancellationTaskDialogState *>(applicationUserData)
        );

        std::size_t totalElapsedMilliseconds = static_cast<std::size_t>(firstArgument);
        if(totalElapsedMilliseconds >= state.AutoConfirmationDelayMilliseconds) {
          // Click the 'Ok' button in the dialog.
          // The return value of this message is ignored according to
          // https://learn.microsoft.com/en-us/windows/win32/controls/tdm-click-button
          ::SendMessageW(
            taskDialogWindowHandle,
            TDM_CLICK_BUTTON,
            IDOK,
            0 // Unused, must be zero
          );
        }

        break;
      }

    }

    return S_OK;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  ModernGuiMessageService::ModernGuiMessageService(
    const std::shared_ptr<ActiveWindowTracker> &activeWindowTracker /* = (
      std::shared_ptr<ActiveWindowTracker>()
    ) */
  ) :
    activeWindowTracker(activeWindowTracker) {}

  // ------------------------------------------------------------------------------------------- //

  void ModernGuiMessageService::Inform(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    Platform::WindowsTaskDialogApi::TaskDialog(
      getActiveTopLevelWindow(this->activeWindowTracker),
      ::HINSTANCE(nullptr),
      topic,
      heading,
      message,
      TDCBF_OK_BUTTON,
      TD_INFORMATION_ICON
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void ModernGuiMessageService::Warn(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    Platform::WindowsTaskDialogApi::TaskDialog(
      getActiveTopLevelWindow(this->activeWindowTracker),
      ::HINSTANCE(nullptr),
      topic,
      heading,
      message,
      TDCBF_OK_BUTTON,
      TD_WARNING_ICON
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void ModernGuiMessageService::Complain(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    Platform::WindowsTaskDialogApi::TaskDialog(
      getActiveTopLevelWindow(this->activeWindowTracker),
      ::HINSTANCE(nullptr),
      topic,
      heading,
      message,
      TDCBF_OK_BUTTON,
      TD_ERROR_ICON
    );
  }

  // ------------------------------------------------------------------------------------------- //

  bool ModernGuiMessageService::AskYesNo(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    int selectedButtonId = Platform::WindowsTaskDialogApi::TaskDialog(
      getActiveTopLevelWindow(this->activeWindowTracker),
      ::HINSTANCE(nullptr),
      topic,
      heading,
      message,
      TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
      nullptr // Microsoft docs say that question icon should not be used anymore
    );
    if((selectedButtonId == IDNO) || (selectedButtonId == IDCANCEL)) {
      return false;
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool ModernGuiMessageService::AskOkCancel(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    int selectedButtonId = Platform::WindowsTaskDialogApi::TaskDialog(
      getActiveTopLevelWindow(this->activeWindowTracker),
      ::HINSTANCE(nullptr),
      topic,
      heading,
      message,
      TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
      nullptr // Microsoft docs say that question icon should not be used anymore
    );
    if((selectedButtonId == IDNO) || (selectedButtonId == IDCANCEL)) {
      return false;
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<bool> ModernGuiMessageService::AskYesNoCancel(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    int selectedButtonId = Platform::WindowsTaskDialogApi::TaskDialog(
      getActiveTopLevelWindow(this->activeWindowTracker),
      ::HINSTANCE(nullptr),
      topic,
      heading,
      message,
      TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON,
      nullptr // Microsoft docs say that question icon should not be used anymore
    );
    if(selectedButtonId == IDNO) {
      return false;
    } else if(selectedButtonId == IDCANCEL) {
      return std::optional<bool>();
    } else {
      return true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::size_t> ModernGuiMessageService::GiveChoices(
    const std::string &topic, const std::string &heading, const std::string &message,
    const std::initializer_list<std::string> &choices
  ) {
    using Nuclex::Support::Text::StringConverter;

    std::size_t choiceCount = choices.size();

    // Prepare the command link buttons. Each button needs a UTF-16 conversion of its
    // display text (our exposed API is UTF-8 only and that's the way we want it!),
    // so one contiguous array for the buttons and one to keep the UTF-16 strings alive.
    std::vector<std::wstring> buttonTexts(choiceCount);
    std::vector<::TASKDIALOG_BUTTON> buttons(choiceCount);
    {
      std::size_t buttonIndex = 0;
      for(const std::string& choice : choices) {
        buttonTexts[buttonIndex] = StringConverter::WideFromUtf8(choice);
        // Button IDs are index + 100 to avoid intersection with IDOK, IDCANCEL, ec.
        buttons[buttonIndex].nButtonID = static_cast<int>(buttonIndex + 100);
        buttons[buttonIndex].pszButtonText = buttonTexts[buttonIndex].c_str();
        ++buttonIndex;
      }
    }

    // UTF-16 variants of the window title, heading and message contents
    std::wstring utf16Topic = StringConverter::WideFromUtf8(topic);
    std::wstring utf16Heading = StringConverter::WideFromUtf8(heading);
    std::wstring utf16Message = StringConverter::WideFromUtf8(message);

    // The extended task dialog uses a large configuration structure that we need to
    // initialize completely because almost any number of combinations of radio buttons,
    // command links, buttons and extra text is possible.
    ::TASKDIALOGCONFIG configuration;
    configuration.cbSize = sizeof(configuration);
    configuration.hwndParent = getActiveTopLevelWindow(this->activeWindowTracker);
    configuration.hInstance = ::HINSTANCE(nullptr);
    configuration.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;
    configuration.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    configuration.pszWindowTitle = utf16Topic.c_str();
    configuration.pszMainIcon = nullptr;
    configuration.pszMainInstruction = utf16Heading.c_str();
    configuration.pszContent = utf16Message.c_str();
    configuration.cButtons = static_cast<UINT>(choiceCount);
    configuration.pButtons = buttons.data();
    configuration.nDefaultButton = IDCLOSE;
    configuration.cRadioButtons = 0;
    configuration.pRadioButtons = nullptr;
    configuration.nDefaultRadioButton = 0;
    configuration.pszVerificationText = nullptr;
    configuration.pszExpandedInformation = nullptr;
    configuration.pszExpandedControlText = nullptr;
    configuration.pszCollapsedControlText = nullptr;
    configuration.pszFooterIcon = nullptr;
    configuration.pszFooter = nullptr;
    configuration.pfCallback = nullptr;
    configuration.lpCallbackData = ::LONG_PTR(0);
    configuration.cxWidth = 0; // auto-calculate

    // With the task dialog configuration fully initialized, we can now call
    // the TaskDialogIndirect() function and get the beast displayed.
    int clickedButtonId = -1;
    Platform::WindowsTaskDialogApi::TaskDialog(
      configuration,
      &clickedButtonId,
      nullptr,
      nullptr
    );

    // Our command link button ids begin at 100 (see for loop at the top) in order to
    // avoid intersections with the predefined button ids (IDCANCEL, IDOK, IDYES, etc.),
    // so if the clicked button id is less than 100, the only possible reson is that
    // the user cancelled the task dialog (escape, alt+f4 or cancel button).
    if(clickedButtonId < 100) {
      return std::optional<std::size_t>();
    } else {
      return clickedButtonId - 100;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  bool ModernGuiMessageService::RequestConfirmation(
    const std::string &topic, const std::string &heading, const std::string &message,
    std::chrono::milliseconds buttonEnableDelay /* = std::chrono::milliseconds(2000) */
  ) {
    using Nuclex::Support::Text::StringConverter;

    // UTF-16 variants of the window title, heading and message contents
    std::wstring utf16Topic = StringConverter::WideFromUtf8(topic);
    std::wstring utf16Heading = StringConverter::WideFromUtf8(heading);
    std::wstring utf16Message = StringConverter::WideFromUtf8(message);

    ConfirmationTaskDialogState dialogState;
    dialogState.ConfirmationButtonEnableDelayMilliseconds = static_cast<std::size_t>(
      buttonEnableDelay.count()
    );
    dialogState.WasEnableMessageSent = false;

    // The extended task dialog uses a large configuration structure that we need to
    // initialize completely because almost any number of combinations of radio buttons,
    // command links, buttons and extra text is possible.
    ::TASKDIALOGCONFIG configuration;
    configuration.cbSize = sizeof(configuration);
    configuration.hwndParent = getActiveTopLevelWindow(this->activeWindowTracker);
    configuration.hInstance = ::HINSTANCE(nullptr);
    configuration.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_CALLBACK_TIMER;
    configuration.dwCommonButtons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
    configuration.pszWindowTitle = utf16Topic.c_str();
    configuration.pszMainIcon = nullptr;
    configuration.pszMainInstruction = utf16Heading.c_str();
    configuration.pszContent = utf16Message.c_str();
    configuration.cButtons = 0;
    configuration.pButtons = nullptr;
    configuration.nDefaultButton = IDCLOSE;
    configuration.cRadioButtons = 0;
    configuration.pRadioButtons = nullptr;
    configuration.nDefaultRadioButton = 0;
    configuration.pszVerificationText = nullptr;
    configuration.pszExpandedInformation = nullptr;
    configuration.pszExpandedControlText = nullptr;
    configuration.pszCollapsedControlText = nullptr;
    configuration.pszFooterIcon = nullptr;
    configuration.pszFooter = nullptr;
    configuration.pfCallback = &ConfirmationTaskDialogCallback;
    configuration.lpCallbackData = reinterpret_cast<::LONG_PTR>(&dialogState);
    configuration.cxWidth = 0; // auto-calculate

    // With the task dialog configuration fully initialized, we can now call
    // the TaskDialogIndirect() function and get the beast displayed.
    int clickedButtonId = -1;
    Platform::WindowsTaskDialogApi::TaskDialog(
      configuration,
      &clickedButtonId,
      nullptr,
      nullptr
    );

    return (clickedButtonId == IDOK);
  }

  // ------------------------------------------------------------------------------------------- //

  bool ModernGuiMessageService::OfferCancellation(
    const std::string &topic, const std::string &heading, const std::string &message,
    std::chrono::milliseconds autoAcceptDelay /* = std::chrono::milliseconds(5000) */
  ) {
    using Nuclex::Support::Text::StringConverter;

    // UTF-16 variants of the window title, heading and message contents
    std::wstring utf16Topic = StringConverter::WideFromUtf8(topic);
    std::wstring utf16Heading = StringConverter::WideFromUtf8(heading);
    std::wstring utf16Message = StringConverter::WideFromUtf8(message);

    ConfirmationTaskDialogState dialogState;
    dialogState.ConfirmationButtonEnableDelayMilliseconds = static_cast<std::size_t>(
      autoAcceptDelay.count()
    );
    dialogState.WasEnableMessageSent = false;

    // The extended task dialog uses a large configuration structure that we need to
    // initialize completely because almost any number of combinations of radio buttons,
    // command links, buttons and extra text is possible.
    ::TASKDIALOGCONFIG configuration;
    configuration.cbSize = sizeof(configuration);
    configuration.hwndParent = getActiveTopLevelWindow(this->activeWindowTracker);
    configuration.hInstance = ::HINSTANCE(nullptr);
    configuration.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_CALLBACK_TIMER;
    configuration.dwCommonButtons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
    configuration.pszWindowTitle = utf16Topic.c_str();
    configuration.pszMainIcon = nullptr;
    configuration.pszMainInstruction = utf16Heading.c_str();
    configuration.pszContent = utf16Message.c_str();
    configuration.cButtons = 0;
    configuration.pButtons = nullptr;
    configuration.nDefaultButton = IDCLOSE;
    configuration.cRadioButtons = 0;
    configuration.pRadioButtons = nullptr;
    configuration.nDefaultRadioButton = 0;
    configuration.pszVerificationText = nullptr;
    configuration.pszExpandedInformation = nullptr;
    configuration.pszExpandedControlText = nullptr;
    configuration.pszCollapsedControlText = nullptr;
    configuration.pszFooterIcon = nullptr;
    configuration.pszFooter = nullptr;
    configuration.pfCallback = &CancellationTaskDialogCallback;
    configuration.lpCallbackData = reinterpret_cast<::LONG_PTR>(&dialogState);
    configuration.cxWidth = 0; // auto-calculate

    // With the task dialog configuration fully initialized, we can now call
    // the TaskDialogIndirect() function and get the beast displayed.
    int clickedButtonId = -1;
    Platform::WindowsTaskDialogApi::TaskDialog(
      configuration,
      &clickedButtonId,
      nullptr,
      nullptr
    );

    return (clickedButtonId == IDOK);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
