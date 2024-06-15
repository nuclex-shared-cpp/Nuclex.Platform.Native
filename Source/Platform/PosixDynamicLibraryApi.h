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

#ifndef NUCLEX_PLATFORM_PLATFORM_POSIXDYNAMICLIBRARYAPI_H
#define NUCLEX_PLATFORM_PLATFORM_POSIXDYNAMICLIBRARYAPI_H

#include "Nuclex/Platform/Config.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Helper methods for accessing dynamic libraries via the Posix API</summary>
  class PosixDynamicLibraryApi {

    /// <summary>Loads a dynamic library (*.so) on a Linux system</summary>
    /// <param name="libraryName">
    ///   Name of the dynamic library that will be loaded. If only the library name without
    ///   a path is specified, the dynamic linker will look in the standard paths, usually
    ///   including /lib and /usr/lib.
    /// </param>
    /// <param name="flags">
    ///   Optional flags that indicate how the library should be loaded, is often set to
    ///   (RTLD_LAZY | RTLD_LOCAL) to only load the barely necessary symbols and avoid
    ///   versioning trouble when only a handful of stable functions needs to be called.
    /// </param>
    /// <returns>
    ///   The handle of the loaded dynamic library. Never returns null - if loading fails,
    ///   an exception is thrown with as much information about the load problem as possible
    /// </returns>
    public: static void *DlOpen(const std::string &libraryName, int flags);

    /// <summary>Tries to load an optional dynamic library (*.so) on a Linux system</summary>
    /// <param name="libraryName">
    ///   Name of the dynamic library that will be loaded. If only the library name without
    ///   a path is specified, the dynamic linker will look in the standard paths, usually
    ///   including /lib and /usr/lib.
    /// </param>
    /// <param name="flags">
    ///   Optional flags that indicate how the library should be loaded, is often set to
    ///   (RTLD_LAZY | RTLD_LOCAL) to only load the barely necessary symbols and avoid
    ///   versioning trouble when only a handful of stable functions needs to be called.
    /// </param>
    /// <returns>
    ///   The handle of the loaded dynamic library or null if it could not be loaded
    ///   for any reason.
    /// </returns>
    public: static void *TryDlOpen(const std::string &libraryName, int flags);

    /// <summary>Closes and unloads the specified dynamic library again</summary>
    /// <param name="dynamicLibraryHandle">
    ///   Handle of the loaded dynamic library that will be closed
    /// </param>
    /// <param name="throwOnError">
    ///   Whether to throw an exception in case the library can not be closed
    /// </param>
    public: static void DlClose(void *dynamicLibraryHandle, bool throwOnError = true);

    /// <summary>Looks for the symbol (function, variable) with the specified name</summary>
    /// <param name="dynamicLibraryHandle">
    ///   Handle of the loaded dynamic library in which the symbol will be searched
    /// </param>
    /// <param name="symbol">Name of the symbol that will be looked up</param>
    /// <returns>
    ///   The memory address of the specified symbol. Its type must be exactly known
    ///   and the caller must use the symbol exactly as its type requires.
    /// </returns>
    public: static void *DlSym(void *dynamicLibraryHandle, const std::string &symbolName);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_POSIXDYNAMICLIBRARYAPI_H
