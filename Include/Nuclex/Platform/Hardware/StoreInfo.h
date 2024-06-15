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

#ifndef NUCLEX_PLATFORM_HARDWARE_STOREINFO_H
#define NUCLEX_PLATFORM_HARDWARE_STOREINFO_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std::vector
#include <optional> // for std::optional

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  enum class StoreType; // declared further down in this file

  // ------------------------------------------------------------------------------------------- //

  class PartitionInfo; // declared further down in this file

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a storage volume accessible on the system</summary>
  /// <remarks>
  ///   <para>
  ///     All the storage-related terms are a bit overused already, with different meanings,
  ///     too, and no agreed-upon denominators, it seems. For this library's purpose,
  ///     a &quot;store&quot; is something that provides individual partitions or shares.
  ///   </para>
  ///   <para>
  ///     So a physical disk drive or SSD is a store on which partitions can exist.
  ///     On Linux, a block device is a store and it also can have partitions. A server
  ///     reachable via network providing different &quot;shares&quot; or &quot;folders&quot;
  ///     is also a store (with the &quot;shares&quot; or &quot;folders&quot; being seen as
  ///     partitions). And a connected CD/DVD drive would be a store that has partitions
  ///     only when a disc is inserted.
  ///   </para>
  ///   <para>
  ///     The purpose of this is a) knowing which paths share the same drive or server
  ///     so you can sequentialize access to this resource, b) knowing which paths to
  ///     present to the user as their drives or mount points and c) being able to
  ///     figure out the basic storage resources for logging and debugging.
  ///   </para>
  /// </remarks>
  class NUCLEX_PLATFORM_TYPE StoreInfo {

/*
    /// <summary>Operating-system specific identifier of the drive</summary>
    public: std::string Identifier;

    /// <summary>Human-readable name of the drive's manufacturer</summary>
    public: std::string ManufacturerName;

    /// <summary>Model name of the drive</summary>
    public: std::string ModelName;

    /// <summary>Total capacity of this drive in megabytes (1024-based)</summary>
    public: std::size_t CapacityInMegabytes;

    */

    /// <summary>How the store is connected or reachable by the local machine</summary>
    public: StoreType Type;

    /// <summary>Whether this is a solid state drive</summary>
    public: std::optional<bool> IsSolidState;

    /// <summary>Detailed information about the mounted partitions from the drive</summary>
    public: std::vector<PartitionInfo> Partitions;


  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Type of the data store, indi</summary>
  enum class NUCLEX_PLATFORM_TYPE StoreType {

    /// <summary>Unknown drive type</summary>
    Unknown,
    /// <summary>A hard drive or SSD installed inside the machine</summary>
    /// <remarks>
    ///   Would also cover eSATA drives, but from a software perspective, they're identical
    ///   to internal SATA drives.
    /// </remarks>
    LocalInternalDrive,
    /// <summary>An external drive connected via USB or Thunderbolt</summary>
    LocalExternalDrive,
    /// <summary>A CD/DVD disc drive, either internal or external</summary>
    /// <remarks>
    ///   Even if we could figure out of this kind of drive is connected via SATA or USB,
    ///   it doesn't really make a difference. No software is shipped on CD/DVD anymore,
    ///   this type is only useful to filter out such drives from inspection (because it
    ///   might spin up the drive) and help the user distinguish it in any possible UI.
    /// </remarks>
    LocalDiscDrive,
    /// <summary>Another computer or storage device on the network</summary>
    NetworkServer

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a partition mounted for access by the system</summary>
  class NUCLEX_PLATFORM_TYPE PartitionInfo {

    /// <summary>Total (theoretical) capacity of the partition in megabytes</summary>
    public: std::optional<std::size_t> CapacityInMegabytes;

    /// <summary>Label that has been assigned to the partition via the file system</summary>
    public: std::string Label;

    /// <summary>Serial number of the partition</summary>
    public: std::string Serial;

    /// <summary>Name describing the type of file system used</summary>
    public: std::string FileSystem;

    /// <summary>Root path to which this partition has been mounted</summary>
    public: std::vector<std::string> MountPaths;

  };

  // ------------------------------------------------------------------------------------------- //

  // QueryFreeSpace() as separate method

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_STOREINFO_H
