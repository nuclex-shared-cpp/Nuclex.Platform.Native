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
#include <chrono> // for std::chrono::milliseconds

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
    public: virtual std::optional<std::size_t> GiveChoices(
      const std::string &topic, const std::string &heading, const std::string &message,
      const std::initializer_list<std::string> &choices
    ) = 0;

    /// <summary>Requests confirmation from the user for a dangerous action</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating what will happen</param>
    /// <param name="buttonEnableDelay">
    ///   Time for which the 'Ok' button will remain disabled after the dialog is displayed
    /// </param>
    /// <returns>True if the user gave confirmation, false otherwise</returns>
    public: virtual bool RequestConfirmation(
      const std::string &topic, const std::string &heading, const std::string &message,
      std::chrono::milliseconds buttonEnableDelay = std::chrono::milliseconds(2000)
    ) = 0;

    /// <summary>Offers the user a chance to cancel an action for a limited time</summary>
    /// <param name="topic">Topic of the action (normally used as the window title)</param>
    /// <param name="heading">Thing that can be cancelled (normally written in bold)</param>
    /// <param name="message">Message text elaborating what will happen</param>
    /// <param name="autoAcceptDelay">
    ///   Time after which the dialog will automatically be confirmed
    /// </param>
    /// <returns>True if the user gave confirmation, false otherwise</returns>
    public: virtual bool OfferCancellation(
      const std::string &topic, const std::string &heading, const std::string &message,
      std::chrono::milliseconds autoAcceptDelay = std::chrono::milliseconds(5000)
    ) = 0;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_MESSAGESERVICE_H

// Support native platform MessageBox
// Support task bar notification
// Support command line output