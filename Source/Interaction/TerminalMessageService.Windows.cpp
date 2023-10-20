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

#include "Nuclex/Platform/Interaction/TerminalMessageService.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "../Platform/WindowsConsoleApi.h"
#include "../Platform/WindowsFileApi.h"

namespace {

  // ------------------------------------------------------------------------------------------- //
  /*
  #if defined(NUCLEX_PLATFORM_WINDOWS)
  static const std::string lineBreak(u8"\r\n", 2);
  #else // Anything other than Windows
  static const std::string lineBreak(u8"\n", 1);
  #endif
  */
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  class TerminalMessageService::ImplementationData {

    /// <summary>Current file handle of the console screen buffer</summary>
    public: ::HANDLE ConsoleScreenBufferHandle;

    /// <summary>Informations about the console's current screen buffer</summary>
    public: ::CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;


  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  TerminalMessageService::TerminalMessageService() :
    doManualWordWrapping(false),
    useWindowsLineBreaks(true),
    implementationData(std::make_unique<ImplementationData>()) {

    this->implementationData->ConsoleScreenBufferHandle = (
      Platform::WindowsFileApi::OpenActiveConsoleScreenBuffer(false)
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TerminalMessageService::~TerminalMessageService() {
    Platform::WindowsFileApi::CloseFile(
      this->implementationData->ConsoleScreenBufferHandle, false
    );
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
    if(this->implementationData->ConsoleScreenBufferHandle != INVALID_HANDLE_VALUE) {
      if(this->doManualWordWrapping) {
        std::string terminalWidth;
        Platform::WindowsConsoleApi::GetConsoleScreenBufferInfo(
          this->implementationData->ConsoleScreenBufferHandle,
          this->implementationData->ConsoleScreenBufferInfo
        );
        terminalWidth = static_cast<std::size_t>(
          this->implementationData->ConsoleScreenBufferInfo.dwSize.X
        );

        // TODO: Print with wrapping

      } else {

        // TODO: Print without wrapping

      }
    }
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

}}} // namespace Nuclex::Platform::Interaction

#endif // defined(NUCLEX_PLATFORM_WINDOWS)