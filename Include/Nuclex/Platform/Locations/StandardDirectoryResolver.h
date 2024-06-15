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

#ifndef NUCLEX_PLATFORM_LOCATIONS_STANDARDDIRECTORYRESOLVER_H
#define NUCLEX_PLATFORM_LOCATIONS_STANDARDDIRECTORYRESOLVER_H

#include "Nuclex/Platform/Config.h"

#include <string> // for std::string
#include <optional> // for std::optional
#include <atomic> // for std::atomic
#include <mutex> // for std::mutex

// Rename this to simply "StandardDirectoryLookup"?

// * Support looking up locations
//   * Install folder (/opt, /usr/local/bin or C:\Program Files)
//   * Data folder (different on Linux, same on Windows)
//   * Savegame folder
//   * Per-user settings folder
//   * Per-machine settings folder
//   * Temp folder
//
// * Support locating the user's audio library?
//   * Standard 'Music' folder from xuserdirs and Microsoft's shit
//   * Defined music libraries from popular audio players if retrievable

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Looks up and reports common directories based on a platform's conventients and
  ///   any configuration that needs to be honored
  /// </summary>
  /// <remarks>
  ///   On Windows, this looks up the known folders via its classic Windows API interface
  ///   and, for extended folders, via COM. On Linux, this follows the XDG specification
  ///   which defines standard user directories and methods to override them via environment
  ///   variable and the <code>user-dirs.dirs</code> file.
  /// </remarks>
  class NUCLEX_PLATFORM_TYPE StandardDirectoryResolver {

    /// <summary>Initializes a new common directory resolver</summary>
    /// <param name="linuxStyleApplicationDirectoryName">
    ///   Name of the application subdirectory on Linux, should be all-lowercase and
    ///   either a single word or as few words as possible separated by dashes.
    /// </param>
    /// <param name="windowsStyleApplicationDirectoryName">
    ///   Name of the application subdirectory on Windows, should be in title case
    ///   and use spaces inbetween words.
    /// </param>
    /// <remarks>
    ///   This constructor is provided for your convenience, so you can keep macros
    ///   selecting between Windows/Linux code paths to a minimum in your code.
    /// </remarks>
    public: NUCLEX_PLATFORM_API StandardDirectoryResolver(
      const std::string &linuxStyleApplicationDirectoryName,
      const std::string &windowsStyleApplicationDirectoryName
    ) :
#if defined(NUCLEX_PLATFORM_WINDOWS)
      StandardDirectoryResolver(windowsStyleApplicationDirectoryName) {
      (void)linuxStyleApplicationDirectoryName; // prevent unused variable warning
    }
#else
      StandardDirectoryResolver(linuxStyleApplicationDirectoryName) {
      (void)windowsStyleApplicationDirectoryName; // prevent unused variable warning
    }
#endif

    /// <summary>Initializes a new common directory resolver</summary>
    /// <param name="applicationName">
    ///   Name of the application subdirectory. If specified, it will be injected into
    ///   any paths returned where appropriate.
    /// </param>
    /// <remarks>
    ///   It is a good idea to also follow a platform's unwritten conventions. For Windows
    ///   builds, passing a human-readable name (starting words with uppercase letters and
    ///   using spaces if needed) is advices. For Linux builds, using an all-lowercase name
    ///   without spaces will make it fit in better (though neither platform has any firm
    ///   rules requiring this).
    /// </remarks>
    public: NUCLEX_PLATFORM_API StandardDirectoryResolver(
      const std::string &applicationName = std::string()
    );

    /// <summary>Frees all resources owned by the directory resolver</summary>
    public: NUCLEX_PLATFORM_API ~StandardDirectoryResolver();

    /// <summary>Returns the directory your executable is running from</summary>
    /// <returns>The directory in which the running executable is stored</returns>
    /// <remarks>
    ///   <para>
    ///     On Windows systems, it has become the convention to also store static data files,
    ///     such as images used in the UI, documentation ("help files") or a game's data files
    ///     containing all its audio, textures and levels. 
    ///   </para>
    ///   <para>
    ///     Please use the directory returned by <see cref="GetStaticDataDirectory" /> for
    ///     such files instead.
    ///   </para>
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetExecutableDirectory() const;
    
    /// <summary>Returns the directory your application's static data is stored in</summary>
    /// <returns>The directory in which static application data is stored</returns>
    /// <remarks>
    ///   Whereas this directory will be the same as the executable directory on Windows
    ///   systems, on Linux it will point either to <code>/opt/application-name</code> or,
    ///   when installed by a package manager that treats it as a standard application,
    ///   to <code>/usr/share/application-name</code>.
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetStaticDataDirectory() const;

    /// <summary>Retrieves the settings directory for user-specific settings</summary>
    /// <returns>The directory in which user-specific settings can be stored</returns>
    /// <remarks>
    ///   <para>
    ///     Use this to store user settings. Ideally, make it settings that only apply
    ///     to the user, but not the machine the application is running on. That might be
    ///     custom keyboard shortcuts, selected theme, favorites or turned-off notifications.
    ///  </para>
    ///  <para>
    ///    Things stored in this directory could theoretically be moved to another system
    ///    or downloaded from a server when the user logs in. Windows users call this
    ///    &quot;roaming&quot; data.
    ///  </para>
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetSettingsDirectory() const;

    /// <summary>Retrieves the state directory retaining the application's state</summary>
    /// <returns>The directory in which the application's state can be saved</returns>
    /// <remarks>
    ///   This directory is intended to store things that are either trivial (such as window
    ///   layouts, most recently opened files and so on) and things that can not be ported
    ///   to other systems, such as a list of running or monitored jobs on the system, but
    ///   also the preferred graphics adapter or machine-specific performance adjustments.
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetStateDirectory() const;

    /// <summary>Retrieves the cache directory for semi-temporary data</summary>
    /// <returns>The directory in which the application can cache things</returns>
    /// <remarks>
    ///   This directory is intended for files you would prefer to keep around, but which
    ///   can be trivially regenerated by your application should they disappear. Windows
    ///   applications generally just use the local appdata directory, but Linux systems
    ///   reserve /var/cache/my-application/ for such cases. Cached data may be thumbnails
    ///   or movie files, shaders specifically compiled for the installed GPU brand or
    ///   an index database for files managed by your application.
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetCacheDirectory() const;

    /// <summary>Retrieves the documents directory of the current user</summary>
    /// <returns>The directory in which created documents should be saved</returns>
    /// <remarks>
    ///   Documents are typically any files explicitly created by the user through a
    ///   &quot;save&quot; dialog and that they may wish to share with others or copy
    ///   around to other systems, TVs or phones.
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetDocumentsDirectory() const;

    /// <summary>Retrieves the directory in which video games should store save states</summary>
    /// <returns>The directory in which video games should store save states</returns>
    /// <remarks>
    ///   Recent Windows versions introduced a special <code>Saved Games</code> directory
    ///   for this purpose, whereas it has become semi-established practices to create
    ///   a <code>Saves Games\My Game</code> directory in the user's <code>Documents</code>
    ///   folder. Linux doesn't have an equivalent directory, but the best equivalent location
    ///   is <code>~/.local/share/my-game/saves</code> (it's state data, but it's important
    ///   to the user and should move with them to the next system, so it belongs in
    ///   <code>~/.local/share</code>).
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetSavedGameDirectory() const;

    /// <summary>Retrieves the system's temporary directory</summary>
    /// <returns>The directory in which temporary files can be stored</returns>
    /// <remarks>
    ///   This is for short-lived files your application may have to store for various reaons,
    ///   such as when it has to invoke an external tool than only knows how to handle files.
    ///   Windows systems usually have no automated that clean up the temp directory (the
    ///   manually launched &quot;Disk Cleanup&quot; does it). On Linux systems, the contents
    ///   of the temp directory generally vanish after a reboot, commonly because a ramdisk
    ///   is used.
    /// </remarks>
    public: NUCLEX_PLATFORM_API std::string GetTemporaryDirectory() const;

    // GetMediaDirectory()
    //   MediaType::Music directory
    //   MediaType::Pictures directory
    //   MediaType::Videos directory
    // GetSaveGameDirectory()
    // GetDocumentsDirectory()

    /// <summary>Platform-specific code to locate the executable directory</summary>
    /// <returns>The full path to the directory holding the running executable</returns>
    private: static std::string locateExecutableDirectory();

    /// <summary>Platform-specific code to locate the static data directory</summary>
    /// <returns>The full path to the directory holding static application data</returns>
    private: static std::string locateStaticDataDirectory();

    /// <summary>Platform-specific code to locate the settings directory</summary>
    /// <returns>The full path to the directory holding application settings</returns>
    private: static std::string locateSettingsDirectory();

    /// <summary>Platform-specific code to locate the state directory</summary>
    /// <returns>The full path to the directory holding the application's state</returns>
    private: static std::string locateStateDirectory();

    /// <summary>Name of the application (directory) in the file system</summary>
    private: std::string applicationName;

    /// <summary>Can be held by the methods as directories are figured out</summary>
    private: mutable std::mutex updateMutex;

    /// <summary>Whether the executable directory has been determined yet</summary>
    private: mutable std::atomic<bool> executableDirectoryKnown;
    /// <summary>The path in which the running executable resides</summary>
    private: mutable std::string executableDirectory;
    /// <summary>Whether the static data directory has been determined yet</summary>
    private: mutable std::atomic<bool> staticDataDirectoryKnown;
    /// <summary>Path to the application's static data directory</summary>
    private: mutable std::string staticDataDirectory;
    /// <summary>Whether the settings directory has been determined yet</summary>
    private: mutable std::atomic<bool> settingsDirectoryKnown;
    /// <summary>Path to the settings directory</summary>
    private: mutable std::string settingsDirectory;
    /// <summary>Whether the state directory has been determined yet</summary>
    private: mutable std::atomic<bool> stateDirectoryKnown;
    /// <summary>Path to the state directory</summary>
    private: mutable std::string stateDirectory;

  }; 

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Locations

#endif // NUCLEX_PLATFORM_LOCATIONS_STANDARDDIRECTORYRESOLVER_H
