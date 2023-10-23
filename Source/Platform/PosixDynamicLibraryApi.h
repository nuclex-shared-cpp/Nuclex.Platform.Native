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
