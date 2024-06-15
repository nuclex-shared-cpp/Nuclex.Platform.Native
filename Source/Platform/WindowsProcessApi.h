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
