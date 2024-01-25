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

#include <stdexcept> // for runtime_error

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
    std::string target(256, '\0');

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
        Nuclex::Platform::Platform::PosixApi::ThrowExceptionForSystemError(
          u8"Could not follow '/proc/self/exe' to own path", originalErrorNumber
        );
      }
    }

    return target;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //

  StandardDirectoryResolver::StandardDirectoryResolver(
    const std::string &applicationName /* = std::string() */
  ) :
    applicationName(applicationName) {}

  // ------------------------------------------------------------------------------------------- //

  StandardDirectoryResolver::~StandardDirectoryResolver() {
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetExecutableDirectory() const {
    std::string executablePath = getExecutablePath();

    // Remove the filename fro mthe path
    std::string::size_type lastBackslashIndex = executablePath.find_last_of('/');
    if(lastBackslashIndex != std::string::npos) {
      executablePath.resize(lastBackslashIndex + 1); // Keep the slash on
    }

    return executablePath;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetStaticDataDirectory() const {
    XdgDirectoryResolver resolver;

    // Perhaps this method needs to return a path list to fully honor the XDG spec?

    // TODO: Check if application name subdirectory is present in XDG_DATA_HOME
    // TODO: Check if executables redies in /opt/ and if so, use that path
    // TODO: Check if executable is in /usr/local/bin, if so, use /usr/local/share
    // TODO: Check if executable is in /usr/bin, if so, use /usr/share
    return resolver.GetDataHomeDirectory();
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetSettingsDirectory() const {
    XdgDirectoryResolver resolver;
    return resolver.GetConfigHomeDirectory();
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetStateDirectory() const {
    XdgDirectoryResolver resolver;
    return resolver.GetStateHomeDirectory();
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetCacheDirectory() const {
    XdgDirectoryResolver resolver;
    return resolver.GetCacheHomeDirectory();
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetDocumentsDirectory() const {
    throw std::runtime_error(u8"Not implemented yet");
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetSavedGameDirectory() const {
    throw std::runtime_error(u8"Not implemented yet");
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetTemporaryDirectory() const {
    throw std::runtime_error(u8"Not implemented yet");
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Locations

#endif // defined(NUCLEX_PLATFORM_LINUX)
