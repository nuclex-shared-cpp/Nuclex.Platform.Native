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
