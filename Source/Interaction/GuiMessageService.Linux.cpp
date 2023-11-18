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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "Nuclex/Platform/Interaction/GuiMessageService.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include "Nuclex/Platform/Interaction/ActiveWindowTracker.h"

#include "../Platform/GtkDialogApi.h"
#include "../Platform/KdeWidgetsAddonsApi.h"

namespace {

  // ------------------------------------------------------------------------------------------- //

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  struct GuiMessageService::PrivateImplementationData {

    public: void *kdeWidgetsAddonsLibraryHandle;
    public: void *gtkLibraryHandle;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction


namespace Nuclex { namespace Platform { namespace Interaction {

  // ------------------------------------------------------------------------------------------- //

  GuiMessageService::GuiMessageService(
    const std::shared_ptr<ActiveWindowTracker> &activeWindowTracker /* = (
      std::shared_ptr<ActiveWindowTracker>()
    ) */
  ) :
    activeWindowTracker(activeWindowTracker),
    implementationData(new PrivateImplementationData()) {
  }

  // ------------------------------------------------------------------------------------------- //

  GuiMessageService::~GuiMessageService() {
    if(static_cast<bool>(this->implementationData)) {
      if(this->implementationData->gtkLibraryHandle) {
        Platform::GtkDialogApi::UnloadLibrary(
          this->implementationData->gtkLibraryHandle, false
        );
      }
      if(this->implementationData->kdeWidgetsAddonsLibraryHandle) {
        Platform::GtkDialogApi::UnloadLibrary(
          this->implementationData->kdeWidgetsAddonsLibraryHandle, false
        );
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void GuiMessageService::Inform(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    void *gtkLibraryHandle = Platform::GtkDialogApi::TryLoadLibrary();
    void *kdeWidgetsAddonsLibraryHandle = Platform::KdeWidgetsAddonsApi::TryLoadLibrary();
    
  }

  // ------------------------------------------------------------------------------------------- //

  void GuiMessageService::Warn(
    const std::string &topic, const std::string &, const std::string &message
  ) {
  }

  // ------------------------------------------------------------------------------------------- //

  void GuiMessageService::Complain(
    const std::string &topic, const std::string &, const std::string &message
  ) {
  }

  // ------------------------------------------------------------------------------------------- //

  bool GuiMessageService::AskYesNo(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  bool GuiMessageService::AskOkCancel(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<bool> GuiMessageService::AskYesNoCancel(
    const std::string &topic, const std::string &, const std::string &message
  ) {
    return std::optional<bool>();
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Interaction

#endif // defined(NUCLEX_PLATFORM_WINDOWS)