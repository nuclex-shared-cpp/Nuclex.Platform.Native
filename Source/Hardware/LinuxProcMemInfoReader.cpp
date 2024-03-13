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

#include "./LinuxProcMemInfoReader.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast
#include <Nuclex/Support/Text/ParserHelper.h> // for ParserHelper

#include "../Platform/LinuxFileApi.h" // for LinuxFileApi::ReadFileIntoMemory()

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Parses the number of megabytes from a memory amount listed in /proc/meminfo
  /// </summary>
  /// <param name="memoryValue">Memory value as found in the /proc/meminfo file</param>
  /// <returns>The amount of megabytes the memory value indicates</returns>
  double megabytesFromMemoryValue(const std::string_view &memoryValue) {
    using Nuclex::Support::Text::lexical_cast;

    std::string::size_type index = memoryValue.find(u8"kB");
    if(index != std::string::npos) {
      return (lexical_cast<std::size_t>(std::string(memoryValue)) + 512) / 1024;
    }

    index = memoryValue.find(u8"mB");
    if(index != std::string::npos) {
      return lexical_cast<std::size_t>(std::string(memoryValue));
    }

    index = memoryValue.find(u8"gB");
    if(index != std::string::npos) {
      return lexical_cast<std::size_t>(std::string(memoryValue)) * 1024;
    }

    return 0.0; // We don't know the unit
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Collects and summarizes informations about the system's memory</summary>
  /// <remarks>
  ///   This helper gets fed each line read from /proc/meminfo and extracts useful
  ///   information that can be queried after all lines have been processed.
  /// </remarks>
  class MemInfoCollector {

    /// <summary>Initializes a new CPU info collector</summary>
    public: MemInfoCollector() :
      TotalMegabytes(0.0),
      FreeMegabytes(0.0),
      AvailableMegabytes(0.0) {}

    /// <summary>Processes one line read from the /proc/meminfo file</summary>
    /// <param name="line">Line that will be processed</param>
    /// <remarks>
    ///   Call this method for each line encountered. The information collector will
    ///   attempt to detect the relevant memory information and be as flexible
    ///   parsing the format as the standard allows.
    /// </remarks>
    public: void ProcessLine(const std::string_view &line);

    /// <summary>Evaluates a single key/value describing some property</summary>
    /// <param name="key">Key identifying the property</param>
    /// <param name="value">Reported value of the property</param>
    private: void processKeyValuePair(
      const std::string_view &key, const std::string_view &value
    );

    /// <summary>Human-readable amount of memory installed</summary>
    public: std::size_t TotalMegabytes;
    /// <summary>Human-readable amount of memory that can be allocated from</summary>
    public: std::size_t FreeMegabytes;
    /// <summary>Human-readable amount of memory that remains unused</summary>
    public: std::size_t AvailableMegabytes;

  };

  // ------------------------------------------------------------------------------------------- //

  void MemInfoCollector::ProcessLine(const std::string_view &line) {
    using Nuclex::Support::Text::ParserHelper;

    // Look for a double colon in this line. If one is found, it's a key/value pair.
    std::string_view::size_type doubleColonIndex = line.find(':');
    if(doubleColonIndex != std::string_view::npos) {

      // Trim any whitespace between the property name and the double colon
      // Linux kernels generally pad the lines with a tab character and have
      // the double colons float like a dividing line at the same column.
      std::string_view::size_type keyEnd = doubleColonIndex;
      while(keyEnd > 0) {
        if(ParserHelper::IsWhitespace(line[keyEnd - 1])) {
          --keyEnd;
        } else {
          break;
        }
      }

      // Trim any whitespace between the double colon and where the value starts
      std::string_view::size_type valueStart = doubleColonIndex + 1;
      while(valueStart < line.length()) {
        if(ParserHelper::IsWhitespace(line[valueStart])) {
          ++valueStart;
        } else {
          break;
        }
      }

      // We've got cleanly trimmed key and value strings. Check if this is
      // a property we're interested in and if so, process it.
      processKeyValuePair(line.substr(0, keyEnd), line.substr(valueStart));

    }
  }

  // ------------------------------------------------------------------------------------------- //

  void MemInfoCollector::processKeyValuePair(
    const std::string_view &key, const std::string_view &value
  ) {
    using Nuclex::Support::Text::lexical_cast;

    if(key == u8"MemTotal") {
      this->TotalMegabytes = megabytesFromMemoryValue(value);
    } else if(key == u8"MemFree") {
      this->FreeMegabytes = megabytesFromMemoryValue(value);
    } else if(key == u8"MemAvailable") {
      this->AvailableMegabytes = megabytesFromMemoryValue(value);
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  MemoryInfo LinuxProcMemInfoReader::TryReadMemInfo(
    const std::shared_ptr<const Tasks::CancellationWatcher> &canceller
  ) {
    MemInfoCollector collector;
    {
      std::vector<std::uint8_t> memInfoContents = (
        Platform::LinuxFileApi::ReadFileIntoMemory(u8"/proc/meminfo")
      );
      const char *memInfoText = reinterpret_cast<const char *>(memInfoContents.data());

      canceller->ThrowIfCanceled();

      // Walk through the file contents, looking for '\n' character to cookie-cut
      // each line as a string_view and process it with the CPU information collection
      std::size_t lineStartIndex = 0;
      std::size_t index = 1;
      while(index < memInfoContents.size()) {
        if(memInfoText[index] == '\n') {
          collector.ProcessLine(
            std::string_view(memInfoText + lineStartIndex, index - lineStartIndex)
          );
          lineStartIndex = index + 1;
        }

        ++index;
      }

      // If the last line didn't end with a newline character, make sure it's processed
      if(index > lineStartIndex + 1) {
        collector.ProcessLine(
          std::string_view(memInfoText + lineStartIndex, index - lineStartIndex)
        );
      }
    }

    // The available process address space on Linux differs between 32-bit and 64-bit processes.
    // Perhaps there is a way to query the kernel for it (I bet it's tweakable), but it's of
    // fading importance (64-bit).
    MemoryInfo result;
    result.InstalledMegabytes = collector.TotalMegabytes;
    if constexpr(sizeof(std::uintptr_t) == 4) {
      result.MaximumProgramMegabytes = std::min(
        std::size_t(3 * 1024), result.InstalledMegabytes
      );
    } else {
      result.MaximumProgramMegabytes = std::min(
        std::size_t(0xffff8800000), result.InstalledMegabytes
      );
    }
      
    return result;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_LINUX)