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
