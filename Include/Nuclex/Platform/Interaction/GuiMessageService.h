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

#ifndef NUCLEX_PLATFORM_INTERACTION_GUIMESSAGESERVICE_H
#define NUCLEX_PLATFORM_INTERACTION_GUIMESSAGESERVICE_H

#include "Nuclex/Platform/Config.h"
#include "Nuclex/Platform/Interaction/MessageService.h"

#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  class ActiveWindowTracker;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Shows notifications and questions to be displayed to an interactive user</summary>
  class NUCLEX_PLATFORM_TYPE GuiMessageService : public MessageService {

    /// <summary>Initializes a new GUI-based message service</summary>
    /// <param name="activeWindowTracker">
    ///   Used to obtain the active top-level window that should become the owner of any
    ///   message boxes that are displayed.
    /// </param>
    public: NUCLEX_PLATFORM_API GuiMessageService(
      const std::shared_ptr<ActiveWindowTracker> &activeWindowTracker = (
        std::shared_ptr<ActiveWindowTracker>()
      )
    );

    /// <summary>Frees all resources owned by the GUI-based message service</summary>
    public: NUCLEX_PLATFORM_API ~GuiMessageService() override;

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

    /// <summary>Provides the currently active top-level window</summary>
    private: std::shared_ptr<ActiveWindowTracker> activeWindowTracker;

    // Stores private, portentially platform-specific implementation details of the class
    private: struct PrivateImplementationData;
    /// <summary>Holds private and platform-specific implementation data</summary>
    private: std::unique_ptr<PrivateImplementationData> implementationData;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_MESSAGESERVICE_H

// Support native platform MessageBox
// Support task bar notification
// Support command line output