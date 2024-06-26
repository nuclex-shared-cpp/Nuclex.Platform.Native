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

#include "XdgDirectoryResolver.h"

// This code is loosely a C++ reimplementation of the code found in
// xdg-user-dir-lookup.c, which is published under the MIT license.

#if defined(NUCLEX_PLATFORM_LINUX)

#include "../Platform/LinuxEnvironmentApi.h"
#include "../Platform/LinuxFileApi.h"

#include <unistd.h> // for getuid()
#include <pwd.h> // for getpwuid() / to get home directory from passwd file as a fallback

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Determines the location of the user-dirs.dirs file and reads it</summary>
  /// <param name="contents">String that will receive the entire file contents</param>
  /// <returns>
  ///   True if the file was read in its entirety, false if anything went wrong
  /// </returns>
  bool LocateAndReadEntireXdgUserDirsFile(std::string &contents) {
    using Nuclex::Platform::Platform::LinuxFileApi;

    const static std::string userDirectorySettingsFilename(u8"user-dirs.dirs", 14);

    // The 'user-dirs.dirs' file is only allowed to be in 'XDG_CONFIG_HOME', but not
    // in the 'XDG_CONFIG_DIRS' according to the spec and the xdg-user-dirs program.
    std::string userDirectorySettingsFilePath = LinuxFileApi::JoinPaths(
      Nuclex::Platform::Locations::XdgDirectoryResolver::GetHomeDirectory(),
      userDirectorySettingsFilename
    );

    // Read the contents of the 'user-dirs.dirs' file in one go
    return LinuxFileApi::TryReadFileInOneReadCall(
      userDirectorySettingsFilePath, contents
    );    
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Expands the '$HOME' placeholder in a string to the user's home directory
  /// </summary>
  /// <param name="path">Path in which the '$HOME' placeholder will be expanded</param>
  void ExpandHomePlaceholder(std::string &path) {
    static const std::string homePlaceholder(u8"$HOME", 5);

    std::string::size_type homeDirectoryIndex = path.find(homePlaceholder);
    if(homeDirectoryIndex != std::string::npos) {
      path.replace(
        homeDirectoryIndex, homePlaceholder.length(),
        Nuclex::Platform::Locations::XdgDirectoryResolver::GetHomeDirectory()
      );
    }
  }


  // ------------------------------------------------------------------------------------------- //

  /// <summary>Extracts the next line from a string containing multiple lines</summary>
  /// <param name="fileContents">String holding multiple lines</param>
  /// <param name="startOffset">
  ///   Index at which the line begins, will be updated to the next line's start index
  /// </param>
  /// <param name="line">String into which the line will be copied</param>
  /// <returns>
  ///   True if the next line has been copied into the string, false if no more lines
  ///   are left to process
  /// </returns>
  bool GetNextLine(
    const std::string &fileContents, std::string::size_type &startOffset, std::string &line
  ) {
    std::string::size_type contentsLength = fileContents.length();
    if(startOffset >= fileContents.length()) {
      return false;
    }

    std::string::size_type nextLineBreakIndex = fileContents.find('\n', startOffset);

    // If no newline character was found (but there were also bytes left to process,
    // see check above), the file ends without a newline, so rest is the last line
    if(nextLineBreakIndex == std::string::npos) {
      line.assign(fileContents, startOffset);
      startOffset = contentsLength;
      return true;
    }

    // Copy the whole line into the provided buffer.
    // If there's a DOS line break, also remove the carriage return.
    if(fileContents[nextLineBreakIndex - 1] == '\r') {
      std::size_t lineLength = nextLineBreakIndex - startOffset - 1;
      line.assign(fileContents, startOffset, lineLength);
    } else {
      std::size_t lineLength = nextLineBreakIndex - startOffset;
      line.assign(fileContents, startOffset, lineLength);
    }

    startOffset = nextLineBreakIndex + 1;
    return true;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Checks if the specified character is a whitespace character</summary>
  /// <param name="character">Character that will be checked</param>
  /// <returns>True if the specified character is a whitespace</returns>
  bool IsWhitespace(char character) {
    return (
      (character == ' ') ||
      (character == '\t') ||
      (character == '\r') ||
      (character == '\n')
    );
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Takes a substring of the specified string and trims it</summary>
  /// <param name="text">Text from which the substring will be taken</param>
  /// <param name="start">Index of the first character to include in the substring</param>
  /// <param name="length">Length of the substring in characters</param>
  /// <param name="result">String in which the trimmed substring will be stored</param>
  void GetTrimmedSubstring(
    const std::string &text,
    std::string::size_type start, std::string::size_type length,
    std::string &result
  ) {
    std::string::size_type firstCharacterIndex = std::string::npos;

    // Scan for the first character that's not a whitespace
    for(std::string::size_type index = 0; index < length; ++index) {
      if(!IsWhitespace(text[index + start])) {
        firstCharacterIndex = start + index;
        break;
      }
    }

    // If the whole string was whitespace, return an empty string
    if(firstCharacterIndex == std::string::npos) {
      result.clear();
      return;
    }

    // If this point is reached, we know there's at least one non-whitespace character,
    // so we can scan backwards without worrying about the case of not finding anything
    std::string::size_type lastCharacterIndex = start + length - 1;
    while(lastCharacterIndex > firstCharacterIndex) {
      if(!IsWhitespace(text[lastCharacterIndex])) {
        break;
      }
      --lastCharacterIndex;
    }

    result.assign(text, firstCharacterIndex, lastCharacterIndex - firstCharacterIndex + 1);
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Parses the right side of a variable assignment</summary>
  /// <param name="text">Text containing the variable assignment</param>
  /// <param name="start">Index at which the assigned value begins</param>
  /// <param name="value">String that will receive the assigned value</param>
  void GetAssignedValue(
    const std::string &text, std::string::size_type start, std::string &value
  ) {
    std::string::size_type textLength = text.length();

    // Skip whitespace at the beginning
    while(start < textLength) {
      if(!IsWhitespace(text[start])) {
        break;
      }
      ++start;
    }

    // Did the right side of the assignment contain nothing or only whitespace?
    if(start >= textLength) {
      value.clear();
    } else { // No, there are characters being assigned

      // Is it a quoted string?
      if(text[start] == '"') {
        ++start;

        // Scan for the closing quote, ignoring escaped quotes
        bool isEscaped = false;
        std::string::size_type end = start;
        while(end < textLength) {
          if(text[end] == '\\') {
            isEscaped = !isEscaped;
          } else if(!isEscaped) {
            if(text[end] == '"') {
              break;
            }
          } else {
            isEscaped = false;
          }
          ++end;
        }

        // If no closing quote was found, just use the rest of the string
        if(end >= textLength) {
          value.assign(text, start, textLength - 1); // no trimmming
        } else { // Otherwise, use the text insie the quotes
          value.assign(text, start, end - start); // no trimmming
        }

      } else { // Unquoted value

        // Use everything up to the first comment marker (if any)
        std::string::size_type commentStartIndex = text.find('#', start);
        if(commentStartIndex == std::string::npos) {
          GetTrimmedSubstring(text, start, textLength - 1, value);
        } else {
          GetTrimmedSubstring(text, start, commentStartIndex - start, value);
        }

      } // if quoted string

    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //
  
  XdgDirectoryResolver::XdgDirectoryResolver(
    GetEnvironmentVariableMethod *getEnvironmentVariable /* = nullptr */,
    ReadEntireXdgUserDirsFileMethod *readEntireXdgUserDirsFile /* = nullptr */
  ) :
    getEnvironmentVariable(getEnvironmentVariable),
    readEntireXdgUserDirsFile(readEntireXdgUserDirsFile) {

    if(getEnvironmentVariable == nullptr) {
      this->getEnvironmentVariable = &Platform::LinuxEnvironmentApi::GetEnvironmentVariable;
    }
    if(readEntireXdgUserDirsFile == nullptr) {
      this->readEntireXdgUserDirsFile = &LocateAndReadEntireXdgUserDirsFile;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::string XdgDirectoryResolver::GetHomeDirectory() {
    const static std::string homeEnvironmentVariable(u8"HOME", 4);

    // Check the 'HOME' environment variable first. This is how it's done by
    // the official xdg lookup implementation, too.
    std::string homeDirectory;
    bool found = Platform::LinuxEnvironmentApi::GetEnvironmentVariable(
      homeEnvironmentVariable, homeDirectory    // ^^ intentionally w/o function pointer
    );
    if(found && !homeDirectory.empty()) {
      return homeDirectory;
    }

    // Next, try getpwuid() to read the home directory from the passwd file
    struct passwd *password = ::getpwuid(::getuid());
    if(password != nullptr) {
      return password->pw_dir;
    }

    // If this doesn't work, something is seriously broken. As a last line of
    // defense, just use '~' and hope the file system functions figure it out.
    // (the tried and proven "let someone else take care of it" approach).
    return u8"~";
  }

  // ------------------------------------------------------------------------------------------- //

  std::string XdgDirectoryResolver::GetConfigHomeDirectory() {
    const static std::string xdgConfigHomeEnvironmentVariable(u8"XDG_CONFIG_HOME", 15);
    const static std::string defaultConfigHomeDirectory(u8".config", 7);

    // Check the 'XDG_CONFIG_HOME' environment variable first
    std::string configHomeDirectory;
    bool found = this->getEnvironmentVariable(
      xdgConfigHomeEnvironmentVariable, configHomeDirectory
    );
    if(found && !configHomeDirectory.empty()) {
      ExpandHomePlaceholder(configHomeDirectory);
      return configHomeDirectory;
    }

    // If the variable wasn't set, assume the path to be '~/.config',
    // just as the xdg specification requires.
    return Platform::LinuxFileApi::JoinPaths(GetHomeDirectory(), defaultConfigHomeDirectory);
  }

  // ------------------------------------------------------------------------------------------- //

  std::string XdgDirectoryResolver::GetDataHomeDirectory() {
    const static std::string xdgDataHomeVariable(u8"XDG_DATA_HOME", 13);
    const static std::string defaultDataHomeDirectory(u8".local/share", 12);

    // Check the 'XDG_DATA_HOME' environment variable first
    std::string dataHomeDirectory;
    bool found = this->getEnvironmentVariable(xdgDataHomeVariable, dataHomeDirectory);
    if(found && !dataHomeDirectory.empty()) {
      ExpandHomePlaceholder(dataHomeDirectory);
      return dataHomeDirectory;
    }

    // Then try the xdg user directory settings file
    found = GetUserDirectory(xdgDataHomeVariable, dataHomeDirectory);
    if(found && !dataHomeDirectory.empty()) {
      return dataHomeDirectory;
    }

    // If all xdg lookups failed, assume the path to be '~/.local/share',
    // just as the xdg specification requires.
    return Platform::LinuxFileApi::JoinPaths(GetHomeDirectory(), defaultDataHomeDirectory);
  }

  // ------------------------------------------------------------------------------------------- //

  std::string XdgDirectoryResolver::GetStateHomeDirectory() {
    const static std::string xdgStateHomeVariable(u8"XDG_STATE_HOME", 14);
    const static std::string defaultStateHomeDirectory(u8".local/state", 12);

    // Check the 'XDG_DATA_HOME' environment variable first
    std::string stateHomeDirectory;
    bool found = this->getEnvironmentVariable(xdgStateHomeVariable, stateHomeDirectory);
    if(found && !stateHomeDirectory.empty()) {
      ExpandHomePlaceholder(stateHomeDirectory);
      return stateHomeDirectory;
    }

    // Then try the xdg user directory settings file
    found = GetUserDirectory(xdgStateHomeVariable, stateHomeDirectory);
    if(found && !stateHomeDirectory.empty()) {
      return stateHomeDirectory;
    }

    // If all xdg lookups failed, assume the path to be '~/.local/share',
    // just as the xdg specification requires.
    return Platform::LinuxFileApi::JoinPaths(GetHomeDirectory(), defaultStateHomeDirectory);
  }

  // ------------------------------------------------------------------------------------------- //

  std::string XdgDirectoryResolver::GetCacheHomeDirectory() {
    const static std::string xdgCacheHomeVariable(u8"XDG_CACHE_HOME", 14);
    const static std::string defaultCacheHomeDirectory(u8".cache", 6);

    // Check the 'XDG_CACHE_HOME' environment variable first
    std::string cacheHomeDirectory;
    bool found = this->getEnvironmentVariable(xdgCacheHomeVariable, cacheHomeDirectory);
    if(found && !cacheHomeDirectory.empty()) {
      ExpandHomePlaceholder(cacheHomeDirectory);
      return cacheHomeDirectory;
    }

    // Then try the xdg user directory settings file
    found = GetUserDirectory(xdgCacheHomeVariable, cacheHomeDirectory);
    if(found && !cacheHomeDirectory.empty()) {
      return cacheHomeDirectory;
    }

    // If all xdg lookups failed, assume the path to be '~/.cache',
    // just as the xdg specification requires.
    return Platform::LinuxFileApi::JoinPaths(GetHomeDirectory(), defaultCacheHomeDirectory);
  }

  // ------------------------------------------------------------------------------------------- //

  bool XdgDirectoryResolver::GetUserDirectory(const std::string &name, std::string &path) {

    // Try to fetch the requested xdg directory assignment from the settings file
    if(readUserDirectoryAssignment(name, path)) {
      ExpandHomePlaceholder(path);
      return true;
    } else { // Assignment was not found
      return false;
    }

  }

  // ------------------------------------------------------------------------------------------- //

  bool XdgDirectoryResolver::readUserDirectoryAssignment(
    const std::string &name, std::string &path
  ) {

    // Read the contents of the 'user-dirs.dirs' file in one go
    std::string userDirectorySettings;
    bool wasSuccessfullyRead = this->readEntireXdgUserDirsFile(userDirectorySettings);
    if(unlikely(!wasSuccessfullyRead)) {
      return false;
    }

    // Go over the file line-by-line to look for the requested variable
    std::string line;
    std::string variableName;
    std::string::size_type lineStartOffset = 0;
    while(GetNextLine(userDirectorySettings, lineStartOffset, line)) {
      std::string::size_type assignmentIndex = line.find('=');
      if(assignmentIndex == std::string::npos) {
        continue;
      }

      // If the line contains a comment character and it sits before
      // the first assignment character, the whole line is invalid
      std::string::size_type commentStartIndex = line.find('#');
      if(commentStartIndex != std::string::npos) {
        if(commentStartIndex < assignmentIndex) {
          continue;
        }
      }

      // Is this the searched-for variable? If not, go to the next line
      GetTrimmedSubstring(line, 0, assignmentIndex, variableName);
      if(variableName != name) {
        continue;
      }

      // Variable name matches 
      GetAssignedValue(line, assignmentIndex + 1, path);
      return true;
    }

    // Ran out of lines without finding the requested variable
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Locations

#endif // defined(NUCLEX_PLATFORM_LINUX)
