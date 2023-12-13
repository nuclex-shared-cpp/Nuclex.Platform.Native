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

#include "./WindowsWmiCpuInfoReader.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/BitTricks.h> // for BitTricks
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/UnicodeHelper.h> // for UnicodeHelper
#include <Nuclex/Support/ScopeGuard.h> // for ScopeGuard

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

#include "./StringHelper.h" // for StringHelper
#include "../Platform/WindowsWmiApi.h" // for WindowsWmiApi, WindowsApi

#include <array> // for std::array

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>RAII scope within which the COM library will be initialized</summary>
  /// <remarks>
  ///   <para>
  ///     Calls to CoInitializeEx() are reference-counted and per thread, so we do not have to
  ///     worry about messing up other parts of the application but shutting down COM or anything
  ///     like that. 
  ///   </para>
  ///   <para>
  ///     The call to CoInitializeSecurity() may be of concern.
  ///   </para>
  /// </remarks>
  class ComInitializationScope {

    /// <summary>
    ///   Initializes the COM library in this thread and applies default security settings
    /// </summary>
    public: ComInitializationScope();

    /// <summary>Releases or shuts down the COM library in this thread</summary>
    public: ~ComInitializationScope();

  };

  // ------------------------------------------------------------------------------------------- //

  ComInitializationScope::ComInitializationScope() {
    HRESULT result = ::CoInitializeEx(
      nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY
    );
    if(FAILED(result)) {
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForHResult(
        u8"Could not initialize COM via CoInitializeEx()", result
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  ComInitializationScope::~ComInitializationScope() {
    ::CoUninitialize();
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Releases COM objects in an array of IWbemClassObject instances</summary>
  /// <typeparam name="ArrayCapacity">Capacity (not size) of the array used</typeparam>
  template<std::size_t ArrayCapacity>
  class WbemObjectArrayScope {

    /// <summary>Initializes a new COM object array release scope</summary>
    /// <param name="wbemObjects">Array of WBEM objects to release upon destruction</param>
    /// <param name="count">Number of WBEM objects actually stored in the array</param>
    public: WbemObjectArrayScope(
      const std::array<IWbemClassObject *, ArrayCapacity> &wbemObjects,
      std::size_t count
    ) :
      wbemObjects(wbemObjects),
      count(count) {}

    /// <summary>Releases all WBEM objects in the array</summary>
    public: ~WbemObjectArrayScope() {
      for(std::size_t index = 0; index < this->count; ++index) {
        this->wbemObjects[index]->Release();
      }
    }

    /// <summary>Array holding the WBEM objects that will be freed on destruction</summary>
    private: const std::array<IWbemClassObject *, ArrayCapacity> &wbemObjects;
    /// <summary>Number of WBEM objects that are actually stored in the array</summary>
    private: std::size_t count;

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Applies the default security settings to COM for the whole process</summary>
  /// <remarks>
  ///   Well, there we are. A set of global settings that needs to be applied/changed for
  ///   something so basic as querying hardware informations. I've got no idea how to deal
  ///   with this beside not using it (we prefer the Windows API and registry digging) and
  ///   documenting this ugly fallback to slow and global state influencing WMI calls...
  /// </remarks>
  void applyProcessWideDefaultComSecuritySettings() {
    HRESULT result = ::CoInitializeSecurity(
      nullptr, // security desriptor
      -1, // Let COM choose which authentication services to register
      nullptr, // must be null if prior parameter is -1
      nullptr, // reserved, must always be null
      RPC_C_AUTHN_LEVEL_DEFAULT, // let COM pick the authentication level
      RPC_C_IMP_LEVEL_IMPERSONATE, // require impersonation, to access local resources
      nullptr, // authentication list w/info per authentication service, optional
      EOAC_NONE, // no special authentication capabilities needed
      nullptr // also reserved, must always be null
    );
    if(FAILED(result)) {
      Nuclex::Platform::Platform::WindowsApi::ThrowExceptionForHResult(
        u8"Could not apply COM security settings via CoInitializeSecurity()", result
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  void WindowsWmiCpuInfoReader::TryQueryCpuInfos(
    void *userPointer,
    CallbackFunction *callback,
    const std::shared_ptr<const Tasks::CancellationWatcher> &canceller
  ) {
    ComInitializationScope comScope;

    // Yuck. MSDN code examples all do this and we do request a few permissions,
    // so I assume this ugly, global state influencing wart is needed.
    applyProcessWideDefaultComSecuritySettings();

    // Create an instance of Microsoft's WBEM client via COM
    {
      IWbemLocatorPtr wbemLocator;
      HRESULT result = wbemLocator.CreateInstance(CLSID_WbemLocator, nullptr);
      if(FAILED(result)) {
        Platform::WindowsApi::ThrowExceptionForHResult(
          u8"Could not create WbemLocator COM class for WMI queries", result
        );
      }

      // Now use the WBEM client to connect to our local system
      {
        // The root\cimv2 namespace contains most of the system information classes
        // https://learn.microsoft.com/en-us/windows/win32/winrm/windows-remote-management-and-wmi
        // WINE: Complains about lacking timeout support but otherwise works (wine-8.0.2)
        IWbemServicesPtr wbemServices;
        result = wbemLocator->ConnectServer(
          _bstr_t(L"root\\cimv2"), // 
          nullptr, // user, null means to use the process own security context
          nullptr, // password, same as above
          0, // use current locale (should we insist on US locale for numbers?)
          WBEM_FLAG_CONNECT_USE_MAX_WAIT, // limit how long we wait for info
          nullptr, // use NTLM authentication from user running the process
          nullptr, // context, something like a user pointer for special info providers
          &wbemServices
        );
        if(FAILED(result)) {
          Platform::WindowsApi::ThrowExceptionForHResult(
            u8"Could not connect to WbemServices via WbemLocator for WMI queries", result
          );
        }

        // CHECK: Should we call CoSetProxyBlanket() here?
        // I've seen MSDN examples do this, but my understanding of COM is not deep enough
        // to reliably tell whether this is needed and if it can maybe even avoid
        // the call to CoInitializeSecurity().

        // Next, we'll run an SQL-like query to obtain the Win32_Processor instances
        {
          IEnumWbemClassObjectPtr wbemEnumerator;
          result = wbemServices->ExecQuery(
            _bstr_t(L"WQL"), // query language, must always be WQL, nothing else is supported
            _bstr_t(L"SELECT * FROM Win32_Processor"), // the WQL query string
            WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, // result enumerator flags
            nullptr, // context, a kind of user pointer passed to the WMI provider (unused)
            &wbemEnumerator
          );
          if(FAILED(result)) {
            Platform::WindowsApi::ThrowExceptionForHResult(
              u8"Could not get enumerator for Win32_Processor objects via WbemServices query",
              result
            );
          }

          // If the query was accepted, we have now obtain an enumerator object that will
          // let us step through the resulting Win32_Processor objects one by one.
          std::size_t processorIndex = 0;
          do {
            const std::size_t BatchSize = 8;

            std::array<IWbemClassObject *, BatchSize> wbemObjects;
            ULONG objectCount = 0;

            canceller->ThrowIfCanceled();

            // Request the next batch of objects from the enumerator. Most examples on
            // the web query one-by-one but that seems to be mere laziness ;-)
            result = wbemEnumerator->Next(
              5000, // milliseconds timeout (we're local, so this should suffice!)
              BatchSize, // number of objects to query and return
              wbemObjects.data(),
              &objectCount
            );
            if(FAILED(result)) {
              Platform::WindowsApi::ThrowExceptionForHResult(
                u8"Wbem enumerator for Win32_Processor objects failed to provide next set",
                result
              );
            }

            // The enumerator should now have filled our array with 1 or more Win32_Processor
            // objects that we can query for their properties
            {
              WbemObjectArrayScope<BatchSize> wbemObjectsScope(wbemObjects, objectCount);

              // A timeout is still considered an alternate type of success as far as
              // the HRESULT goes. Well, not for us! So we need to catch this separately.
              if(result == WBEM_S_TIMEDOUT) {
                Platform::WindowsApi::ThrowExceptionForHResult(
                  u8"Wbem enumerator timed out while gathering Win32_Processor objects",
                  result
                );
              }

              // Now go over each Win32_Processor in the current batch and produce
              for(std::size_t index = 0; index < objectCount; ++index) {
                using Platform::WindowsWmiApi;

                std::string name = WindowsWmiApi::GetWbemPropertyValueAsString(
                  wbemObjects[index], L"Name"
                );
                std::size_t logicalProcessorCount = (
                  WindowsWmiApi::GetWbemPropertyValueAsUnsignedInteger(
                    wbemObjects[index], L"NumberOfLogicalProcessors"
                  )
                );
                std::size_t coreCount = WindowsWmiApi::GetWbemPropertyValueAsUnsignedInteger(
                  wbemObjects[index], L"NumberOfCores"
                );
                double frequencyInMhz = WindowsWmiApi::GetWbemPropertyValueAsDouble(
                  wbemObjects[index], L"MaxClockSpeed"
                );

                callback(
                  userPointer,
                  processorIndex, coreCount, logicalProcessorCount,
                  name, frequencyInMhz
                );
                ++processorIndex;
              }
            } // wbemObjects scope
          } while(result != WBEM_S_FALSE); // stop when enumerator has run dry
        } // wbemEnumerator scope
      } // wbemServices scope
    } // wbemLocator scope
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
