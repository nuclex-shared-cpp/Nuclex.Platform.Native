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

#include "./WindowsRegistryCpuInfoReader.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/BitTricks.h> // for BitTricks
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/ScopeGuard.h> // for ScopeGuard

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

#include "../Platform/WindowsRegistryApi.h" // for WindowsRegistryApi
#include "./StringHelper.h" // for StringHelper

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  bool WindowsRegistryCpuInfoReader::TryReadCpuInfos(
    std::size_t processorCount,
    void *userPointer,
    CallbackFunction *callback,
    const std::shared_ptr<const Tasks::CancellationWatcher> &canceller
  ) {
    using Nuclex::Platform::Platform::WindowsRegistryApi;

    // This path holds information about the individual processors, but it is neither
    // the official way to obtain that information nor is it documented, so we try
    // it (because it's fast and accurate) and fall back to official methods if these
    // keys do not exist.
    std::wstring registryPath(L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\", 45);

    // Set to false if even one processor could not be found in the registry
    // (because that's a smell that something changed and we're off track)
    bool gotEverything = true;

    // Step through all processors for which the registry has subkeys. We blindly assume
    // that these indices are identical to the core index reported through the processor
    // mask in the SYSTEM_LOGICAL_PROCESSOR_INFORMATION structure...
    for(std::size_t index = 0; index < processorCount; ++index) {
      registryPath.append(Nuclex::Support::Text::wlexical_cast<std::wstring>(index));

      // Try to open the subkey holding information for the specific processor
      ::HKEY processorKeyHandle = WindowsRegistryApi::OpenExistingSubKey(
        HKEY_LOCAL_MACHINE, registryPath
      );
      if(processorKeyHandle != ::HKEY(nullptr)) {
        ON_SCOPE_EXIT { WindowsRegistryApi::CloseKey(processorKeyHandle, false); };

        double frequency;
        {
          // The processor's unboosted maximum frequency will be stored in a key named
          // '~MHz' quite appropriately. We need to parse this.
          std::optional<std::string> frequencyString = WindowsRegistryApi::QueryStringValue(
            processorKeyHandle, std::wstring(L"~MHz", 4U), 16U
          );
          if(frequencyString.has_value()) {
            using Nuclex::Support::Text::lexical_cast;
            frequency = lexical_cast<double>(frequencyString.value());
          } else {
            frequency = 0.0;
            gotEverything = false;
          }
        }

        std::string processorName;
        {
          // There's also the processor name (which the processor information query
          // does not report), take that one, too.
          std::optional<std::string> name = WindowsRegistryApi::QueryStringValue(
            processorKeyHandle, std::wstring(L"ProcessorNameString", 19U), 64U
          );
          if(name.has_value()) {
            processorName = name.value();
          } else {
            processorName.assign(u8"<unknown>", 9);
            gotEverything = false;
          }
        }

        // Invoke the callback with the information we got
        callback(userPointer, index, processorName, frequency);

      } else { // Registry subkey for the whole processor was not found
        gotEverything = false;
      }

      canceller->ThrowIfCanceled();

      // Cut off the processor index from the registry path so we can append
      // the index of the next processor that we want to query.
      registryPath.resize(45);
    } // for each processor

    return gotEverything;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
