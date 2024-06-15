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
