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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSREGISTRYAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSREGISTRYAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h" // for ::HKEY via Windows.h

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <optional> // for std::optional

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps the API used to interface with the registry on Windows systems</summary>
  class WindowsRegistryApi {

    /// <summary>Retrieves a value stored under a registry key as a string</summary>
    /// <param name="keyHandle">Handle of the registry key in which the value is stored</param>
    /// <param name="valueName">Name of the value in the registry</param>
    /// <param name="sizeHint">Expected size of the value, useful for strings</param>
    /// <returns>The registry value, if it was present</returns>
    public: static std::optional<std::string> WindowsRegistryApi::QueryStringValue(
      ::HKEY keyHandle, const std::wstring &valueName, std::size_t sizeHint = 16
    );

    /// <summary>Opens a subkey below the specified parent registry key</summary>
    /// <param name="parentKeyHandle">Handle of the parent registry key</param>
    /// <param name="subKeyName">Name of the subkey that will be opened</param>
    /// <returns>
    ///   The handle of the opened registry subkey or a null pointer if the key doesn't exist
    /// </returns>
    public: static ::HKEY OpenExistingSubKey(
      ::HKEY parentKeyHandle, const std::wstring &subKeyName
    );

    /// <summary>Closes the specified registry key again</summary>
    /// <param name="keyHandle">Handle of the registry key that will be closed</param>
    /// <param name="throwOnError">
    ///   Whether to throw an exception when closing the registry key fails
    /// </param>
    public: static void CloseKey(::HKEY keyHandle, bool throwOnError = true);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSREGISTRYAPI_H
