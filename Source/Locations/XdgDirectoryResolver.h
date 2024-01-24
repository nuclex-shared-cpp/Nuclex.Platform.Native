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

#ifndef NUCLEX_PLATFORM_LOCATIONS_XDGDIRECTORYRESOLVER_H
#define NUCLEX_PLATFORM_LOCATIONS_XDGDIRECTORYRESOLVER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include <string>

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Resolves standard paths for a user according to the xdg specification</summary>
  /// <remarks>
  ///   <para>
  ///     The xdg specification defines a set of directories and the data they should be
  ///     used for as well as a mechanism (a configuration file) for users to customize
  ///     these directories. It has been adopted by all Linux distributions I'm aware of.
  ///   </para>
  ///   <para>
  ///     If you're a Windows developer, this is similar to Microsoft's &quot;known folders&quot;
  ///     and even defines several directories with identical purposes.
  ///   </para>
  /// </remarks>
  class XdgDirectoryResolver {

    /// <summary>Looks up the user's home directory</summary>
    /// <returns>The home directory of the user running the application</returns>
    /// <remarks>
    ///   No data should be stored directory under the user's home directory,
    ///   but this is a good starting point for accessing user-specific configuration files.
    /// </remarks>
    public: static std::string GetHomeDirectory();

    /// <summary>Looks up the directory for user-specific configuration files</summary>
    /// <returns>The directory to use for user-specific configuration files</returns>
    /// <remarks>
    ///   This usually points to $HOME/.config/ and has been widely adopted. Any user-specific
    ///   configuration data is stored inside this folder. The established convention is to
    ///   create one subdirectory for each application (i.e. $HOME/.config/awesome-game/).
    /// </remarks>
    public: static std::string GetConfigHomeDirectory();

    /// <summary>Looks up the directory for user-specific data</summary>
    /// <returns>The directory to use for user-specific data</returns>
    /// <remarks>
    ///   <para>
    ///     This usually points to $HOME/.local/share/ and should be used to store
    ///     user-specific data (anything that's not pure configuration - screenshots,
    ///     user-created maps, etc.). The established convention is to create one
    ///     subdirectory for each application (i.e. $HOME/.local/share/awesome-game/).
    ///   </para>
    ///   <para>
    ///     If a user moved to a different system (or had a Windows-style roaming profile
    ///     downloaded upon login from a server), this would be a directory that would be
    ///     moved to the new system (whereas its sibling, the 'state' directory, woudln't).
    ///   </para>
    /// </remarks>
    public: static std::string GetDataHomeDirectory();

    /// <summary>Looks up the directory for persistent application states</summary>
    /// <returns>The directory to use for application state data</returns>
    /// <remarks>
    ///   <para>
    ///     This usually points to $HOME/.local/state/ and should be used to store data
    ///     by which an application remembers its state - a list of recently used files,
    ///     view layout, undo history or such. The established convention is to create
    ///     one subdirectory for each application (i.e. $HOME/.local/share/awesome-game/).
    ///   </para>
    ///   <para>
    ///     As per the XDG Base Directory Specification, this should be used for state
    ///     data that is just for convenience or data that is non-portable. Compare this
    ///     with the <see cref="GetDatahomeDirectory" /> method where information that
    ///     is considered permanent and portable.
    ///   </para>
    /// </remarks>
    public: static std::string GetStateHomeDirectory();

    /// <summary>Looks up the directory for cached, non-essential files</summary>
    /// <returns>The directory to use for cached, non-essential files</returns>
    /// <remarks>
    ///   This usually points to $HOME/.cache/. It can be used to store intermediate
    ///   (and recreatable) files that should - maybe - stay around across multiple
    ///   starts of your appliation (think compiled shaders or indexes that the game
    ///   can recreate when needed)
    /// </remarks>
    public: static std::string GetCacheHomeDirectory();

    /// <summary>
    ///   Looks up a user-specific directory using the behavior described in
    ///   the xdg specification
    /// <summary>
    /// <param name="name">Name of the user-specific directory</param>
    /// <param name="path">String that will receive the looked-up path</param>
    /// <returns>
    ///   True if the path was successfully looked up, false if the path was not
    ///   defined for the user and the <see cref="path" /> parameter is unchanged
    /// <returns>
    /// <remarks>
    ///   Name should be the full variable name, i.e. 'XDG_CUSTOM_DIR', not just 'CUSTOM'.
    ///   The resulting string can contain '$HOME' to reference to the home directory.
    ///   This is the only substitution supported by xdg.
    /// </remarks>
    public: static bool GetUserDirectory(const std::string &name, std::string &path);

    /// <summary>Reads a path assignment from the 'user-dirs.dirs' file</summary>
    /// <param name="name">Name of the path assignment that will be read</param>
    /// <param name="path">Receives the path if an assignment could be found</param>
    /// <returns>
    ///   True if the path was successfully looked up, false if the path had no
    ///   assignment and the <see cref="path" /> parameter is unchanged
    /// <returns>
    private: static bool readUserDirectoryAssignment(const std::string &name, std::string &path);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Locations

#endif // defined(NUCLEX_PLATFORM_LINUX)

#endif // NUCLEX_PLATFORM_LOCATIONS_XDGDIRECTORYRESOLVER_H
