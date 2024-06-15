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