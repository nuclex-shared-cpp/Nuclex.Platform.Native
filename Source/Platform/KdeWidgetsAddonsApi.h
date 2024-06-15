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

#ifndef NUCLEX_PLATFORM_PLATFORM_KDEWIDGETSADDONSAPI_H
#define NUCLEX_PLATFORM_PLATFORM_KDEWIDGETSADDONSAPI_H

#include "Nuclex/Platform/Config.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Offers generic methods for dealing with the KDE widgets addons API</summary>
  class KdeWidgetsAddonsApi {

    /// <summary>
    ///   Attempts to load the dynamic KDE widgets addons library on the current system
    /// </summary>
    /// <returns>A handle for the loaded GTK library or a null pointer if not found</returns>
    public: static void *TryLoadLibrary();

    /// <summary>>
    ///   Unloads the KDE widgets addons library after it has been loaded via
    ///   <see cref="TryLoadLibrary" />
    /// </summary>
    /// <param name="kdeWidgetsAddonsLibraryHandle">
    ///   Handle that was returned by <see cref="TryLoadLibrary" /> when
    ///   the KDE widgets addons library was loaded.
    /// </param>
    /// <param name="throwOnError">
    ///   Whether an exception will be thrown if the library can not be unloaded.
    ///   This is provided to suppress errors in shutdown code or RAII destructors.
    /// </param>
    public: static void UnloadLibrary(
      void *kdeWidgetsAddonsLibraryHandle, bool throwOnError = true
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_KDEWIDGETSADDONSAPI_H
