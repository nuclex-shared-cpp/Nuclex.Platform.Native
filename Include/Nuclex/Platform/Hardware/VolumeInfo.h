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

#ifndef NUCLEX_PLATFORM_HARDWARE_VOLUMEINFO_H
#define NUCLEX_PLATFORM_HARDWARE_VOLUMEINFO_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std::vector

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  class PartitionInfo; // declared further down in this file

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a storage volume accessible on the system</summary>
  /// <remarks>
  ///   <para>
  ///     All the storage-related terms are overused already, with different meaning, too,
  ///     so finding something unique or unambigious is proving hard. For this library's
  ///     purpose, a &quot;volume&quot; is something that provides individual paritions
  ///     of storage.
  ///   </para>
  ///   <para>
  ///     So a physical disk drive or SSD is a volume on which partitions can exist.
  ///     On Linux, a block device is a volume and it also can have partitions.
  ///     A server reachable via network from which different &quot;shares&quot; or
  ///     &quot;folders&quot; are mounted/connected from the local system would also
  ///     be a volume (and the individual shares/folders would be partitions.
  ///   </para>
  ///   <para>
  ///     The purpose of this is a) knowing which paths share the same drive or server
  ///     so you can sequentialize access to this resource, b) knowing which paths to
  ///     present to the user as their drives or mount points and c) being able to
  ///     figure out the basic storage resources for logging and debugging.
  ///   </para>
  /// </remarks>
  class NUCLEX_PLATFORM_TYPE VolumeInfo {

    /// <summary>Operating-system specific identifier of the drive</summary>
    public: std::string Identifier;

    /// <summary>Human-readable name of the drive's manufacturer</summary>
    public: std::string ManufacturerName;

    /// <summary>Model name of the drive</summary>
    public: std::string ModelName;

    /// <summary>Total capacity of this drive in megabytes (1024-based)</summary>
    public: std::size_t CapacityInMegabytes;

    /// <summary>Whether this is a solid state drive</summary>
    public: bool IsSolidState;

    /// <summary>Detailed information about the mounted partitions from the drive</summary>
    public: std::vector<PartitionInfo> Partitions;

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a partition mounted for access by the system</summary>
  class NUCLEX_PLATFORM_TYPE PartitionInfo {

    /// <summary>Root path to which this partition has been mounted</summary>
    public: std::string MountPath;

    /// <summary>Label that has been assigned to the partition via the file system</summary>
    public: std::string Label;

    /// <summary>Total (theoretical) capacity of the partition in megabytes</summary>
    public: std::size_t CapacityInMegabytes;

    /// <summary>Name describing the type of file system used</summary>
    public: std::string FileSystemName;

  };

  // ------------------------------------------------------------------------------------------- //

  // QueryFreeSpace() as separate method

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_VOLUMEINFO_H
