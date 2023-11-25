#pragma region CPL License
/*
Nuclex Videl
Copyright (C) 2020-2021 Nuclex Development Labs

This application is free software; you can redistribute it and/or
modify it under the terms of the IBM Common Public License as
published by the IBM Corporation; either version 1.0 of the
License, or (at your option) any later version.

This application is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
IBM Common Public License for more details.

You should have received a copy of the IBM Common Public
License along with this application
*/
#pragma endregion // CPL License

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSREGISTRYAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSREGISTRYAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <optional> // for std::optional

#include "WindowsApi.h" // for ::HKEY via Windows.h

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
