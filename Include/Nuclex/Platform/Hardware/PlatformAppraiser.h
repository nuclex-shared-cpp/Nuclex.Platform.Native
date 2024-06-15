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

#ifndef NUCLEX_PLATFORM_HARDWARE_PLATFORMAPPRAISER_H
#define NUCLEX_PLATFORM_HARDWARE_PLATFORMAPPRAISER_H

#include "Nuclex/Platform/Config.h"

#include <memory> // for std::shared_ptr
#include <string> // for std::string
#include <vector> // for std::vector
#include <future> // for std::future

#include "Nuclex/Platform/Hardware/CpuInfo.h"
#include "Nuclex/Platform/Hardware/MemoryInfo.h"
#include "Nuclex/Platform/Hardware/GpuInfo.h"
#include "Nuclex/Platform/Hardware/StoreInfo.h"

// PlatformAnalyzer
//   -> I wouldn't think of a hardware inventory querying system reading this name
//
// SystemAppraiser
//   -> Sounds a bit too much like something from a video game
//
// SystemAssessor
//   -> Yeah, but 'assessor' sounds like 'accessor' and it's an uncommon term :-/
//
// All of the above, with typedefs!
//   -> Woohoo!

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class CancellationWatcher;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Hardware {
  void removeTrailingSlash(std::wstring& volumeName);

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Provides hardware metrics used to optimally schedule tasks and estimate durations
  /// </summary>
  /// <remarks>
  ///   Some of the image processors are GPU-based, so the application needs to know if
  ///   Vulkan and/or CUDA are available. CPU-based processing stages are scheduled to make
  ///   optimal use of CPU cores (i.e. put more resources towards a slow stage rather than
  ///   just letting all stages burn as much CPU as they want).
  /// </remarks>
  class NUCLEX_PLATFORM_TYPE PlatformAppraiser {

    /// <summary>Analyzes the CPUs installed in the system</summary>
    /// <param name="canceller">
    ///   Allows cancellation of the data collection process before it is finished
    /// </param>
    /// <returns>
    ///   An <see cref="std::future" /> that will provide a description of
    ///   the CPU topology when the detection has completed
    /// </returns>
    public: NUCLEX_PLATFORM_API static std::future<std::vector<CpuInfo>> AnalyzeCpuTopology(
      const std::shared_ptr<const Tasks::CancellationWatcher> &canceller = (
        std::shared_ptr<const Tasks::CancellationWatcher>()
      )
    );

    /// <summary>Analyzes the installed and available memory in the system</summary>
    /// <returns>
    ///   An <see cref="std::future" /> that will provide a description of
    ///   the installed and available memory in the system
    /// </returns>
    public: NUCLEX_PLATFORM_API static std::future<MemoryInfo> AnalyzeMemory(
      const std::shared_ptr<const Tasks::CancellationWatcher> &canceller = (
        std::shared_ptr<const Tasks::CancellationWatcher>()
      )
    );

    /// <summary>Analyzes the installed and mounted storage volumes in the system</summary>
    /// <returns>
    ///   An <see cref="std::future" /> that will provide a description of
    ///   the installed and mounted storage volumes in the system
    /// </returns>
    public: NUCLEX_PLATFORM_API static std::future<
      std::vector<StoreInfo>
    > AnalyzeStorageVolumes(
      const std::shared_ptr<const Tasks::CancellationWatcher> &canceller = (
        std::shared_ptr<const Tasks::CancellationWatcher>()
      )
    );

    /// <summary>Runs in a thread to analyze the system's CPU topology</summary>
    /// <param name="canceller">Allows the information collection to be cancelled</param>
    /// <returns>A description of the system's CPU topology</returns>
    private: static std::vector<CpuInfo> analyzeCpuTopologyAsync(
      std::shared_ptr<const Tasks::CancellationWatcher> canceller
    );

    /// <summary>Runs in a thread to analyze the system's memory</summary>
    /// <param name="canceller">Allows the information collection to be cancelled</param>
    /// <returns>A description of the system's memory</returns>
    private: static MemoryInfo analyzeMemoryAsync(
      std::shared_ptr<const Tasks::CancellationWatcher> canceller
    );

    /// <summary>Runs in a thread to analyze th system's storage volumes</summary>
    /// <param name="canceller">Allows the information collection to be cancelled</param>
    /// <returns>A description of the system's storage volumes</returns>
    private: static std::vector<StoreInfo> analyzeStorageVolumesAsync(
      std::shared_ptr<const Tasks::CancellationWatcher> canceller
    );

    // --------------------------------
    // old from Videl
    // --------------------------------

#if 0
    /// <summary>Re-queries the hardware metrics</summary>
    public: void Refresh();

    /// <summary>Provides a short string summarizing the system's installed CPUs</summary>
    /// <returns>A short description of the CPUs in the system</returns>
    /// <remarks>
    ///   This is a simple string that can be displayed in the UI. It could be something
    ///   like &quot;AMD Ryzen ThreadRipper&quot; or &quot;2x Intel Xeon E5-2680&quot;.
    /// </remarks>
    public: std::string DescribeCpuModels() const { return this->cpuNames; }

    /// <summary>Provides a short string describing cores and frequency of the CPUs</summary>
    /// <returns>A short description of the power of the CPUs in the system</returns>
    /// <remarks>
    ///   This is a simple string that can be displayed in the UI. It could be something
    ///   like &quot;32x 3.8 GHz (+HT)&quot; or &quot;16x 3.5 GHz&quot;.
    /// </remarks>
    public: std::string DescribeCpuPower() const { return this->cpuPower; }

    /// <summary>Returns the amount of usable system memory in bytes</summary>
    /// <returns>The amount of usable system memory in bytes</returns>
    /// <remarks>
    ///   This will be a little less than actually installed in the system, subtracting
    ///   a reasonable amount of space for use by the operating system which would otherwise
    ///   be forced to use the page file if the applications makes full use of all memory.
    /// </remarks>
    public: std::size_t CountUsableMemoryBytes() const { return this->memoryUsableBytes; }

    /// <summary>Provivdes a shot string describing the amount of installed memory</summary>
    /// <returns>A short description of the memory installed in the system</returns>
    /// <remarks>
    ///   This is a simple string intended to display in the UI. It could be something
    ///   like &quot;64 GiB&quot;.
    /// </remarks>
    public: std::string DescribeInstalledMemory() const { return this->memoryInstalled; }

    /// <summary>Retrieves a list of GPUs present in the system</summary>
    /// <returns>A list of GPUs in the system with their specifications</returns>
    public: const std::vector<GpuInfo> &GetGpuTopology() const {
      return this->gpuTopology;
    }

    /// <summary>
    ///   Returns the highest amount of video memory installed on any of the system's GPUs
    /// </summary>
    /// <returns>The highest amount of video memory that can be used on the system</returns>
    public: std::size_t GetMaximumVideoMemory() const {
      return this->gpuMaximumVideoMemoryBytes;
    }

    /// <summary>Provides a short string summarizing the system's installed GPUs</summary>
    /// <returns>A short description of the GPUs in the system</returns>
    /// <remarks>
    ///   This is a simple string that can be displayed in the UI. It could be something
    ///   like &quot;NVidia RTX 3090&quot; or &quot;AMD Radeon RX 6900 XT&quot;.
    /// </remarks>
    public: std::string DescribeGpuModels() const {
      return this->gpuNames;
    }

    /// <summary>Queries the operating system for installed and available memory</summary>
    private: void queryMemoryInformation();

    /// <summary>Queries the operating system about the installed CPUs</summary>
    private: void queryCpuInformation();

    /// <summary>Forms some human-readable strings describing the installed CPUs</summary>
    private: void formCpuDescriptions();

    /// <summary>Queries the system for GPUs supporting Vulkan Compute</summary>
    private: void queryVulkanGpuInformation();

    /// <Summary>Forms some human-readable strings describing the installed GPUs</summary>
    private: void formGpuDescriptions();

    /// <summary>Topology of CPUs in the system</summary>
    private: std::vector<CpuInfo> cpuTopology;
    /// <summary>Number of CPU cores in the system (without HyperThreading)</summary>
    private: std::size_t cpuCoreCount;
    /// <summary>Number of usable processors in the system</summary>
    private: std::size_t cpuProcessorCount;
    /// <summary>Short string describing the make and model of the CPU(s)</summary>
    private: std::string cpuNames;
    /// <summary>Short string describing the power of the CPU(s)</summary>
    private: std::string cpuPower;

    /// <summary>Usable amount of system memory in bytes</summary>
    private: std::size_t memoryUsableBytes;
    /// <summary>Short string describing the installed memory</summary>
    private: std::string memoryInstalled;

    /// <summary>Topology of GPUs in the system</summary>
    private: std::vector<GpuInfo> gpuTopology;
    /// <summary>Highest amount of video memory present on any of the GPUs</summary>
    private: std::size_t gpuMaximumVideoMemoryBytes;
    /// <summary>Short string describing the make and model of the GPU(s)</summary>
    private: std::string gpuNames;
#endif

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_PLATFORMAPPRAISER_H
