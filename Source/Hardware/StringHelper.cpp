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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "./StringHelper.h"

#include <Nuclex/Support/Text/UnicodeHelper.h> // for UnicodeHelper

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Checks whether the specified codepoint is a number</summary>
  /// <param name="codepoint">Unicode codepoint that will be checked</param>
  /// <returns>True if the codepoint is a number, false otherwise</returns>
  bool isNumber(char32_t codepoint) {
    return (
      (codepoint >= U'0') &&
      (codepoint <= U'9')
    );
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  // TODO: This number finder does not recognize +/- prefixes and exponential notation
  std::string_view StringHelper::FindNextFloat(
    const std::string_view &text, std::string::size_type startIndex /* = 0 */
  ) {
    typedef Nuclex::Support::Text::UnicodeHelper::char8_t char8_t;
    char32_t codepoint;

    // Hunt for the first non-whitespace character
    const char8_t *current = reinterpret_cast<const char8_t *>(text.data()) + startIndex;
    const char8_t *end = current + text.length();
    const char8_t *start;
    for(;;) {
      if(current >= end) {
        return std::string_view();
      }

      start = current;

      codepoint = Nuclex::Support::Text::UnicodeHelper::ReadCodePoint(current, end);
      if(codepoint == char32_t(-1)) {
        return std::string_view(); // Invalid code point found, string is not processable
      }

      // Did we find the potential start of the number?
      if(!Nuclex::Support::Text::ParserHelper::IsWhitespace(codepoint)) {
        break;
      }
    }

    // The number may start with a dot (as in ".7GHz") or a number (as in "3.5GHz").
    // In case the first character is a dot, the next must be a number character.
    bool foundDot = (codepoint == U'.');
    if(foundDot) {
      if(current >= end) {
        return std::string_view();
      }

      codepoint = Nuclex::Support::Text::UnicodeHelper::ReadCodePoint(current, end);
      if(codepoint == char32_t(-1)) {
        return std::string_view(); // Invalid code point found, string is not processable
      }
    }
    if(!isNumber(codepoint)) {
      return std::string_view();
    }

    // We've got a valid number already, not scan for where it ends
    for(;;) {
      if(current >= end) {
        break;
      }

      codepoint = Nuclex::Support::Text::UnicodeHelper::ReadCodePoint(current, end);
      if(codepoint == char32_t(-1)) {
        return std::string_view(); // Invalid code point found, string is not processable
      }

      // If there's a dot, make sure it's not another dot (otherwise we consider everything
      // invalid -- it wasn't a number after all).
      if(codepoint == U'.') {
        if(foundDot) {
          return std::string_view(); // Invalid code point found, string is not processable
        } else {
          foundDot = true;
        }
      } else if(!isNumber(codepoint)) {
        break;
      }
    }

    // We have a number with start and end index, return a string_view for that segment
    startIndex = start - reinterpret_cast<const char8_t *>(text.data());
    std::size_t length = current - start;
    return text.substr(startIndex, length - 1);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware