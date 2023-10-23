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

#ifndef NUCLEX_PLATFORM_PLATFORM_GTKDIALOGAPI_H
#define NUCLEX_PLATFORM_PLATFORM_GTKDIALOGAPI_H

#include "Nuclex/Platform/Config.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Offers generic methods for dealing with the GTK dialogs API</summary>
  class GtkDialogApi {

    public: static bool TryLoadGtk();

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_PLATFORM_GTKDIALOGAPI_H
