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

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append()

#include "../Platform/WindowsApi.h" // for ThrowExceptionForSystemError()
#include "../Platform/WindowsProcessApi.h" // for GetModuleHandle(), GetModuleFileName()
#include "../Platform/WindowsShellApi.h" // for GetKnownFolderPath()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Determines the path of the process image file for the runnign application
  /// </summary>
  /// <param name="target">String that will receive the path of the executable</param>
  std::string getExecutablePath() {
    using Nuclex::Platform::Platform::WindowsProcessApi;

    ::HMODULE executableModuleHandle = WindowsProcessApi::GetModuleHandle();
    return WindowsProcessApi::GetModuleFileName(executableModuleHandle);
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
    std::string::size_type lastBackslashIndex = executablePath.find_last_of('\\');
    if(lastBackslashIndex != std::string::npos) {
      executablePath.resize(lastBackslashIndex + 1); // Keep the slash on
    }

    return executablePath;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::locateStaticDataDirectory() {

    // Windows applications, the standard way of shipping is to place all static
    // data inside the same directory as the executable
    return locateExecutableDirectory();

  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::locateSettingsDirectory() {

    //return Platform::WindowsShellApi::GetKnownFolderPath(
    //  KNOWNFOLDERID_
    //);

    throw std::runtime_error(u8"Not implemented yet");
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::locateStateDirectory() {
    throw std::runtime_error(u8"Not implemented yet");
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Locations

#endif // defined(NUCLEX_PLATFORM_LINUX)
