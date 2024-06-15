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

#ifndef NUCLEX_PLATFORM_HARDWARE_WINDOWSREGISTRYCPUINFOREADER_H
#define NUCLEX_PLATFORM_HARDWARE_WINDOWSREGISTRYCPUINFOREADER_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include <string> // for std::string
#include <cstddef> // for std::size_t
#include <memory> // for std::shared_ptr

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  class CancellationWatcher;

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Reads processor names and base frequencies from the Windows registry</summary>
  /// <remarks>
  ///   <para>
  ///     The location this information is pulled from
  ///     (HKLM\HARDWARE\DESCRIPTION\System\CentralProcessor\<n>\) is not an officially
  ///     documented place to find it. So future Windows versions could theoretically
  ///     stop storing it there or place random dummy data in its place. Theoretically.
  ///   </para>
  ///   <para>
  ///     There are applications out in the wild that access this registry path, there are
  ///     StackOverflow posts recommending it and Wine/Proton provide the information here,
  ///     too, so practically, we're pretty safe. And just in case, we do paranoid checking
  ///     for anything that looks off and fall back to the officially sanctioned way to
  ///     query for this information (COM + WMI, *barf*).
  ///   </para>
  /// </remarks>
  class WindowsRegistryCpuInfoReader {

    /// <summary>
    ///   Signature for the callback function invoked by <see cref="TryQueryCpuInfos" />
    /// </summary>
    public: typedef void CallbackFunction(
      void *userPointer,
      std::size_t processorIndex,
      const std::string &name,
      double frequencyInMhz
    );

    /// <summary>
    ///   Attempts to read informations about processors from the Windows registry
    /// </summary>
    /// <param name="processorCount">Number of processors to query</param>
    /// <param name="userPointer">Will be passed to the callback unmodified</param>
    /// <param name="callback">
    ///   Callback that will be invoked for each processor, provided with the
    ///   user pointer, processor index, processor name and base frequency in GHz
    /// </param>
    /// <param name="canceller">Allows the enumeration to be cancelled early</param>
    /// <returns>
    ///   True if all informations were in the expected place, complete and in
    ///   the expected format and the callback was invoked for each processor.
    /// </returns>
    public: static bool TryReadCpuInfos(
      std::size_t processorCount,
      void *userPointer,
      CallbackFunction *callback,
      const std::shared_ptr<const Tasks::CancellationWatcher> &canceller
    );

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // defined(NUCLEX_PLATFORM_WINDOWS)

#endif // NUCLEX_PLATFORM_HARDWARE_WINDOWSREGISTRYCPUINFOREADER_H
