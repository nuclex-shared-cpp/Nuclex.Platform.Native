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

#ifndef NUCLEX_PLATFORM_INTERACTION_MESSAGESERVICE_H
#define NUCLEX_PLATFORM_INTERACTION_MESSAGESERVICE_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <optional> // for std;:optional

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Shows notifications and questions to be displayed to an interactive user</summary>
  class NUCLEX_PLATFORM_TYPE MessageService {

    /// <summary>Frees all resources owned by the instance</summary>
    public: NUCLEX_PLATFORM_API virtual ~MessageService() = default;

#if 0 // Requires library user to subclass message service implementation... good/bad?
    /// <summary>Whether the message service is interactive (versus unattended)</summary>
    /// <remarks>
    ///   Normally, applications making use of the message service are interactive ones,
    ///   meaning a human is inspection and confirming notifications. If your application can
    ///   also run unattended, for simple use cases you can use this method to skip messages
    ///   and use default behaviors where the user would ordinarily be asked questions.
    /// </remarks>
    public: virtual bool IsInteractive() const = 0;
#endif

    /// <summary>Displays a notification containing information to the user</summary>
    /// <param name="topic">Topic of the information (normally used as the window title)</param>
    /// <param name="heading">Headline of the message (normally written in bold)</param>
    /// <param name="message">Message text containing detailed informations</param>
    public: virtual void Inform(
      const std::string &topic, const std::string &heading, const std::string &message
    ) = 0;

    /// <summary>Displays a warning to the user</summary>
    /// <param name="topic">Topic of the warning (normally used as the window title)</param>
    /// <param name="heading">Headline of the message (normally written in bold)</param>
    /// <param name="message">Message text containing more details</param>
    public: virtual void Warn(
      const std::string &topic, const std::string &heading, const std::string &message
    ) = 0;

    /// <summary>Displays an error message to the user</summary>
    /// <param name="topic">Topic of the error (normally used as the window title)</param>
    /// <param name="heading">Headline of the message (normally written in bold)</param>
    /// <param name="message">Message text containing more details</param>
    public: virtual void Complain(
      const std::string &topic, const std::string &heading, const std::string &message
    ) = 0;

    /// <summary>Displays a question to the user, answerable with either yes or no</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message text elaborating in the question and actions</param>
    /// <returns>True if the user answered yes, false if the user answered no</returns>
    public: virtual bool AskYesNo(
      const std::string &topic, const std::string &heading, const std::string &message
    ) = 0;

    /// <summary>
    ///   Displays a confirmation prompt to the user, answerable with either ok or cancel
    /// </summary>
    /// <param name="topic">Topic of the conformation (normally used as the window title)</param>
    /// <param name="heading">What the user has to confirm (normally written in bold)</param>
    /// <param name="message">Message text elaborating the action to be confirmed</param>
    /// <returns>True if the user answered ok, false if the user answered cancel</returns>
    public: virtual bool AskOkCancel(
      const std::string &topic, const std::string &heading, const std::string &message
    ) = 0;

    /// <summary>Displays a question to the user, answerable with yes, no or cancel</summary>
    /// <param name="topic">Topic of the question (normally used as the window title)</param>
    /// <param name="heading">Basic question being asked (normally written in bold)</param>
    /// <param name="message">Message elaborating the question and possible actions</param>
    /// <returns>
    ///   True if the user answered yes, false if the user answered no and nothing if
    ///   the user wishes to cancel
    /// </returns>
    public: virtual std::optional<bool> AskYesNoCancel(
      const std::string &topic, const std::string &heading, const std::string &message
    ) = 0;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_MESSAGESERVICE_H

// Support native platform MessageBox
// Support task bar notification
// Support command line output