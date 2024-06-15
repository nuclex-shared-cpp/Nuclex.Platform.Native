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

#include "./LinuxProcCpuInfoReader.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast
#include <Nuclex/Support/Text/ParserHelper.h> // for ParserHelper

#include "../Platform/LinuxFileApi.h" // for LinuxFileApi::ReadFileIntoMemory()

#include "Nuclex/Platform/Tasks/CancellationWatcher.h" // for CancellationWatcher

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Collects and summarizes informations about the system's CPUs</summary>
  /// <remarks>
  ///   This helper gets fed each line read from /proc/cpuinfo and extracts useful
  ///   information that gets reported via a callback each time a processor block is complete.
  /// </remarks>
  class CpuInfoCollector {

    /// <summary>Initializes a new CPU info collector</summary>
    public: CpuInfoCollector(
      Nuclex::Platform::Hardware::LinuxProcCpuInfoReader::CallbackFunction *callback,
      void *userPointer
    ) :
      callback(callback),
      userPointer(userPointer),
      processorIndex(std::size_t(-1)),
      modelName(),
      physicalId(std::size_t(-1)),
      coreId(std::size_t(-1)),
      currentMhz(-1.0),
      bogoMips(-1.0) {}

    /// <summary>Processes one line read from the /proc/cpuinfo file</summary>
    /// <param name="line">Line that will be processed</param>
    /// <remarks>
    ///   Call this method for each line encountered. The information collector will
    ///   attempt to detect when a new processor paragraph starts and be as flexible
    ///   parsing the format as the standard allows.
    /// </remarks>
    public: void ProcessLine(const std::string_view &line);

    /// <summary>Must be called after a line has been processed</summary>
    /// <remarks>
    ///   Since this is an open-end line collector, the last listed processor will not
    ///   have been reported yet at the end. This method reports it.
    /// </remarks>
    public: void ReportProcessor();

    /// <summary>Evaluates a single key/value describing some property</summary>
    /// <param name="key">Key identifying the property</param>
    /// <param name="value">Reported value of the property</param>
    private: void processKeyValuePair(
      const std::string_view &key, const std::string_view &value
    );

    /// <summary>Resets the current processor informations to record a new one</summary>
    private: void startNewProcessor();

    /// <summary>Returns a normalized value for the CPU's frequency in GHz</summary>
    /// <param name="cpuName">CPU make and model string containing the frequency</param>
    /// <param name="maxMhzSeen">Live frequency used as an emergency fallback</param>
    /// <returns>The CPU frequency in plain GHz</returns>
    private: static double sanitizeCpuFrequency(
      const std::string_view &cpuName, double maxMhzSeen
    );

    /// <summary>Callback that will be invoked for each processor parsed</summary>
    private: Nuclex::Platform::Hardware::LinuxProcCpuInfoReader::CallbackFunction *callback;
    /// <summary>Pointer that will be handed, untouched, to the clalback function</summary>
    private: void *userPointer;

    /// <summary>Index of the current processor</summary>
    private: std::size_t processorIndex;
    /// <summary>Human-readable make and model of the current processor</summary>
    private: std::string modelName;
    /// <summary>Unique id of the physical CPU whose processor is being described</summary>
    private: std::size_t physicalId;
    /// <summary>Unique id of the CPU core currently being described</summary>
    private: std::size_t coreId;
    /// <summary>Encountered frequency of the current core in MHz</summary>
    private: double currentMhz;
    /// <summary>Rough benchmark value of the current processor</summary>
    private: double bogoMips;

  };

  // ------------------------------------------------------------------------------------------- //

  void CpuInfoCollector::ProcessLine(const std::string_view &line) {
    using Nuclex::Support::Text::ParserHelper;

    // Look for a double colon in this line. If one is found, it's a key/value pair.
    std::string_view::size_type doubleColonIndex = line.find(':');
    if(doubleColonIndex == std::string_view::npos) {
      ReportProcessor();
      startNewProcessor(); // No key/value pair - must be a blank line between paragraphs
    } else { // It's a key/value pair, process it as a property

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

  void CpuInfoCollector::ReportProcessor() {

    // Check if any information has been collected yet. This makes the method safe
    // for redundant calls which we allow.
    if(this->physicalId != std::size_t(-1)) {
      (*this->callback)(
        nullptr,
        this->processorIndex,
        this->physicalId,
        this->coreId,
        this->modelName,
        this->currentMhz,
        this->bogoMips
      );
    }

  }

  // ------------------------------------------------------------------------------------------- //

  void CpuInfoCollector::processKeyValuePair(
    const std::string_view &key, const std::string_view &value
  ) {
    using Nuclex::Support::Text::lexical_cast;

    if(key == u8"processor") {
      ReportProcessor();
      startNewProcessor();
      this->processorIndex = lexical_cast<std::size_t>(std::string(value));
    } else if(key == u8"model name") {
      this->modelName = value;
    } else if(key == u8"cpu MHz") {
      this->currentMhz = lexical_cast<double>(std::string(value));
    } else if(key == u8"cpu GHz") {
      this->currentMhz = lexical_cast<double>(std::string(value)) * 1000.0;
    } else if(key == u8"physical id") {
      this->physicalId = lexical_cast<std::size_t>(std::string(value));
    } else if(key == u8"core id") {
      this->coreId = lexical_cast<std::size_t>(std::string(value));
    } else if(key == u8"bogomips") {
      this->bogoMips = lexical_cast<double>(std::string(value));
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void CpuInfoCollector::startNewProcessor() {

    // Reset the running state so it can be filled from the next processor paragraph
    this->processorIndex = std::size_t(-1);
    this->modelName.clear();
    this->physicalId = std::size_t(-1);
    this->coreId = std::size_t(-1);
    this->currentMhz = -1.0;
    this->bogoMips = -1.0;

  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  void LinuxProcCpuInfoReader::TryReadCpuInfos(
    void *userPointer,
    CallbackFunction *callback,
    const std::shared_ptr<const Tasks::CancellationWatcher> &canceller
  ) {
    CpuInfoCollector collector(callback, userPointer);
    {
      std::vector<std::uint8_t> cpuInfoContents = (
        Platform::LinuxFileApi::ReadFileIntoMemory(u8"/proc/cpuinfo")
      );
      const char *cpuInfoText = reinterpret_cast<const char *>(cpuInfoContents.data());

      canceller->ThrowIfCanceled();

      // Walk through the file contents, looking for '\n' character to cookie-cut
      // each line as a string_view and process it with the CPU information collection
      std::size_t lineStartIndex = 0;
      std::size_t index = 1;
      while(index < cpuInfoContents.size()) {
        if(cpuInfoText[index] == '\n') {
          collector.ProcessLine(
            std::string_view(cpuInfoText + lineStartIndex, index - lineStartIndex)
          );
          lineStartIndex = index + 1;
        }

        ++index;
      }

      // If the last line didn't end with a newline character, make sure it's processed
      if(index > lineStartIndex + 1) {
        collector.ProcessLine(
          std::string_view(cpuInfoText + lineStartIndex, index - lineStartIndex)
        );
      }
    }

    // Report the last procesor recorded as well.
    collector.ReportProcessor();
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_LINUX)