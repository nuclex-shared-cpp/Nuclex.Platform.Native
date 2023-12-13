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

    /// <summary>Erases all occurences of the specified substring</summary>
    /// <param name="master">String from which substrings will be erased</param>
    /// <param name="victim">Substring that will be erased from the master string</param>
    public: static void EraseSubstrings(std::string &master, const std::string &victim) {
      for(;;) {
        std::string::size_type rIndex = master.find(victim);
        if(rIndex == std::string::npos) {
          break;
        }

        master.erase(rIndex, victim.size());
      }
    }

    /// <summary>Collapses all whitespaces between words to one space each</summary>
    /// <param name="target">String in which duplicate whitespace will be collapsed</param>
    public: static inline void EraseDuplicateWhitespace(std::string &target);

    /// <summary>Locates the next floating-point number within a string</summary>
    /// <param name="text">Text in which the next floating-point numbers will be found</param>
    /// <param name="startIndex">Index from which onwards to look for a number</param>
    /// <returns>The first number found in the string or an empty string_view</returns>
    public: static std::string_view FindNextFloat(
      const std::string_view &text, std::string::size_type startIndex = 0
    );

  };

  // ------------------------------------------------------------------------------------------- //

  inline void StringHelper::EraseDuplicateWhitespace(std::string &target) {
    std::string::size_type length = target.length();

    std::string::size_type toIndex = 0;
    {
      bool previousWasWhitespace = true; // effectively trims whitespace at the beginning

      for(std::string::size_type fromIndex = 0; fromIndex < length; ++fromIndex) {
        bool isWhitespace = Nuclex::Support::Text::ParserHelper::IsWhitespace(
          static_cast<std::uint8_t>(target[fromIndex])
        );
        if(isWhitespace) {
          if(!previousWasWhitespace) {
            previousWasWhitespace = true;
            target[toIndex] = u8' ';
            ++toIndex;
          }
        } else {
          previousWasWhitespace = false;
          target[toIndex] = target[fromIndex];
          ++toIndex;
        }
      }

      // If the string ended in a whitespace, take that off again
      if(previousWasWhitespace) {
        if(toIndex >= 1) {
          --toIndex;
        }
      }
    }

    target.resize(toIndex);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_STRINGHELPER_H
