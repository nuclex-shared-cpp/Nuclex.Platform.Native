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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSMESSAGEBOXAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSMESSAGEBOXAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps message box functions from the Windows desktop API</summary>
  class WindowsMessageBoxApi {

    /// <summary>Displays a message box to the user</summary>
    /// <param name="ownerWindowHandle">
    ///   Handle of a window that will become the owner of the message box window
    /// </param>
    /// <param name="title">Window title for the message box window</param>
    /// <param name="message">Message that will be displayed in the text area</param>
    /// <param name="flags">Different flags that control styling and options</param>
    public: static int ShowMessageBox(
      ::HWND ownerWindowHandle,
      const std::string &title,
      const std::string &message,
      ::UINT flags
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSMESSAGEBOXAPI_H
