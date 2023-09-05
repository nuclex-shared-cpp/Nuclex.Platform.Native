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

#include "Nuclex/Platform/Tasks/NaiveTaskCoordinator.h"
#include "./ResourceBudget.h"

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //


  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Tasks {

  // ------------------------------------------------------------------------------------------- //

  NaiveTaskCoordinator::NaiveTaskCoordinator() :
    availableResources(std::make_unique<ResourceBudget>()),
    threadPool(),
    totalCpuCoreCount(0) {}

  // ------------------------------------------------------------------------------------------- //

  NaiveTaskCoordinator::~NaiveTaskCoordinator() {
    if(this->threadPool.has_value()) {
      this->threadPool.reset();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::AddResource(
    Tasks::ResourceType resourceType, std::size_t amountAvailable
  ) {
    if(this->threadPool.has_value()) {
      throw std::logic_error(u8"Cannot add resources after Start() has been called");
    }

    this->availableResources->AddResource(resourceType, amountAvailable);
    if(resourceType == ResourceType::CpuCores) {
      this->totalCpuCoreCount += amountAvailable;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::Start() {
    if(this->totalCpuCoreCount == 0) {
      throw std::logic_error(u8"Please add at least one CPU core before starting");
    }
    if(this->threadPool.has_value()) {
      throw std::logic_error(u8"Start must not be called more than once");
    }
    
    this->threadPool.emplace(1, this->totalCpuCoreCount);
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t NaiveTaskCoordinator::QueryResourceMaximum(ResourceType resourceType) const {
    return this->availableResources->QueryResource(resourceType);
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::Schedule(
    const std::shared_ptr<Task> &task
  ) {
    (void)task;
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::Schedule(
    const std::shared_ptr<TaskEnvironment> &environment,
    const std::shared_ptr<Task> &task
  ) {
    (void)environment;
    (void)task;
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::ScheduleWithAlternative(
    const std::shared_ptr<Task> &preferredTask,
    const std::shared_ptr<Task> &alternativeTask
  ) {
    (void)preferredTask;
    (void)alternativeTask;
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::ScheduleWithAlternative(
    const std::shared_ptr<TaskEnvironment> &environment,
    const std::shared_ptr<Task> &preferredTask,
    const std::shared_ptr<Task> &alternativeTask
  ) {
    (void)environment;
    (void)preferredTask;
    (void)alternativeTask;
  }

  // ------------------------------------------------------------------------------------------- //
/*
  bool NaiveTaskCoordinator::Prioritize(const std::shared_ptr<Task> &task) {
    (void)task;
    return false;
  }
*/
  // ------------------------------------------------------------------------------------------- //

  bool NaiveTaskCoordinator::Cancel(const std::shared_ptr<Task> &task) {
    (void)task;
    return false;
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::CancelAll(bool forever /* = true */) {
    (void)forever;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Videl::Tasks
