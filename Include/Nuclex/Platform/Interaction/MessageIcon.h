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

#ifndef NUCLEX_PLATFORM_INTERACTION_MESSAGEICON_H
#define NUCLEX_PLATFORM_INTERACTION_MESSAGEICON_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std;:vector

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Icons that can be shown in a message box</summary>
  /// <remarks>
  ///   This is mainly used with the MessageService to hint at the icon that should be used
  ///   to illustrate the severity of the displayed message with an OS-dependant image.
  ///   It may not lead to an image being displayed at all.
  /// </remarks>
  enum class MessageIcon {

    /// <summary>Do not display any image if possible</summary>
    None = 0,

    /// <summary>Display the image that indicates an information message</summary>
    Information,

    /// <summary>Display the image that indicates a warning message</summary>
    Warning,

    /// <summary>Display the image that indicates an error message</summary>
    Error,

    /// <summary>Display the image that indicates a question for the user</summary>
    Question

  };

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_MESSAGEICON_H
