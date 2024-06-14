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

#ifndef NUCLEX_PLATFORM_ERRORS_INSUFFICIENTPERMISSIONSERROR_H
#define NUCLEX_PLATFORM_ERRORS_INSUFFICIENTPERMISSIONSERROR_H

#include "Nuclex/Platform/Config.h"

#include <stdexcept> // for std::runtime_error

namespace Nuclex { namespace Platform{ namespace Errors {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Indicates that the user or process are lacking permissions to perform some action
  /// </summary>
  class NUCLEX_PLATFORM_TYPE InsufficientPermissionsError : public std::runtime_error {

    /// <summary>Initializes a new insufficient permissions error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_PLATFORM_API explicit InsufficientPermissionsError(
      const std::string &message
    ) :
      std::runtime_error(message) {}

    /// <summary>Initializes a new insufficient permissions error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_PLATFORM_API explicit InsufficientPermissionsError(const char *message) :
      std::runtime_error(message) {}

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Errors

#endif // NUCLEX_PLATFORM_ERRORS_INSUFFICIENTPERMISSIONSERROR_H
