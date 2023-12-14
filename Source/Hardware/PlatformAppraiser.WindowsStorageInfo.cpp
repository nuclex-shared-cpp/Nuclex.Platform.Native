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

#include "../Platform/WindowsSysInfoApi.h"

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  std::vector<VolumeInfo> PlatformAppraiser::analyzeStorageVolumesAsync(
    std::shared_ptr<const Tasks::CancellationWatcher> canceller
  ) {
    std::vector<VolumeInfo> result;

    //DWORD consumedDriveLetters = ::GetLogicalDrives();

    // Enumerate all the volumes the Windows system knows about. In the context of our
    // reported topology, these are on the level of partitions - mapped network shares,
    // disk drives and actual partitions on hard disks or SSD.
    std::wstring volumeName;
    ::HANDLE findVolumeHandle = Platform::WindowsSysInfoApi::FindFirstVolume(volumeName);
    {
      ON_SCOPE_EXIT{
        Platform::WindowsSysInfoApi::FindVolumeClose(findVolumeHandle, false);
      };

      // Keep enumerating until we have fetched each volume the Windows API is letting us see.
      bool nextVolumeEnumerated;
      do {
        using Nuclex::Support::Text::StringConverter;

        //result.emplace_back();
        //VolumeInfo &newVolumeInfo = result.back();
        //newVolumeInfo.Identifier = StringConverter::Utf8FromWide(volumeName);

        std::vector<std::string> mappedPaths;
        Platform::WindowsSysInfoApi::GetVolumePathNamesForVolumeName(volumeName, mappedPaths);

        for(std::size_t pathIndex = 0; pathIndex < mappedPaths.size(); ++pathIndex) {
          //newVolumeInfo.Partitions.emplace_back();
          //PartitionInfo &newPartition = newVolumeInfo.Partitions.back();
          //newPartition.MountPath.assign(std::move(mappedPaths[pathIndex]));
        }

        nextVolumeEnumerated = Platform::WindowsSysInfoApi::FindNextVolume(
          findVolumeHandle, volumeName
        );
      } while(nextVolumeEnumerated);
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
