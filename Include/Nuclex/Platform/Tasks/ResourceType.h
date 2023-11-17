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
  enum class NUCLEX_PLATFORM_TYPE ResourceType : std::size_t {

    /// <summary>Number of CPU cores that will be busied</summary>
    CpuCores,
    /// <summary>Amount of system memory that will be used</summary>
    SystemMemory,
    /// <summary>Amount ot GPU memory that will be used</summary>
    VideoMemory,
    /// <summary>Number of web requests that will be generated</summary>
    /// <remarks>
    ///   I suspect this will not see much use. It allows you to have a global throttle
    ///   on the number of web requests (feel free to abuse it for something else).
    ///   In controlled use cases, it could also do basic load balancing (i.e. add
    ///   3 units supporting 1 web request at a time and tasks running in parallel will
    ///   each be given one of the units - allowing task to use i.e. 3 micro services).
    /// </remarks>
    WebRequests

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Highest value present in the ResourceType enumeration</summary>
  constexpr const std::size_t MaximumResourceType = static_cast<std::size_t>(
    ResourceType::WebRequests
  );

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

#endif // NUCLEX_PLATFORM_TASKS_RESOURCETYPE_H
