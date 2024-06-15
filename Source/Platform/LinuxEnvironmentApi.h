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

#ifndef NUCLEX_PLATFORM_PLATFORM_LINUXENVIRONMENTAPI_H
#define NUCLEX_PLATFORM_PLATFORM_LINUXENVIRONMENTAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include <string> // std::string

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps methods that access environment settings on Linux</summary>
  class LinuxEnvironmentApi {

    /// <summary>Retrieves the value held by an environment variable</summary>
    /// <param name="name">Name of the environment variable that will be looked up</param>
    /// <param name="value">String that will receive the value of the variable</param>
    /// <returns>
    ///   True if the environment variable was retrieved, false if the variable
    ///   did not exist and the <see cref="value" /> parameter is unchanged.
    /// </returns>
    public: static bool GetEnvironmentVariable(const std::string &name, std::string &value);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_LINUX)

#endif // NUCLEX_PLATFORM_PLATFORM_LINUXENVIRONMENTAPI_H
