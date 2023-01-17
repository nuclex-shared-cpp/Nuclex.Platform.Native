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

#ifndef NUCLEX_PLATFORM_HARDWARE_GPUINFO_H
#define NUCLEX_PLATFORM_HARDWARE_GPUINFO_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std;:vector

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a GPU installed in the system</summary>
  class GpuInfo {

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
