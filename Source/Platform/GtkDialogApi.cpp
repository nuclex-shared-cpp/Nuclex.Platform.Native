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
