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
