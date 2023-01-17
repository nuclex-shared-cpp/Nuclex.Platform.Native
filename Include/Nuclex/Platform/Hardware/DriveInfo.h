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

#ifndef NUCLEX_PLATFORM_HARDWARE_DRIVEINFO_H
#define NUCLEX_PLATFORM_HARDWARE_DRIVEINFO_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std;:vector

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  class PartitionInfo; // declared further down in this file

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a physical drive installed in the system</summary>
  class DriveInfo {

    /// <summary>Whether this is a solid state drive</summary>
    public: bool IsSolidState;

    public: std::string ManufacturerName;

    public: std::string ModelName;

    public: std::size_t CapacityInMegabytes;

    /// <summary>Detailed information about the drive's partitions</summary>
    public: std::vector<PartitionInfo> Partitions;

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a physical CPU installed in the system</summary>
  class PartitionInfo {

    public: std::string MountPoint;

    public: std::size_t SizeInMegabytes;

    public: std::string FileSystemTypeName;

  };

  // ------------------------------------------------------------------------------------------- //

  // QueryFreeSpace() as separate method

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_DRIVEINFO_H
