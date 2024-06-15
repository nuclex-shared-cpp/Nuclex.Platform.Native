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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "Nuclex/Platform/Interaction/GuiMessageService.h"

#if defined(NUCLEX_PLATFORM_LINUX)

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

#endif // defined(NUCLEX_PLATFORM_LINUX)