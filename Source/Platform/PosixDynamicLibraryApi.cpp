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
