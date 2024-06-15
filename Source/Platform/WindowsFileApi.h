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

#ifndef NUCLEX_PLATFORM_PLATFORM_WINDOWSFILEAPI_H
#define NUCLEX_PLATFORM_PLATFORM_WINDOWSFILEAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "WindowsApi.h"

#include <cstdint> // for std::uint64_t
#include <vector> // for std::vector

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps file access functions from the Windows file system API</summary>
  class WindowsFileApi {

    /// <summary>Opens the active screen buffer for the process console window</summary>
    /// <param name="throwIfNoneExists">
    ///   Whether to throw an exception if no console screen buffer exists
    /// </param>
    /// <returns>The handle of the process' active console screen buffer</returns>
    public: static HANDLE OpenActiveConsoleScreenBuffer(
      bool throwIfNoneExists = true
    );

    /// <summary>
    ///   Opens an existing file for read access while allowing other processes to access it
    /// </summary>
    /// <param name="path">Path to the file or volume to open for shared reading</param>
    /// <param name="permissions">Permissions to request for the file handle</param>
    /// <returns>The handle representing the opened file or volume</returns>
    /// <remarks>
    ///   An invalid handle (<code>INVALID_HANDLE_VALUE</code>) will be returned if
    ///   the file cannot be found or the process lacks permissions. All other problems
    ///   will result in exception being thrown.
    /// </remarks>
    public: static HANDLE TryOpenExistingFileForSharedReading(
      const std::wstring &path, DWORD permissions = GENERIC_READ
    );

    /// <summary>Closes the specified file</summary>
    /// <param name="fileHandle">Handle of the file that will be closed</param>
    /// <param name="throwOnError">
    ///   Whether to throw an exception if the file cannot be closed
    /// </param>
    public: static void CloseFile(HANDLE fileHandle, bool throwOnError = true);

    /// <summary>
    ///   Starts a volume enumeration and provides the name of the first volume
    /// </summary>
    /// <param name="volumeName">Receives the name of the first volume</param>
    /// <returns>
    ///   The volume enumeration handle which needs to be closed again using
    ///   the <see cref="FindVolumeClose" /> method
    /// </returns>
    public: static ::HANDLE FindFirstVolume(std::wstring &volumeName);

    /// <summary>Advances to the next volume in an active volume enumeration</summary>
    /// <param name="findHandle">
    ///   Handle returned by <see cref="FindFirstVolume" /> that will be advanced
    ///   to the next storage volume on the system
    /// </param>
    /// <param name="volumeName">
    ///   Receives the name of the next storage volume unless the enumeration ended
    /// </param>
    /// <returns>
    ///   True if the next volume name was written into the <paramref name="volumeName" />
    ///   parameter, false if the enumeration had reached the last volume
    /// </returns>
    public: static bool FindNextVolume(::HANDLE findHandle, std::wstring &volumeName);

    /// <summary>Closes a storage volume numeration handle</summary>
    /// <param name="fileHandle">
    ///   Volume enumeration handle originally obtained via
    ///   the <see cref="FindFirstVolume" /> method.
    /// </param>
    /// <param name="throwOnError">
    ///   Whether to throw an exception if the handle could not be closed.
    ///   Can be set to false if this method is used in RAII-like scopes.
    /// </param>
    public: static void FindVolumeClose(::HANDLE findHandle, bool throwOnError = true);

    /// <summary>Retrieves the paths mapped to a volume</summary>
    /// <param name="volumeName">
    ///   Name of the volume, this must be a volume name in the form that is returend by
    ///   <see cref="FindFirstVolume" />.
    /// </param>
    /// <param name="pathNames">
    ///   Receives the path names that are mapped to the volume
    /// </param>
    /// <returns>
    ///   True if the volume name was valid and could be checked (the number of paths
    ///   returned may still be zero)
    /// </returns>
    public: static bool TryGetVolumePathNamesForVolumeName(
      const std::wstring &volumeName, std::vector<std::string> &pathNames
    );

    /// <summary>Retrieves informations about a storage volume</summary>
    /// <param name="volumeName">
    ///   Name of the volume for which informations will be queried
    /// </param>
    /// <param name="serialNumber">Receives the serial number of the volume</param>
    /// <param name="label">Label the user has given the volume</param>
    /// <param name="fileSystem">The name of the file system used on the volume</param>
    /// <returns>
    ///   True if the information about the volume could be queried,
    ///   false if there was a permission problem or the medium was missing (disc drives),
    ///   all other errors lead to an exception being thrown.
    /// </returns>
    public: static bool TryGetVolumeInformation(
      const std::wstring &volumeName,
      DWORD &serialNumber, std::string &label, std::string &fileSystem
    );

    /// <summary>Queries the amount of free and total disk space of a partition</summary>
    /// <param name="path">Path of a volume or directory with trailing slash</param>
    /// <param name="freeByteCount">Receives the number of free bytes on the partition</param>
    /// <param name="totalByteCount">Receives the capacity, in bytes, of the partition</param>
    /// <returns>
    ///   True if the information was obtained, false if one of the acceptable error cases
    ///   happened and the system (or Wine) is unable to provide the information.
    /// </returns>
    public: static bool TryGetDiskFreeSpace(
      const std::wstring &path,
      std::uint64_t &freeByteCount, std::uint64_t &totalByteCount
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_WINDOWSFILEAPI_H
