  - **Status:** Early development. Most of the planned features do not work,
    including message boxes, the task coordinator and half of the hardware info.

  - **Platforms:** Cross-platform, developed on Linux, will eventually
    receive testing on Windows.

Nuclex.Platform.Native
======================

This library provides you with basic tools to make use of the resources
provided by any platform. This includes identifying the system's hardware,
displaying notifications to the user and running tasks while managing
available resources between them.

**Hardware**
* Determine number of CPUs, cores and threads, names and frequency
* Get amount of installed memory, available and maximum per-process memory
* Enumerate connected hard drives, their partitions and file systems
* Find out what GPUs the system has, their Vulkan support and memory

**Interaction**
* Display message boxes to the user, using native operating system dialogs
* Ask the user to make choices, using extended dialogs provided by the OS
* Let the user pick a file or a folder

**Tasks**
* Distribute work over any number of CPU cores
* Run resource-intensive tasks in a controlled fashion within resource limits
* Cancel background tasks in a unified way

**Everything:**
* Supports Windows, Linux and ARM Linux (Raspberry PI)
* Compiles cleanly at maximum warning levels with MSVC, GCC and clang
* If it's there, it's unit-tested


PlatformAppraiser
-----------------

Planning...

```
vector<CpuInfo>
  + ModelName
  + CoreCount
  + EcoCoreCount
  + ThreadCount
  + vector<CoreInfo> Cores
    + FrequencyInMhz
    + IsEcoCore
    + ThreadCount
```

```
MemoryInfo
  + InstalledMegabytes
  + MaximumProgramMegabytes
```

```
vector<DriveInfo>
  + ManfacturerName
  + ModelName
  + CapacityInMegabytes
  + IsSolidState
  + vector<PartitionInfo> Partitions
    + MountPath
    + Label
    + CapacityInMegabytes
    + FileSystemName
```

