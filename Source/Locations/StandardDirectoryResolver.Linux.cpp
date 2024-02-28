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

#include "Nuclex/Platform/Locations/StandardDirectoryResolver.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include <linux/limits.h> // for PATH_MAX
#include <unistd.h> // for getpid()

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append()

#include "XdgDirectoryResolver.h" // for XdgDirectoryResolver
#include "../Platform/PosixApi.h" // for ThrowExceptionForSystemError()
#include "../Platform/LinuxFileApi.h" // for ReadLink()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Determines the path of the process image file for the runnign application
  /// </summary>
  /// <param name="target">String that will receive the path of the executable</param>
  std::string getExecutablePath() {
    std::string target(256, '\0'); // 256 as initial size, but will be resized as needed

    std::string ownProcessLink;
    ownProcessLink.reserve(16);
    ownProcessLink.assign(u8"/proc/self/exe", 14);

    // Try to read the symlink to obtain the path to the running executable
    int originalErrorNumber = 0;
    bool wasSuccessfullyRead = Nuclex::Platform::Platform::LinuxFileApi::TryReadLink(
      ownProcessLink, target, &originalErrorNumber
    );
    if(unlikely(!wasSuccessfullyRead)) {

      // Make another attempt with the PID file accessed directly (no recursive symlink).
      {
        ownProcessLink.resize(6);

        ::pid_t ownPid = ::getpid();
        Nuclex::Support::Text::lexical_append(target, ownPid);

        ownProcessLink.append(u8"/exe", 4);
      }

      // Try to read the symlink to obtain the path to the running executable
      wasSuccessfullyRead = Nuclex::Platform::Platform::LinuxFileApi::TryReadLink(
        ownProcessLink, target
      );
      if(unlikely(!wasSuccessfullyRead)) {
        // Report the original error. The non-recursive symlink was a nice try,
        // but what we really want is for the vanilla, standard way to work.
        Nuclex::Platform::Platform::PosixApi::ThrowExceptionForSystemError(
          u8"Could not follow '/proc/self/exe' to own path", originalErrorNumber
        );
      }
    }

    return target;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Checks whether the a string begins with the specified substring</summary>
  /// <param name="haystack">String that will be checked</param>
  /// <param name="needle">Substring the other string needs to start with</param>
  /// <returns>True if the string starts with the specified substring, false otherwise</returns>
  bool stringStartsWith(const std::string &haystack, const std::string &needle) {
    std::string::size_type haystackLength = haystack.length();
    if(haystackLength == 0) {
      return needle.empty();
    }

    std::string::size_type needleLength = needle.length();
    if(haystackLength < needleLength) {
      return false;
    }

    for(std::string::size_type index = 0; index < needleLength; ++index) {
      if(haystack[index] != needle[index]) {
        return false;
      }
    }

    return true;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::locateExecutableDirectory() {
    std::string executablePath = getExecutablePath();

    // Remove the filename from the path so we're left with only the directory.
    // If there are no slashes in the path, we honestly don't know what's going on
    // and just leave the string unmodified (would an exception be less confusing?)
    std::string::size_type lastBackslashIndex = executablePath.find_last_of('/');
    if(lastBackslashIndex != std::string::npos) {
      executablePath.resize(lastBackslashIndex + 1); // Keep the slash on
    }

    return executablePath;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::locateStaticDataDirectory() {
    const static std::string usrLocalBinPath(u8"/usr/local/bin/", 15);
    const static std::string usrBinPath(u8"/usr/bin/", 9);

    std::string staticDataDirectory;
    {
      // Check where the executable resides, this will tell us where and how
      // the application is installed
      staticDataDirectory = getExecutablePath();
      if(stringStartsWith(staticDataDirectory, usrLocalBinPath)) {
        staticDataDirectory.assign(u8"/usr/local/share/", 17);
      } else if(stringStartsWith(staticDataDirectory, usrBinPath)) {
        staticDataDirectory.assign(u8"/usr/share/", 11);
      } else { // it's likely in /opt/ or a custom path
        std::string::size_type lastBackslashIndex = staticDataDirectory.find_last_of('/');
        if(lastBackslashIndex != std::string::npos) {
          staticDataDirectory.resize(lastBackslashIndex + 1); // Keep the slash on
        }
      }
    }

    return staticDataDirectory;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::locateSettingsDirectory() {
    XdgDirectoryResolver resolver;
    return resolver.GetConfigHomeDirectory();
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::locateStateDirectory() {
    XdgDirectoryResolver resolver;
    return resolver.GetStateHomeDirectory();
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Locations

#endif // defined(NUCLEX_PLATFORM_LINUX)
