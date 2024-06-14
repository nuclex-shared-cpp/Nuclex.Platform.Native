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

#ifndef NUCLEX_PLATFORM_INTERACTION_ACTIVEWINDOWTRACKER_H
#define NUCLEX_PLATFORM_INTERACTION_ACTIVEWINDOWTRACKER_H

#include "Nuclex/Platform/Config.h"

#include <any> // for std;:any

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Allows the current main window or top-level dialog to be looked up</summary>
  class NUCLEX_PLATFORM_TYPE ActiveWindowTracker {

    /// <summary>Frees all resources owned by the instance</summary>
    public: NUCLEX_PLATFORM_API ~ActiveWindowTracker() = default;

    /// <summary>Looks up the currently active window</summary>
    /// <returns>
    ///   The native window handle of the currently active main window or top-level window.
    ///   This handle should be the of native type, i.e. an HWND on Windows Desktop systems
    ///   or (GtkWindow *) when GTK is used for the UI.
    /// </returns>
    public: virtual std::any GetActiveWindow() const = 0;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // NUCLEX_PLATFORM_INTERACTION_ACTIVEWINDOWTRACKER_H
