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

#ifndef NUCLEX_PLATFORM_HARDWARE_MEMORYINFO_H
#define NUCLEX_PLATFORM_HARDWARE_MEMORYINFO_H

#include "Nuclex/Platform/Config.h"

#include <cstddef> // for std::size_t
#include <string> // for std::string
#include <vector> // for std;:vector

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Informations about a physical drive installed in the system</summary>
  class NUCLEX_PLATFORM_TYPE MemoryInfo {

    /// <summary>The amount of memory installed in in the system</summary>
    public: std::size_t InstalledMegabytes;

    /// <summary>The amount of memory a running program is allowed to use at most</summary>
    public: std::size_t MaximumProgramMegabytes;

  };

  // ------------------------------------------------------------------------------------------- //

  // QueryFreeMemory() as separate method (total free memory in system)
  // QueryUsableMemory() as separate method (total free memory in process)

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // NUCLEX_PLATFORM_HARDWARE_MEMORYINFO_H
