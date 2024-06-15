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
