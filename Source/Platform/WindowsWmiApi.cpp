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

#include "WindowsWmiApi.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/UnicodeHelper.h> // for UnicodeHelper

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Fetches the value of a property on a WBEM object</summary>
  /// <param name="wbemObject">WBEM object from which the property will be fetched</param>
  /// <param name="propertyName">Name of the property that will be fetched</param>
  /// <returns>The value of the requested WBEM property</returns>
  _variant_t getWbemPropertyValue(IWbemClassObject *wbemObject, const wchar_t *propertyName) {
    _variant_t value;

    HRESULT result = wbemObject->Get(
      propertyName,
      0, // flags, according to docs must always be 0
      &value,
      nullptr, // &valueType, // Data type of the value according to common information model
      nullptr // "flavor" that indicates how the value was collected
    );
    if(FAILED(result)) {
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForHResult(
        u8"Could not query value of property from Wbem object", result
      );
    }

    return value;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Retrieves a WBEM object property as a double precision floating point value
  /// </summary>
  /// <param name="wbemObject">Object from which the property will be read</param>
  /// <param name="propertyName">Name of the property that will be read</param>
  /// <returns>The value of the requested property as a floating point value</returns>
  std::size_t WindowsWmiApi::GetWbemPropertyValueAsUnsignedInteger(
    IWbemClassObject *wbemObject, const wchar_t *propertyName
  ) {
    _variant_t value = getWbemPropertyValue(wbemObject, propertyName);

    // If the variant contained a numeric value, we can directly convert it into a double
    // ourselves without going through the bloaty COM support code.
    {
      using Nuclex::Support::Text::lexical_cast;
      switch(value.vt) {
        case VT_NULL: { return static_cast<std::size_t>(0); }
        case VT_UI1: { return static_cast<std::size_t>(value.bVal); }
        case VT_I1: { return static_cast<std::size_t>(value.cVal); }
        case VT_UI2: { return static_cast<std::size_t>(value.uiVal); }
        case VT_I2: { return static_cast<std::size_t>(value.iVal); }
        case VT_UI4: { return static_cast<std::size_t>(value.uintVal); }
        case VT_I4: { return static_cast<std::size_t>(value.intVal); }
        case VT_UI8: { return static_cast<std::size_t>(value.lVal); }
        case VT_I8: { return static_cast<std::size_t>(value.ulVal); }
        case VT_R4: { return static_cast<std::size_t>(value.fltVal); }
        case VT_R8: { return static_cast<std::size_t>(value.dblVal); }
      }
    }

    // For everything else, we coerce the variant into a double and rely on whatever
    // the COM supporting code does to obtain our double (it should convert strings, too)
    HRESULT result = ::VariantChangeType(&value, &value, 0, VT_UI8);
    if(FAILED(result)) {
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForHResult(
        u8"Could not convert variant to unsigned 64 bit integer type", result
      );
    }

    return static_cast<std::size_t>(value.ulVal);
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Retrieves a WBEM object property as a double precision floating point value
  /// </summary>
  /// <param name="wbemObject">Object from which the property will be read</param>
  /// <param name="propertyName">Name of the property that will be read</param>
  /// <returns>The value of the requested property as a floating point value</returns>
  double WindowsWmiApi::GetWbemPropertyValueAsDouble(
    IWbemClassObject *wbemObject, const wchar_t *propertyName
  ) {
    _variant_t value = getWbemPropertyValue(wbemObject, propertyName);

    // If the variant contained a numeric value, we can directly convert it into a double
    // ourselves without going through the bloaty COM support code.
    {
      using Nuclex::Support::Text::lexical_cast;
      switch(value.vt) {
        case VT_NULL: { return static_cast<double>(0.0); }
        case VT_UI1: { return static_cast<double>(value.bVal); }
        case VT_I1: { return static_cast<double>(value.cVal); }
        case VT_UI2: { return static_cast<double>(value.uiVal); }
        case VT_I2: { return static_cast<double>(value.iVal); }
        case VT_UI4: { return static_cast<double>(value.uintVal); }
        case VT_I4: { return static_cast<double>(value.intVal); }
        case VT_UI8: { return static_cast<double>(value.lVal); }
        case VT_I8: { return static_cast<double>(value.ulVal); }
        case VT_R4: { return static_cast<double>(value.fltVal); }
        case VT_R8: { return static_cast<double>(value.dblVal); }
      }
    }

    // For everything else, we coerce the variant into a double and rely on whatever
    // the COM supporting code does to obtain our double (it should convert strings, too)
    HRESULT result = ::VariantChangeType(&value, &value, 0, VT_R8);
    if(FAILED(result)) {
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForHResult(
        u8"Could not convert variant to double (64 bit float) type", result
      );
    }

    return value.dblVal;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Retrieves the value of a WBEM object property as an UTF-8 string</summary>
  /// <param name="wbemObject">Object from which the property will be read</param>
  /// <param name="propertyName">Name of the property that will be read</param>
  /// <returns>The value of the requested property as an UTF-8 string</returns>
  std::string WindowsWmiApi::GetWbemPropertyValueAsString(
    IWbemClassObject *wbemObject, const wchar_t *propertyName
  ) {
    _variant_t value = getWbemPropertyValue(wbemObject, propertyName);

    // If the variant contained a numeric value, we can directly convert it into a string
    // ourselves without going through the bloaty COM support code.
    {
      using Nuclex::Support::Text::lexical_cast;
      switch(value.vt) {
        case VT_NULL: { return std::string(); }
        case VT_UI1: {
          return lexical_cast<std::string>(static_cast<std::uint16_t>(value.bVal));
        }
        case VT_I1: {
          return lexical_cast<std::string>(static_cast<std::int16_t>(value.cVal));
        }
        case VT_UI2: {
          return lexical_cast<std::string>(static_cast<std::uint16_t>(value.uiVal));
        }
        case VT_I2: {
          return lexical_cast<std::string>(static_cast<std::int16_t>(value.iVal));
        }
        case VT_UI4: {
          return lexical_cast<std::string>(static_cast<std::uint32_t>(value.uintVal));
        }
        case VT_I4: {
          return lexical_cast<std::string>(static_cast<std::int32_t>(value.intVal));
        }
        case VT_UI8: {
          return lexical_cast<std::string>(static_cast<std::uint64_t>(value.ulVal));
        }
        case VT_I8: {
          return lexical_cast<std::string>(static_cast<std::int64_t>(value.lVal));
        }
        case VT_R4: {
          return lexical_cast<std::string>(static_cast<float>(value.fltVal));
        }
        case VT_R8: {
          return lexical_cast<std::string>(static_cast<double>(value.dblVal));
        }
      }
    }

    // For everything else, we coerce the variant into a UTF-16 string and then simply convert
    // that into our UTF-8 return string.
    if(value.vt != VT_BSTR) {
      HRESULT result = ::VariantChangeType(&value, &value, VARIANT_ALPHABOOL, VT_BSTR);
      if(FAILED(result)) {
        Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForHResult(
          u8"Could not convert variant to BSTR (COM/OLE unicode string) type", result
        );
      }
    }

    // Convert the UTF-16 string stored in the variant into a UTF-8 string
    std::string utf8Value;
    {
      std::size_t length = static_cast<std::size_t>(::SysStringLen(value.bstrVal));
      utf8Value.resize(static_cast<std::size_t>(length));
      {
        using Nuclex::Support::Text::UnicodeHelper;
        typedef UnicodeHelper::Char8Type Char8Type;

        // BSTR is a pointer to the string (so it works as a (wchar_t *) or in Microsoft
        // language, an LPWSTR). The length is stored in the 4 bytes before the pointer.
        // Since COM is a binary standard (locking down how stuff has to be stored in memory
        // in order to support different compilers/languages), it is entirely safe to just
        // waltz into the BSTR's memory and read the UTF-16 string.
        const char16_t *current = reinterpret_cast<const char16_t *>(value.bstrVal);
        const char16_t *end = current + length;
        Char8Type *write = reinterpret_cast<Char8Type *>(utf8Value.data());
        while(current < end) {
          char32_t codePoint = UnicodeHelper::ReadCodePoint(current, end);
          if(codePoint == char32_t(-1)) {
            break;
          }
          UnicodeHelper::WriteCodePoint(write, codePoint);
        }

        utf8Value.resize(
          write - reinterpret_cast<Char8Type *>(utf8Value.data())
        );
      }
    }

    return utf8Value;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)
