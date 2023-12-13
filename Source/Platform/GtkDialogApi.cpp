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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "GtkDialogApi.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/Text/LexicalAppend.h>

#include "./PosixDynamicLibraryApi.h" // for DlOpen(), DlClose(), DlSym()
#include <dlfcn.h> // for RTLD_LAZY, RTLD_LOCAL

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  void *GtkDialogApi::TryLoadLibrary() {
    void *gtkLibraryHandle = PosixDynamicLibraryApi::TryDlOpen(
      u8"libgtk-3.so", RTLD_LAZY | RTLD_LOCAL
    );
    if(unlikely(gtkLibraryHandle == nullptr)) {
      // TODO Try different GTK versions and or explicit paths?
      return nullptr;
    } else {
      return gtkLibraryHandle;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void GtkDialogApi::UnloadLibrary(void *gtkLibraryHandle, bool throwOnError /* = true */) {
    PosixDynamicLibraryApi::DlClose(gtkLibraryHandle, throwOnError);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)