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
    typedef Nuclex::Support::Text::UnicodeHelper::Char8Type Char8Type;
    char32_t codepoint;

    // Hunt for the first non-whitespace character
    const Char8Type *current = reinterpret_cast<const Char8Type *>(text.data()) + startIndex;
    const Char8Type *end = current + text.length();
    const Char8Type *start;
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
    startIndex = start - reinterpret_cast<const Char8Type *>(text.data());
    std::size_t length = current - start;
    return text.substr(startIndex, length - 1);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware