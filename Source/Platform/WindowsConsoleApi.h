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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSCONSOLEAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSCONSOLEAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps console functions from the Windows desktop API</summary>
  class WindowsConsoleApi {

    /// <summary>Retrieves the window handle of the process' active console</summary>
    /// <returns>
    ///   The window handle of the console window or NULL if the process does not have a console
    /// </returns>
    public: static ::HWND GetConsoleWindow();

    /// <summary>Retrieves informations about the console buffer of the process</summary>
    /// <param name="consoleScreenBufferHandle">
    ///   Handle of the console screen buffer for which informations will be returned
    /// </param>
    /// <param name="consoleScreenBufferInfo">
    ///   Screen buffer information structure that will receive the requested informations
    /// </returns>
    public: static void GetConsoleScreenBufferInfo(
      ::HANDLE consoleScreenBufferHandle,
      ::CONSOLE_SCREEN_BUFFER_INFO &consoleScreenBufferInfo
    );

    /// <summary>Retrieves informations about the console buffer of the process</summary>
    /// <param name="consoleScreenBufferHandle">
    ///   Handle of the console screen buffer for which informations will be returned
    /// </param>
    /// <returns>A structure describing the console used by he current process</returns>
    public: static ::CONSOLE_SCREEN_BUFFER_INFO GetConsoleScreenBufferInfo(
      ::HANDLE consoleScreenBufferHandle
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSCONSOLEAPI_H
