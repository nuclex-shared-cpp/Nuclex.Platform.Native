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

#include "WindowsShellApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::invalid_argument

#include "Nuclex/Support/Text/StringConverter.h" // for UTF-16 <-> UTF-8 conversion

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Small RAII helper that frees a pointer via ::CoTaskMemFree()</summary>
  class CoTaskMemFreeScope {

    /// <summary>Initializes a memory freeing scope</summary>
    /// <param name="memoryToFree">Memory that will be freed via ::CoTaskMemFree()</param>
    public: CoTaskMemFreeScope(void *memoryToFree) :
      memoryToFree(memoryToFree) {}

    /// <summary>Frees the memory</summary>
    public: ~CoTaskMemFreeScope() {
      ::CoTaskMemFree(this->memoryToFree);
    }

    /// <summary>Memory that will be freed when the instance is destroyed</summary>
    private: void *memoryToFree;

  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  std::string WindowsShellApi::GetKnownFolderPath(const ::KNOWNFOLDERID &knownFolderId) {
    wchar_t *path = nullptr;

    // Query for the path, this method from the Windows API invents yet another
    // way to pass variable-length strings by returning a pointer that the caller
    // then needs to free via ::CoTaskMemFree()
    HRESULT resultHandle = ::SHGetKnownFolderPath(
      knownFolderId, KF_FLAG_DEFAULT, nullptr, &path
    );
    if(unlikely(FAILED(resultHandle))) {
      std::string errorMessage(u8"Could not determine path of known folder");
      Platform::WindowsApi::ThrowExceptionForHResult(errorMessage, resultHandle);
    }

    // Just to be on the safe side, if the functions succeeds but still doesn't
    // provide a pointer, we want to throw an exception rather than segfault.
    if(unlikely(path == nullptr)) {
      throw std::runtime_error(
        u8"SHGetKnownFolderPath() reported success but did not return a path"
      );
    }

    // Convert the returned UTF-16 string to UTF-8, wrap it and free the original.
    {
      CoTaskMemFreeScope freePathScope(path);
      return Nuclex::Support::Text::StringConverter::Utf8FromWide(path);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
