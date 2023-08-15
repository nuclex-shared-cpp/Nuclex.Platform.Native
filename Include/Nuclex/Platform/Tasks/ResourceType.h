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

#ifndef NUCLEX_PLATFORM_TASKS_RESOURCETYPE_H
#define NUCLEX_PLATFORM_TASKS_RESOURCETYPE_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Types of resources that can be occupied on a PC</summary>
  enum class ResourceType : std::size_t {

    /// <summary>Number of CPU cores that will be busied</summary>
    CpuCores,
    /// <summary>Amount of system memory that will be used</summary>
    SystemMemory,
    /// <summary>Amount ot GPU memory that will be used</summary>
    VideoMemory,
    /// <summary>Hard drive index that will be accessed</summary>
    Drive

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Highest value present in the ResourceType enumeration</summary>
  constexpr const std::size_t MaximumResourceType = static_cast<std::size_t>(
    ResourceType::Drive
  );

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_RESOURCETYPE_H
