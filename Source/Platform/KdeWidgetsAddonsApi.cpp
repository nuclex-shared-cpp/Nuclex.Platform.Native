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

#include "KdeWidgetsAddonsApi.h"

#if !defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/Text/LexicalAppend.h>

#include "./PosixDynamicLibraryApi.h" // for DlOpen(), DlClose(), DlSym()
#include <dlfcn.h> // for RTLD_LAZY, RTLD_LOCAL

// https://api.kde.org/frameworks/kwidgetsaddons/html/kmessagebox_8h_source.html
// https://api.kde.org/legacy/4.14-api/kdelibs-apidocs/kdeui/html/kmessagebox_8h_source.html
// https://api.kde.org/legacy/2.0-api/classref/kdeui/kmessagebox_h.html

/*
class Q_CORE_EXPORT QObject {
  public: virtual QObject();
}
class Q_WIDGETS_EXPORT QWidget : public QObject, public QPaintDevice;
*/

#if 0
class QWidget;

/**
  * Displays an "Error" dialog with a "Details >>" button.
  *
  * @param parent  Parent widget.
  * @param text    Message string.
  * @param details Detailed message string.
  * @param title   Message box title. The application name is added to
  *                the title. The default title is i18n("Error").
  * @param options  see Options
  *
  * Your program messed up and now it's time to inform the user.
  * To be used for important things like "Sorry, I deleted your hard disk."
  *
  * The @p details message can contain additional information about
  * the problem and can be shown on request to advanced/interested users.
  *
  * The default button is "&OK". Pressing "Esc" selects the OK-button.
  *
  * @note The OK button will always have the i18n'ed text '&OK'.
  */
 typedef DetailedErrorFunction(
   QWidget *parent,
   const QString &text,
   const QString &details,
   const QString &title = QString(),
   Options options = Notify
);
#endif

namespace {

  // ------------------------------------------------------------------------------------------- //

  enum ButtonCode {
    Ok = 1,
    Cancel = 2,
    PrimaryAction = 3,
    SecondaryAction = 4,
    Continue = 5
  };  

  enum DialogType {
    QuestionTwoActions = 1,
    WarningTwoActions = 2,
    WarningContinueCancel = 3,
    WarningTwoActionsCancel = 4,
    Information = 5,
    Error = 8,
    QuestionTwoActionsCancel = 9
  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  void *KdeWidgetsAddonsApi::TryLoadLibrary() {
    void *kdeWidgetsAddonsLibraryHandle = PosixDynamicLibraryApi::TryDlOpen(
      u8"libKF5WidgetsAddons.so", RTLD_LAZY | RTLD_LOCAL
    );
    if(unlikely(kdeWidgetsAddonsLibraryHandle == nullptr)) {
      // TODO Try different KDE versions and or explicit paths?
      return nullptr;
    } else {
      return kdeWidgetsAddonsLibraryHandle;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void KdeWidgetsAddonsApi::UnloadLibrary(
    void *kdeWidgetsAddonsLibraryHandle, bool throwOnError /* = true */
  ) {
    PosixDynamicLibraryApi::DlClose(kdeWidgetsAddonsLibraryHandle, throwOnError);
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
