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

#ifndef NUCLEX_PLATFORM_HARDWARE_STRINGHELPER_H
#define NUCLEX_PLATFORM_HARDWARE_STRINGHELPER_H

#include "Nuclex/Platform/Config.h"

#include <Nuclex/Support/Text/ParserHelper.h>

#include <string> // for std::string

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Helper methods for dealing with strings</summary>
  class StringHelper {

    /// <summary>Locates the next floating-point number within a string</summary>
    /// <param name="text">Text in which the next floating-point numbers will be found</param>
    /// <param name="startIndex">Index from which onwards to look for a number</param>
    /// <returns>The first number found in the string or an empty string_view</returns>
    public: static std::string_view FindNextFloat(
      const std::string_view &text, std::string::size_type startIndex = 0
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_STRINGHELPER_H
