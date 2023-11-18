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

#ifndef NUCLEX_PLATFORM_PLATFORM_GTKDIALOGAPI_H
#define NUCLEX_PLATFORM_PLATFORM_GTKDIALOGAPI_H

#include "Nuclex/Platform/Config.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Offers generic methods for dealing with the GTK dialogs API</summary>
  class GtkDialogApi {

    /// <summary>Attempts to load the dynamic GTK library on the current system</summary>
    /// <returns>A handle for the loaded GTK library or a null pointer if not found</returns>
    public: static void *TryLoadLibrary();

    /// <summary>>
    ///   Unloads the GTK library after it has been loaded via <see cref="TryLoadLibrary" />
    /// </summary>
    /// <param name="gtkLibraryHandle">
    ///   Handle that was returned by <see cref="TryLoadLibrary" /> when the GTK library
    ///   was loaded.
    /// </param>
    /// <param name="throwOnError">
    ///   Whether an exception will be thrown if the library can not be unloaded.
    ///   This is provided to suppress errors in shutdown code or RAII destructors.
    /// </param>
    public: static void UnloadLibrary(void *gtkLibraryHandle, bool throwOnError = true);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_GTKDIALOGAPI_H
