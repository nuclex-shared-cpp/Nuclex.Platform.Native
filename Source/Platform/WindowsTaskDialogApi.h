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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSTASKDIALOGAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSTASKDIALOGAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

#include <CommCtrl.h> // for TaskDialog(), TASKDIALOG_COMMON_BUTTON_FLAGS

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps task dialog functions from the Windows desktop API</summary>
  class WindowsTaskDialogApi {

    /// <summary>Displays a simple task dialog with text and buttons</summary>
    /// <param name="ownerWindowHandle">
    ///   Window that will become the parent of the task dialog and modally display it
    /// </param>
    /// <param name="resourceInstanceHandle">
    ///   Handle of the process or loaded library containing the resources that will
    ///   be accessed to find the custom icon, if specified
    /// </param>
    /// <param name="windowTitle">
    ///   Text that will appear in the title bar of the task dialog
    /// </param>
    /// <param name="mainInstruction">
    ///   Topic of single-line summary of the message, will be displayed in a larger
    ///   font above the actual message
    /// </param>
    /// <param name="content">
    ///   Message that describes the information you wish to pass to the user. Will
    ///   be displayed in a smaller font with automatic line breaks.
    /// </param>
    /// <param name="commonButtonFlags">
    ///   Combination of button flags that controls which buttons will be available
    /// </param>
    /// <param name="iconResource">
    ///   Name of the resource in the executable or loaded library indicated by the
    ///   <paramref name="resourceInstanceHandle" /> parameter or a standard resource.
    /// </param>
    public: static int TaskDialog(
      ::HWND ownerWindowHandle,
      ::HINSTANCE resourceInstanceHandle,
      const std::string &windowTitle,
      const std::string &mainInstruction,
      const std::string &content,
      ::TASKDIALOG_COMMON_BUTTON_FLAGS commonButtonFlags,
      const std::wstring::value_type *iconResource = nullptr
    );

    /// <summary>Displays a simple task dialog with text and buttons</summary>
    /// <param name="configuration">
    ///   Advanced configuration for the task dialog that controls the text being
    ///   displayed and the options available to the user
    /// </param>
    /// <param name="clickedButtonId">
    ///   Id of the clicked submit button or command link, if any
    /// </param>
    /// <param name="selectedRadioButtonId">
    ///   If of the radio button that has been selected by the user
    /// </param>
    /// <param name="verificationCheckBoxChecked">
    ///   Whether the confirmation / verification check box was checked
    /// </param>
    public: static void TaskDialog(
      const ::TASKDIALOGCONFIG &configuration,
      int *clickedButtonId,
      int *selectedRadioButtonId,
      bool *verificationCheckBoxChecked
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSTASKDIALOGAPI_H
