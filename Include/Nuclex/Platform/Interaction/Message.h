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

#ifndef NUCLEX_PLATFORM_INTERACTION_MESSAGE_H
#define NUCLEX_PLATFORM_INTERACTION_MESSAGE_H

#include "Nuclex/Platform/Config.h"

#include <string> // for std::string

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Message or question that can be displayed to the user</summary>
  class NUCLEX_PLATFORM_TYPE Message {

    /// <summary>Initializes a new message</summary>
    /// <param name="caption">Caption of the message, intended as the window title</param>
    /// <param name="heading">
    ///   Headline for the message, stating the message or question as a single sentence,
    ///   intended to be displayed at the top of a notification box
    /// </param>
    /// <param name="details">
    ///   Optional details, describing what happened or the consequences of a choice made
    ///   by the user if the message is a question
    /// </param>
    public: NUCLEX_PLATFORM_API Message(
      const std::string &caption,
      const std::string &heading,
      const std::string &details = std::string()
    ) :
      Caption(caption),
      Heading(heading),
      Details(details) {}

    /// <summary>Caption of the message, intended as the window title</summary>
    public: std::string Caption;

    /// <summary>
    ///   Headline for the message, stating the message or question as a single sentence,
    ///   intended to be displayed at the top of a notification box
    /// </summary>
    public: std::string Heading;

    /// <summary>
    ///   Optional details, describing what happened or the consequences of a choice made
    ///   by the user if the message is a question
    /// </summary>
    public: std::string Details;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_MESSAGE_H
