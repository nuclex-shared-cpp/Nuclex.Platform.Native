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
    totalCpuCoreCount(0),
    threadPool(), // leave the std::optional empty for now,
    coordinationThreadRunningFlag(false),
    coordinationThreadFuture(),
    coordinationThreadShutdownFlag(false),
    queueAccessMutex(),
    waitingTasks(),
    tasksAvailableSemaphore(0) {} 

  // ------------------------------------------------------------------------------------------- //

  NaiveTaskCoordinator::~NaiveTaskCoordinator() {

    // Set everything up so a (possibly) running coordination thread will cancel at
    // the next opportunity it has.
    this->coordinationThreadShutdownFlag.store(true, std::memory_order::memory_order_release);
    this->tasksAvailableSemaphore.Post(1024); // Just make sure that coordation thread wakes :)

    // Now, if the coordination thread actually *was* running, wait for it to shut down.
    bool coordinationThreadWasRunning = (
      this->coordinationThreadRunningFlag.load(std::memory_order::memory_order_consume)
    );
    if(coordinationThreadWasRunning) {
      reinterpret_cast<std::future<void> *>(this->coordinationThreadFuture)->wait();
    }
    
    // Finally, if the coordination thread has stopped, we can rest assured that no
    // tasks are running any, so we can kill the thread pool
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

    // Set up the thread pool.
    //
    // We'll allow it to grow up to the size of schedulable CPU cores, so even if the user
    // schedules that number of invididual tasks, each blocking just 1 CPU core and riding it
    // right in the Schedule() method, we've got enough threads.
    //
    std::size_t maximumThreadCount = this->totalCpuCoreCount;

    // On top of that, we give it 4 threads per GPU, so that 4 more tasks could run tasks on
    // the GPU(s) of the system.
    //
    // There's no surefire formula for an upper bound of the thread count, but I tried to
    // avoid just saying INT_MAX or something here. Perhaps I should?
    //
    maximumThreadCount += 4 * this->availableResources->CountResourceUnits(
      ResourceType::VideoMemory
    );

    // And one as our coordination thread, to kick off scheduled tasks. This one will run
    // throughout the lifetime of the task coordinator and check available resources whenever
    // a task is scheduled or finishes to potentially kick off the next one(s).
    //
    maximumThreadCount += 1;

    {
      std::lock_guard<std::mutex> queueAccessLock(this->queueAccessMutex);

      // Now we create the thread pool. As the minimum, we have 2 threads to handle the first
      // incoming tasks and 1 thread that will become our execution.
      this->threadPool.emplace(3, maximumThreadCount);

      new(reinterpret_cast<std::future<void> *>(this->coordinationThreadFuture)) std::future(
        this->threadPool->Schedule(&NaiveTaskCoordinator::invokeCoordinationThread, this)
      );

      this->coordinationThreadRunningFlag.store(true, std::memory_order::memory_order_release);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t NaiveTaskCoordinator::QueryResourceMaximum(ResourceType resourceType) const {
    return this->availableResources->QueryResourceMaximum(resourceType);
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::Schedule(
    const std::shared_ptr<Task> &task
  ) {
    std::lock_guard<std::mutex> queueAccessLock(this->queueAccessMutex);

    this->waitingTasks.emplace_back(task);
    // TODO: Check if execution thread needs to be launched by checking number of active cores
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::Schedule(
    const std::shared_ptr<TaskEnvironment> &environment,
    const std::shared_ptr<Task> &task
  ) {
    std::lock_guard<std::mutex> queueAccessLock(this->queueAccessMutex);

    this->waitingTasks.emplace_back(task, environment);
    // TODO: Check if execution thread needs to be launched by checking number of active cores
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

  void NaiveTaskCoordinator::KickOffRunnableTasks() {
    
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::coordinationThread() {
    for(;;) {

      // If there are no tasks and the we're not asked to shut down, go to sleep
      // to ensure we're not hogging a CPU core for no reason.
      bool wasTaskAvailable = this->tasksAvailableSemaphore.WaitForThenDecrement(
        std::chrono::milliseconds(50)
      );

      // When woken up, check if the we're being asked to shut down before anything
      // else so we can facilitate a timely shutdown.
      bool wasRequestedToShutDown = this->coordinationThreadShutdownFlag.load(
        std::memory_order::memory_order_relaxed
      );
      if(wasRequestedToShutDown) {
        break;
      }

      // If a task is waiting, see if we can kick off that one or another task
      if(wasTaskAvailable) {
        KickOffRunnableTasks();
      }

    }
  }

  // ------------------------------------------------------------------------------------------- //

  void NaiveTaskCoordinator::invokeCoordinationThread(NaiveTaskCoordinator *self) {
    self->coordinationThread();
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Tasks
