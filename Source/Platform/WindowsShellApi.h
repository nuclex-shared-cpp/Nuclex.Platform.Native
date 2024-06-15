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
