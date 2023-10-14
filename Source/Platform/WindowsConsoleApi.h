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
