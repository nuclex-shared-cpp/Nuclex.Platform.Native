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
