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
