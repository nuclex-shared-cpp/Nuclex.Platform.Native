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

#include "Nuclex/Platform/Locations/StandardDirectoryResolver.h"

#include <stdexcept> // for runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Locations {

  // ------------------------------------------------------------------------------------------- //

  StandardDirectoryResolver::StandardDirectoryResolver(
    const std::string &applicationName /* = std::string() */
  ) :
    applicationName(applicationName) {}

  // ------------------------------------------------------------------------------------------- //

  StandardDirectoryResolver::~StandardDirectoryResolver() {}

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetExecutableDirectory() const {
    if(this->executableDirectoryKnown.load(std::memory_order::memory_order_consume)) {
      return this->executableDirectory;
    }

    // Seems like we haven't figured out this directory yet, update it using
    // the private method (which contains the platform-specific lookup code)
    {
      std::lock_guard<std::mutex> updateLock(this->updateMutex);

      // Re-check because if there really was a competing thread for the mutex,
      // it will have filled in the necessary data when we're let in
      if(!this->executableDirectoryKnown.load(std::memory_order::memory_order_consume)) {
        this->executableDirectory = locateExecutableDirectory();
      }

      this->executableDirectoryKnown.store(true, std::memory_order::memory_order_release);
    }

    return this->executableDirectory;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetStaticDataDirectory() const {
    if(this->staticDataDirectoryKnown.load(std::memory_order::memory_order_consume)) {
      return this->staticDataDirectory;
    }

    // Seems like we haven't figured out this directory yet, update it using
    // the private method (which contains the platform-specific lookup code)
    {
      std::lock_guard<std::mutex> updateLock(this->updateMutex);

      // Re-check because if there really was a competing thread for the mutex,
      // it will have filled in the necessary data when we're let in
      if(!this->staticDataDirectoryKnown.load(std::memory_order::memory_order_consume)) {
        this->staticDataDirectory = locateStaticDataDirectory();

        // If an application name was provided, tuck it on so we end up in a subdirectory
        // such as /usr/local/share/my-app/. If none was provided, use the shared direcotry.
        if(!this->applicationName.empty()) {
          this->staticDataDirectory.append(this->applicationName);
          this->staticDataDirectory.push_back('/');
        }

        this->staticDataDirectoryKnown.store(true, std::memory_order::memory_order_release);
      }
    }

    return this->staticDataDirectory;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetSettingsDirectory() const {
    if(this->settingsDirectoryKnown.load(std::memory_order::memory_order_consume)) {
      return this->settingsDirectory;
    }

    // Seems like we haven't figured out this directory yet, update it using
    // the private method (which contains the platform-specific lookup code)
    {
      std::lock_guard<std::mutex> updateLock(this->updateMutex);

      // Re-check because if there really was a competing thread for the mutex,
      // it will have filled in the necessary data when we're let in
      if(!this->settingsDirectoryKnown.load(std::memory_order::memory_order_consume)) {
        this->settingsDirectory = locateSettingsDirectory();

        // If an application name was provided, tuck it on so we end up in a subdirectory
        // such as /usr/local/share/my-app/. If none was provided, use the shared direcotry.
        if(!this->applicationName.empty()) {
          this->settingsDirectory.append(this->applicationName);
          this->settingsDirectory.push_back('/');
        }

        this->settingsDirectoryKnown.store(true, std::memory_order::memory_order_release);
      }
    }

    return this->settingsDirectory;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetStateDirectory() const {
    if(this->stateDirectoryKnown.load(std::memory_order::memory_order_consume)) {
      return this->stateDirectory;
    }

    // Seems like we haven't figured out this directory yet, update it using
    // the private method (which contains the platform-specific lookup code)
    {
      std::lock_guard<std::mutex> updateLock(this->updateMutex);

      // Re-check because if there really was a competing thread for the mutex,
      // it will have filled in the necessary data when we're let in
      if(!this->stateDirectoryKnown.load(std::memory_order::memory_order_consume)) {
        this->stateDirectory = locateStateDirectory();

        // If an application name was provided, tuck it on so we end up in a subdirectory
        // such as /usr/local/share/my-app/. If none was provided, use the shared direcotry.
        if(!this->applicationName.empty()) {
          this->stateDirectory.append(this->applicationName);
          this->stateDirectory.push_back('/');
        }

        this->stateDirectoryKnown.store(true, std::memory_order::memory_order_release);
      }
    }

    return this->stateDirectory;
  }

  // ------------------------------------------------------------------------------------------- //

  std::string StandardDirectoryResolver::GetCacheDirectory() const {
    throw std::runtime_error(u8"Not implemented yet");
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
