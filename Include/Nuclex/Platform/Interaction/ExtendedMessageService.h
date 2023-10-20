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

#ifndef NUCLEX_PLATFORM_INTERACTION_EXTENDEDMESSAGESERVICE_H
#define NUCLEX_PLATFORM_INTERACTION_EXTENDEDMESSAGESERVICE_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Interaction/MessageService.h"

#include <initializer_list> // for std::initializer_list

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Allows multiple choice dialogs to be displayed for user interaction</summary>
  class NUCLEX_PLATFORM_TYPE ExtendedMessageService : MessageService {

    /// <summary>Frees all resources owned by the instance</summary>
    public: NUCLEX_PLATFORM_API virtual ~ExtendedMessageService() override = default;

    /// <summary>Requires the user to choose between a set of options</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating the question and choices</param>
    /// <param name="choices">Choices that are available for the user</param>
    /// <returns>The index of the choice that the user has made</returns>
    public: virtual std::size_t GiveChoices(
      const std::string &topic, const std::string &heading, const std::string &message,
      const std::initializer_list<std::string> &choices
    ) = 0;

    /// <summary>Requests confirmation from the user for a dangerous action</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating what will happen</param>
    /// <param name="choices">Choices that are available for the user</param>
    /// <returns>True if the user gave confirmation, false otherwise</returns>
    public: virtual bool RequestConfirmation(
      const std::string &topic, const std::string &heading, const std::string &message,
      int buttonEnableDelay = 0
    ) = 0;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_MESSAGESERVICE_H

// Support native platform MessageBox
// Support task bar notification
// Support command line output