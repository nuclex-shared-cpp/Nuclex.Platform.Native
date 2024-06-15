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

#ifndef NUCLEX_PLATFORM_HARDWARE_GPUINFO_H
#define NUCLEX_PLATFORM_HARDWARE_GPUINFO_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std;:vector

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a GPU installed in the system</summary>
  class NUCLEX_PLATFORM_TYPE GpuInfo {

    /// <summary>Name of the company that manufactured the GPU</summary>
    /// <remarks>
    ///   This will simply be &quot;NVidia&quot;, &quot;AMD&quot; or &quot;Intel&quot;.
    /// </remarks>
    public: std::string ManufacturerName;

    // Could attempt to provide the board manufacturer's name, too. It should be stored
    // in the GPU's specialized BIOS since manufacturers use different maximum power ratings
    // (and these depend on board design) and which CUDA/compute cores are enabled also is
    // part of this BIOS settings block, as is the company name (i.e. "Asus" or "MSI")
    //

    /// <summary>Human-readable model name of this GPU</summary>
    public: std::string ModelName;

    /// <summary>Version of the driver by which this GPU is interfaced with</summary>
    public: std::string DriverVersion;

    /// <summary>Total amount of video memory installed on this GPU in megabytes</summary>
    public: std::size_t VideoMemoryInMegabytes;

    /// <summary>Maximum pixel shader version this GPU can handle</summary>
    public: std::size_t PixelShaderVersion;

    /// <summary>Maximum vertex shader version this GPU can handle</summary>
    public: std::size_t VertexShaderVersion;

    /// <summary>Whether this GPU is a dedicated add-on board (vs. CPU-integrated)</summary>
    public: bool IsDedicated;

    /// <summary>Whether this GPU supports Vulkan</summary>
    public: bool SupportsVulkan;

  };

  // ------------------------------------------------------------------------------------------- //

  // QueryGpuTemperature() as separate method

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_GPUINFO_H
