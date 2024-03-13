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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSSHELLAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSSHELLAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

#include <ShlObj.h> // for REFKNOWNFOLDERID

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps shell information functions from the Windows desktop API</summary>
  class WindowsShellApi {

    /// <summary>Looks up the path of a known folder for the current user</summary>
    /// <param name="knownFolderId">GUID of the known folder that will be looked up</param>
    /// <returns>The full path of the specified known folder</returns>
    public: static std::string GetKnownFolderPath(const ::KNOWNFOLDERID &knownFolderId);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSSHELLAPI_H
