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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSWMIAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSWMIAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

#include <objbase.h> // for CoInitializeSecurity()
#include <comdef.h> // for _com_ptr_t, _bstr_t, _com_error_t
#include <WbemCli.h> // for IWbemLocator, IWbemService

// WBEM - Web-Based Enterprise Management is an 1996 open standard for managing machines
// on a network. WMI - Windows Management Instrumentation is Microsoft's own implementation
// of said standard. 

// Smart pointer to an IWbemLocator through which connections to WBEM can be established
_COM_SMARTPTR_TYPEDEF(IWbemLocator, IID_IWbemLocator);

// Smart pointer to an IWbemService that lets us run queries on WBEM server
_COM_SMARTPTR_TYPEDEF(IWbemServices, IID_IWbemServices);

// Smart pointer to an IEnumWbemClassObject that enumerates over the results of a WMI query
_COM_SMARTPTR_TYPEDEF(IEnumWbemClassObject, IID_IEnumWbemClassObject);

// Smart pointer to an IWbemClassObject that stores a single row returned by a WMI query
_COM_SMARTPTR_TYPEDEF(IWbemClassObject, IID_IWbemClassObject);

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps various methods from the of the Windows WMI client</summary>
  class WindowsWmiApi {

    /// <summary>Retrieves the value of a WBEM object property as an unsigned integer</summary>
    /// <param name="wbemObject">Object from which the property will be read</param>
    /// <param name="propertyName">Name of the property that will be read</param>
    /// <returns>The value of the requested property as an unsigned integer</returns>
    public: static std::size_t GetWbemPropertyValueAsUnsignedInteger(
      IWbemClassObject *wbemObject, const wchar_t *propertyName
    );

    /// <summary>
    ///   Retrieves a WBEM object property as a double precision floating point value
    /// </summary>
    /// <param name="wbemObject">Object from which the property will be read</param>
    /// <param name="propertyName">Name of the property that will be read</param>
    /// <returns>The value of the requested property as a floating point value</returns>
    public: static double GetWbemPropertyValueAsDouble(
      IWbemClassObject *wbemObject, const wchar_t *propertyName
    );

    /// <summary>Retrieves the value of a WBEM object property as an UTF-8 string</summary>
    /// <param name="wbemObject">Object from which the property will be read</param>
    /// <param name="propertyName">Name of the property that will be read</param>
    /// <returns>The value of the requested property as an UTF-8 string</returns>
    public: static std::string GetWbemPropertyValueAsString(
      IWbemClassObject *wbemObject, const wchar_t *propertyName
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSWMIAPI_H
