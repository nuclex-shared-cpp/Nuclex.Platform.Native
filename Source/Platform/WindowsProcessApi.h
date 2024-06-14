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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSPROCESSAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSPROCESSAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps process information functions from the Windows desktop API</summary>
  class WindowsProcessApi {

    /// <summary>Retrieves the module handle of the file with the executable image</summary>
    /// <returns>The module handle of the loaded executable image file</returns>
    /// <remarks>
    ///   The module handle is returned without incrementing its reference count, so it
    ///   is a mistake to call ::FreeLibrary() on the returned handle.
    /// </remarks>
    public: static ::HMODULE GetModuleHandle();

    /// <summary>Retrieves the full path of a module loaded into the process</summary>
    /// <param name="moduleHandle">
    ///   Handle of the module whose full path will be retrieved
    /// </param>
    /// <returns>The full path of the module with the specified moduel handle</returns>
    public: static std::string GetModuleFileName(::HMODULE moduleHandle);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSPROCESSAPI_H
