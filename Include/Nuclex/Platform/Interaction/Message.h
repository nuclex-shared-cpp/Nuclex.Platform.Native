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
