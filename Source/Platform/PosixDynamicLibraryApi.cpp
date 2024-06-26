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

#include "PosixDynamicLibraryApi.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include "PosixApi.h"

#include <Nuclex/Support/Text/LexicalAppend.h>

#include <dlfcn.h> // for ::dlopen()

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  void *PosixDynamicLibraryApi::DlOpen(const std::string &libraryName, int flags) {
    void *sharedObjectHandle = ::dlopen(libraryName.c_str(), flags);
    if(sharedObjectHandle == nullptr) {

      // Grab these immediately before any other call unsets them
      int errorNumber = errno;
      char *dynamicLibraryErrorMessage = ::dlerror();

      // Now build an error message. If ::dlerror() can provide extra information from
      // the dynamic library loader, we display it.
      std::string errorMessage(u8"Could not load dynamic library '");
      errorMessage.append(libraryName);
      errorMessage.append(u8"'");
      if(dynamicLibraryErrorMessage != nullptr) {
        errorMessage.append(u8" - ");
        errorMessage.append(dynamicLibraryErrorMessage);
      }

      // Throw the exception. The textual error description for errno will also be
      // appended to the message at this point.
      PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);

    }

    return sharedObjectHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  void *PosixDynamicLibraryApi::TryDlOpen(const std::string &libraryName, int flags) {
    return ::dlopen(libraryName.c_str(), flags);
  }

  // ------------------------------------------------------------------------------------------- //

  void PosixDynamicLibraryApi::DlClose(
    void *dynamicLibraryHandle, bool throwOnError /* = true */
  ) {
    int result = ::dlclose(dynamicLibraryHandle);
    if(throwOnError && unlikely(result != 0)) {
      int errorNumber = errno;
      PosixApi::ThrowExceptionForSystemError(
        u8"Could not close/unload dynamic library", errorNumber
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void *PosixDynamicLibraryApi::DlSym(
    void *dynamicLibraryHandle, const std::string &symbolName
  ) {
    ::dlsym(dynamicLibraryHandle, symbolName.c_str());
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
