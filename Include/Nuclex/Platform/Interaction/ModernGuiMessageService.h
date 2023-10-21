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

#ifndef NUCLEX_PLATFORM_INTERACTION_MODERNGUIMESSAGESERVICE_H
#define NUCLEX_PLATFORM_INTERACTION_MODERNGUIMESSAGESERVICE_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Interaction/ExtendedMessageService.h"

#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  class ActiveWindowTracker;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Shows notifications and questions to be displayed to an interactive user</summary>
  class NUCLEX_PLATFORM_TYPE ModernGuiMessageService : ExtendedMessageService {

    /// <summary>Initializes a new GUI-based message service</summary>
    /// <param name="activeWindowTracker">
    ///   Used to obtain the active top-level window that should become the owner of any
    ///   message boxes that are displayed.
    /// </param>
    public: NUCLEX_PLATFORM_API ModernGuiMessageService(
      const std::shared_ptr<ActiveWindowTracker> &activeWindowTracker = (
        std::shared_ptr<ActiveWindowTracker>()
      )
    );

    /// <summary>Frees all resources owned by the instance</summary>
    public: NUCLEX_PLATFORM_API ~ModernGuiMessageService() override = default;

    /// <summary>Displays a notification containing information to the user</summary>
    /// <param name="topic">Topic of the information (normally used as the window title)</param>
    /// <param name="heading">Headline of the message (normally written in bold)</param>
    /// <param name="message">Message text containing detailed informations</param>
    public: NUCLEX_PLATFORM_API void Inform(
      const std::string &topic, const std::string &heading, const std::string &message
    ) override;

    /// <summary>Displays a warning to the user</summary>
    /// <param name="topic">Topic of the warning (normally used as the window title)</param>
    /// <param name="heading">Headline of the message (normally written in bold)</param>
    /// <param name="message">Message text containing more details</param>
    public: NUCLEX_PLATFORM_API void Warn(
      const std::string &topic, const std::string &heading, const std::string &message
    ) override;

    /// <summary>Displays an error message to the user</summary>
    /// <param name="topic">Topic of the error (normally used as the window title)</param>
    /// <param name="heading">Headline of the message (normally written in bold)</param>
    /// <param name="message">Message text containing more details</param>
    public: NUCLEX_PLATFORM_API void Complain(
      const std::string &topic, const std::string &heading, const std::string &message
    ) override;

    /// <summary>Displays a question to the user, answerable with either yes or no</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating in the question and actions</param>
    /// <returns>True if the user answered yes, false if the user answered no</returns>
    public: NUCLEX_PLATFORM_API bool AskYesNo(
      const std::string &topic, const std::string &heading, const std::string &message
    ) override;

    /// <summary>
    ///   Displays a confirmation prompt to the user, answerable with either ok or cancel
    /// </summary>
    /// <param name="topic">Topic of the conformation (normally used as the window title)</param>
    /// <param name="heading">What the user has to confirm (normally written in bold)</param>
    /// <param name="message">Message text elaborating the action to be confirmed</param>
    /// <returns>True if the user answered ok, false if the user answered cancel</returns>
    public: NUCLEX_PLATFORM_API bool AskOkCancel(
      const std::string &topic, const std::string &heading, const std::string &message
    ) override;

    /// <summary>Displays a question to the user, answerable with yes, no or cancel</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating in the question and actions</param>
    /// <returns>
    ///   True if the user answered yes, false if the user answered no and nothing if
    ///   the user wishes to cancel
    /// </returns>
    public: NUCLEX_PLATFORM_API std::optional<bool> AskYesNoCancel(
      const std::string &topic, const std::string &heading, const std::string &message
    ) override;

    /// <summary>Requires the user to choose between a set of options</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating the question and choices</param>
    /// <param name="choices">Choices that are available for the user</param>
    /// <returns>The index of the choice that the user has made</returns>
    public: NUCLEX_PLATFORM_API std::optional<std::size_t> GiveChoices(
      const std::string &topic, const std::string &heading, const std::string &message,
      const std::initializer_list<std::string> &choices
    ) override;

    /// <summary>Requests confirmation from the user for a dangerous action</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating what will happen</param>
    /// <param name="choices">Choices that are available for the user</param>
    /// <returns>True if the user gave confirmation, false otherwise</returns>
    public: NUCLEX_PLATFORM_API bool RequestConfirmation(
      const std::string &topic, const std::string &heading, const std::string &message,
      std::chrono::milliseconds buttonEnableDelay = std::chrono::milliseconds(2000)
    ) override;

    /// <summary>Offers the user a chance to cancel an action for a limited time</summary>
    /// <param name="topic">Topic of the action (normally used as the window title)</param>
    /// <param name="heading">Thing that can be cancelled (normally written in bold)</param>
    /// <param name="message">Message text elaborating what will happen</param>
    /// <param name="autoAcceptDelay">
    ///   Time after which the dialog will automatically be confirmed
    /// </param>
    /// <returns>True if the user gave confirmation, false otherwise</returns>
    public: NUCLEX_PLATFORM_API bool OfferCancellation(
      const std::string &topic, const std::string &heading, const std::string &message,
      std::chrono::milliseconds autoAcceptDelay = std::chrono::milliseconds(5000)
    ) override;

    /// <summary>Provides the currently active top-level window</summary>
    private: std::shared_ptr<ActiveWindowTracker> activeWindowTracker;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_MODERNGUIMESSAGESERVICE_H
