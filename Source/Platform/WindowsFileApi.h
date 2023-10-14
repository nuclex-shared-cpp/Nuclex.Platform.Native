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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSFILEAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSFILEAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps file access functions from the Windows file system API</summary>
  class WindowsFileApi {

    /// <summary>Opens the active screen buffer for the process console window</summary>
    /// <param name="throwIfNoneExists">
    ///   Whether to throw an exception if no console screen buffer exists
    /// </param>
    /// <returns>The handle of the process' active console screen buffer</returns>
    public: static HANDLE OpenActiveConsoleScreenBuffer(
      bool throwIfNoneExists = true
    );

    /// <summary>Closes the specified file</summary>
    /// <param name="fileHandle">Handle of the file that will be closed</param>
    /// <param name="throwOnError">
    ///   Whether to throw an exception if the file cannot be closed
    /// </param>
    public: static void CloseFile(HANDLE fileHandle, bool throwOnError = true);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSFILEAPI_H