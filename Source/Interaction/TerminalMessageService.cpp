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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "Nuclex/Platform/Interaction/TerminalMessageService.h"
#include <Nuclex/Support/Text/UnicodeHelper.h>
#include <Nuclex/Support/Text/ParserHelper.h>

#if 0
#if defined(NUCLEX_PLATFORM_WINDOWS)
#error Fuck Windows
#else
#include <unistd.h> // for read(), write(), STDOUT_FILENO, STDIN_FILENO
#endif

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  TerminalMessageService::TerminalMessageService() :
    doManualWordWrapping(false),
    useWindowsLineBreaks(true),
    buffer() {


  }

  // ------------------------------------------------------------------------------------------- //

  void TerminalMessageService::EnableManualWordWrapping(bool enable /* = true */) {
    this->doManualWordWrapping = enable;
  }

  // ------------------------------------------------------------------------------------------- //

  void TerminalMessageService::EnableWindowsLineBreaks(bool enable /* = true */) {
    this->useWindowsLineBreaks = enable;
  }

  // ------------------------------------------------------------------------------------------- //

  void TerminalMessageService::Inform(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
#if defined(NUCLEX_PLATFORM_WINDOWS)
#error Fuck Windows
#else
    //
    // (i) Title
    //     Message

#endif
  }

  // ------------------------------------------------------------------------------------------- //

  void TerminalMessageService::Warn(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    //
    // (!) Title
    //     Message

  }

  // ------------------------------------------------------------------------------------------- //

  void TerminalMessageService::Complain(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    //
    // (X) Title
    //     Message

  }

  // ------------------------------------------------------------------------------------------- //

  bool TerminalMessageService::AskYesNo(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    //
    // (?) Title
    //     Message
    //     [Y]es [N]o
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  bool TerminalMessageService::AskOkCancel(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    //
    // (?) Title
    //     Message
    //     [O]kay [C]ancel
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<bool> TerminalMessageService::AskYesNoCancel(
    const std::string &topic, const std::string &heading, const std::string &message
  ) {
    //
    // (?) Title
    //     Message
    //     [Y]es [N]o [C]ancel
    return std::optional<bool>();
  }

  // ------------------------------------------------------------------------------------------- //

  void TerminalMessageService::appendLineWrapped(
    const std::string &text, std::size_t lineWidth, std::size_t indent /* = 0 */
  ) {
    using Nuclex::Support::Text::UnicodeHelper;
    using Nuclex::Support::Text::ParserHelper;

    // For our purposes, we assume the buffer already starts out with the amount of
    // characters the text block should be indented by (it will contain the severity prefix),
    // so that we only need to indent after we generate a line break.
    std::size_t currentLineLength = indent;

    // No step through the text, one UTF-8 code point at a time, tracking where the most
    // recent word ended in order to intelligently break the lines
    auto current = reinterpret_cast<const UnicodeHelper::char8_t *>(text.c_str());
    const UnicodeHelper::char8_t *end = current + text.length();


    const UnicodeHelper::char8_t *lineStart = current;
    const UnicodeHelper::char8_t *potentialLineEnd = lineStart;
    bool previousWasWhitespace = false;
    while(current < end) {
      char32_t codePoint = UnicodeHelper::ReadCodePoint(current, end);

      // If the current character is a whitespace, remember this as the most recent
      // location for a line break
      if(ParserHelper::IsWhitespace(codePoint)) {
        potentialLineEnd = current;
        previousWasWhitespace = true;

        // Skip any whitespaces that follow. There is nothing we do with successive
        // whitespaces, so we can use a simplified inner loop to get past them quickly
        while(current < end) {
          codePoint = UnicodeHelper::ReadCodePoint(current, end);
          if(!ParserHelper::IsWhitespace(codePoint)) {
            break;
          }
        }
      }

      


    }


  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

/*
#if defined(NUCLEX_PLATFORM_WINDOWS)
    static const std::string lineBreak(u8"\r\n", 2);
#else // Anything other than Windows
    static const std::string lineBreak(u8"\n", 1);
#endif
*/
#endif