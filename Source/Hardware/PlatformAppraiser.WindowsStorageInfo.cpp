#pragma region CPL License
/*
Nuclex Native Framework
Copyright (C) 2002-2023 Nuclex Development Labs

This library is free software; you can redistribute it and/or
modify it under the terms of the IBM Common Public License as
published by the IBM Corporation; either version 1.0 of the
License, or (at your option) any later version.q

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

#include "Nuclex/Platform/Hardware/PlatformAppraiser.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/BitTricks.h> // for BitTricks
#include <Nuclex/Support/ScopeGuard.h> // for ScopeGuard
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

#include "./WindowsBasicVolumeInfoReader.h" // for WindowsBasicStoreInfoReader

#include "../Platform/WindowsSysInfoApi.h" // for WindowsSysInfoApi
#include "../Platform/WindowsFileApi.h" // for WindowsFileApi

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  std::vector<StoreInfo> PlatformAppraiser::analyzeStorageVolumesAsync(
    std::shared_ptr<const Tasks::CancellationWatcher> canceller
  ) {
    std::vector<StoreInfo> result;

    WindowsBasicVolumeInfoReader r;
    r.EnumerateWindowsVolumes();

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
