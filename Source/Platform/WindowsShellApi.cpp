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
