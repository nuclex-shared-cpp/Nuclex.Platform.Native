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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "WindowsRegistryApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <stdexcept> // for std::runtime_error
#include <memory> // for std::unique_ptr
#include <cassert> // for assert()

#include <Nuclex/Support/Text/StringConverter.h> // for StringConverter
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Interprets a registry value as a string</summary>
  /// <param name="valueBytes">Buffer that contains the value</param>
  /// <param name="valueSize">Size of the value in bytes</param>
  /// <param name="valueType">Type of the value</param>
  /// <returns>The value interpreted as a string</returns>
  std::optional<std::string> interpretValueAsString(
    const BYTE *valueBytes, std::size_t valueSize, ::DWORD valueType
  ) {
    switch(valueType) {
      case REG_DWORD: {
        return Nuclex::Support::Text::lexical_cast<std::string>(
          *reinterpret_cast<const std::uint32_t *>(valueBytes)
        );
      }
      case REG_QWORD: {
        return Nuclex::Support::Text::lexical_cast<std::string>(
          *reinterpret_cast<const std::uint64_t *>(valueBytes)
        );
      }
      case REG_SZ: {
        std::wstring valueUtf16(
          reinterpret_cast<const std::wstring::value_type *>(valueBytes),
          (valueSize - 1) / sizeof(std::wstring::value_type)
        );
        return Nuclex::Support::Text::StringConverter::Utf8FromWide(valueUtf16);
      }
      default: {
        throw std::runtime_error(u8"Read registry value had a type we don't support");
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::string> WindowsRegistryApi::QueryStringValue(
    ::HKEY keyHandle, const std::wstring &valueName, std::size_t sizeHint /* = 16 */
  ) {
    ::DWORD valueType;
    ::DWORD valueSize = static_cast<::DWORD>(sizeHint);

    // First, try to use a stack-allocated memory buffer
    if(sizeHint < 17) {
      ::BYTE stackValue[16];
      ::LSTATUS result = ::RegQueryValueExW(
        keyHandle,
        valueName.c_str(),
        nullptr,
        &valueType,
        stackValue,
        &valueSize
      );
      if(likely(result == ERROR_SUCCESS)) {
        return interpretValueAsString(stackValue, valueSize, valueType);
      } else if(likely(result == ERROR_FILE_NOT_FOUND)) { // Value does not exist
        return std::optional<std::string>();
      } else if(unlikely(result != ERROR_MORE_DATA)) {
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not query value stored in registry key", result
        );
      } // Intentional fall-through on ERROR_MORE_DATA
    }

    // Either the caller hinted at a larger size or the value turned out to be larger
    // then hinted. Try a heap-allocated buffer with the hinted or known size.
    for(;;) {
      std::unique_ptr<std::uint8_t[]> heapValue(new std::uint8_t[valueSize]);
      ::LSTATUS result = ::RegQueryValueExW(
        keyHandle,
        valueName.c_str(),
        nullptr,
        &valueType,
        heapValue.get(),
        &valueSize
      );
      if(likely(result == ERROR_SUCCESS)) {
        return interpretValueAsString(heapValue.get(), valueSize, valueType);
      } else if(likely(result == ERROR_FILE_NOT_FOUND)) {
        return std::optional<std::string>();
      }

      // If this was our first attempt, retry with the now known size.
      // Otherwise, we treat even ERROR_MORE_DATA as an error
      if(sizeHint >= 17) {
        if(likely(result == ERROR_MORE_DATA)) {
          sizeHint = 0;
          continue;
        }
      }

      // This point is reached if the ::RegQueryValueExW() method call called two times
      // already, the second time with a bfufer using its self-provided value size.
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForSystemError(
        u8"Could not query value stored in registry key", result
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  ::HKEY WindowsRegistryApi::OpenExistingSubKey(
    ::HKEY parentKeyHandle, const std::wstring &subKeyName
  ) {
    ::HKEY subKeyHandle;

    ::LSTATUS result = ::RegOpenKeyExW(
      parentKeyHandle,
      subKeyName.c_str(),
      0, // options
      KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS,
      &subKeyHandle
    );
    if(unlikely(result != ERROR_SUCCESS)) {
      if(likely(result == ERROR_FILE_NOT_FOUND)) {
        return ::HKEY(nullptr);
      } else {
        std::string message(u8"Could not open registry subkey under ", 37);
        message.append(Nuclex::Support::Text::StringConverter::Utf8FromWide(subKeyName));
        Platform::WindowsApi::ThrowExceptionForSystemError(message, result);
      }
    }

    return subKeyHandle;
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsRegistryApi::CloseKey(::HKEY keyHandle, bool throwOnError /* = true */) {
    ::LSTATUS result = ::RegCloseKey(keyHandle);
    if(throwOnError) {
      if(unlikely(result != ERROR_SUCCESS)) {
        Platform::WindowsApi::ThrowExceptionForSystemError(
          u8"Could not close registry key", result
        );
      }
    } else {
      assert(result == ERROR_SUCCESS);
    }
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
