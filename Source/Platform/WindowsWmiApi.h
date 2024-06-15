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
