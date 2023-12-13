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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_PLATFORM_SOURCE 1

#include "./WindowsBasicCpuInfoReader.h"

#if defined(NUCLEX_PLATFORM_WINDOWS)

#include "../Platform/WindowsSysInfoApi.h" // for WindowsSysInfoApi

namespace Nuclex { namespace Platform { namespace Hardware {

  // ------------------------------------------------------------------------------------------- //

  WindowsBasicCpuInfoReader::WindowsBasicCpuInfoReader() :
    UsesHyperThreading(false),
    NonZeroEfficiencySpotted(false),
    LowestEfficiencySeen(0),
    HighestEfficiencySeen(0),
    PhysicalCpuCount(0),
    CoreCount(0),
    ThreadCount(0),
    GroupsOfProcessors() {}

  // ------------------------------------------------------------------------------------------- //

  //WindowsBasicCpuInfoReader::~WindowsBasicCpuInfoReader() = default;

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicCpuInfoReader::FetchViaWindowsXpApi() {

    // Retrieve the CPU information we can get via the regular Windows API
    std::vector<std::uint8_t> logicalProcessorInformationBuffer(
      Platform::WindowsSysInfoApi::GetLogicalProcessorInformation()
    );

    std::size_t bufferSize = logicalProcessorInformationBuffer.size();

    // The ::GetLogicalProcessorInformation() method will have written multiple processor
    // information structures into our buffer, covering all relationships in undefined
    // order. So now we need to sort through these to extract the information we need.
    std::size_t offset = 0;
    while(bufferSize >= offset + sizeof(::SYSTEM_LOGICAL_PROCESSOR_INFORMATION)) {
      ingestLogicalProcessor(
        *reinterpret_cast<const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION *>(
          logicalProcessorInformationBuffer.data() + offset
        )
      );
      offset += sizeof(::SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
    }

  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicCpuInfoReader::FetchViaWindowsSevenApi() {

    // Retrieve the CPU information we can get via the regular Windows API
    std::vector<std::uint8_t> extendedLogicalProcessorInformationBuffer(
      Platform::WindowsSysInfoApi::GetLogicalProcessorInformationEx()
    );

    std::size_t bufferSize = extendedLogicalProcessorInformationBuffer.size();

    // The ::GetLogicalProcessorInformation() method will have written multiple variably-sized
    // extended processor information structures into our buffer, covering all relationships
    // in undefined order. So now we need to sort through these to obtain our information.
    std::size_t offset = 0;
    while(bufferSize >= offset + sizeof(::SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)) {
      const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX &logicalProcesorInformation = (
        *reinterpret_cast<const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(
          extendedLogicalProcessorInformationBuffer.data() + offset
        )
      );
      ingestLogicalProcessor(logicalProcesorInformation);

      offset += static_cast<std::size_t>(logicalProcesorInformation.Size);
    }

  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicCpuInfoReader::ingestLogicalProcessor(
    const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION &logicalProcessor
  ) {
    switch(logicalProcessor.Relationship) {

      // This record is generated to report a many-to-many relationship between CPU cores
      // and "processors", i.e. often between one core and two processors if Hyper-Threading
      // is present and enabled on the system.
      case LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore: {
        ++this->CoreCount;

        // From MSDN: If the value of this member is 1, the logical processors identified
        // by the value of the ProcessorMask member share functional units,
        // as in Hyperthreading or SMT.
        if((logicalProcessor.ProcessorCore.Flags & 1) != 0) {
          this->UsesHyperThreading = true;
        }

        // The XP-age API does not have processor groups (thus limited to 64 cores),
        // so we auto-create a single processaor group to stick everything into
        if(this->GroupsOfProcessors.empty()) {
          this->GroupsOfProcessors.emplace_back();
        }
        std::vector<ProcessorInfo> &processors = this->GroupsOfProcessors.front();

        // Assign all processors in the mask to a core (we simply count the cores as
        // there is no core index reported and the ordering is stated to be random)
        constexpr std::size_t bitCount = (
          std::numeric_limits<decltype(logicalProcessor.ProcessorMask)>::digits
        );
        for(std::size_t bitIndex = 0; bitIndex < bitCount; ++bitIndex) {
          decltype(logicalProcessor.ProcessorMask) checkedBit = 1;
          checkedBit <<= bitIndex;

          // This is where the many-to-many part of the relationship comes into play.
          // Through the bit mask, the same processors may be listed for other cores,
          // but that would be reverse of HyperThreading - one thread on multiple cores.
          if((logicalProcessor.ProcessorMask & checkedBit) != 0) {
            if(bitIndex >= processors.size()) {
              processors.resize(bitIndex + 1);
            }
            processors[bitIndex].CoreIndex = this->CoreCount;
          }
        }

        break;
      }

      // This record is generated to report a physical CPU and the cores in it.
      case LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorPackage: {
        ++this->PhysicalCpuCount;

        // The XP-age API does not have processor groups (thus limited to 64 cores),
        // so we auto-create a single processaor group to stick everything into
        if(this->GroupsOfProcessors.empty()) {
          this->GroupsOfProcessors.emplace_back();
        }
        std::vector<ProcessorInfo> &processors = this->GroupsOfProcessors.front();

        // Assign the physical CPU index to all processors included in the bit mask
        constexpr std::size_t bitCount = (
          std::numeric_limits<decltype(logicalProcessor.ProcessorMask)>::digits
        );
        for(std::size_t bitIndex = 0; bitIndex < bitCount; ++bitIndex) {
          decltype(logicalProcessor.ProcessorMask) checkedBit = 1;
          checkedBit <<= bitIndex;

          if((logicalProcessor.ProcessorMask & checkedBit) != 0) {
            if(bitIndex >= processors.size()) {
              processors.resize(bitIndex + 1);
            }
            processors[bitIndex].PhysicalCpuIndex = this->PhysicalCpuCount;
          }
        }

        break;
      }

    } // switch
  }

  // ------------------------------------------------------------------------------------------- //

  void WindowsBasicCpuInfoReader::ingestLogicalProcessor(
    const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX &logicalProcessor
  ) {
    switch(logicalProcessor.Relationship) {

      // This record is generated to report a many-to-many relationship between CPU cores
      // and "processors", i.e. often between one core and two processors if Hyper-Threading
      // is present and enabled on the system.
      case LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore: {
        ++this->CoreCount;

        // From MSDN: this member is LTP_PC_SMT if the core has more than one
        // logical processor, or 0 if the core has one logical processor.
        if((logicalProcessor.Processor.Flags & LTP_PC_SMT) != 0) {
          this->UsesHyperThreading = true;
        }

        // If the efficiency value is non-zero, that means there are different kinds
        // of cores present on the CPU. We'll keep track of their range.
        std::uint8_t efficiency = logicalProcessor.Processor.EfficiencyClass;
        if(efficiency != 0) {
          if(this->NonZeroEfficiencySpotted) {
            this->LowestEfficiencySeen = std::min(this->LowestEfficiencySeen, efficiency);
            this->HighestEfficiencySeen = std::max(this->HighestEfficiencySeen, efficiency);
          } else {
            this->LowestEfficiencySeen = this->HighestEfficiencySeen = efficiency;
          }
        }

        // The processors may be split into groups. The rules for this appear a bit vague
        // and there's hint it has to do with physical proximity and 64 processor limits
        // (but how? a single actual core with >64 HyperThreads or in two physical locations?)
        std::size_t groupCount = logicalProcessor.Processor.GroupCount;
        for(std::size_t index = 0; index < groupCount; ++index) {
          const ::GROUP_AFFINITY &groupMask = logicalProcessor.Processor.GroupMask[index];

          // The bits all reference the processors in the stated group only,
          // so we may need to set up the group first
          if(groupMask.Group >= this->GroupsOfProcessors.size()) {
            this->GroupsOfProcessors.resize(groupMask.Group + 1);
          }
          std::vector<ProcessorInfo> &processors = this->GroupsOfProcessors[groupMask.Group];

          // Assign all processors in the mask to a core (we simply count the cores as
          // there is no core index reported and the ordering is stated to be random)
          constexpr std::size_t bitCount = (
            std::numeric_limits<decltype(groupMask.Mask)>::digits
          );
          for(std::size_t bitIndex = 0; bitIndex < bitCount; ++bitIndex) {
            decltype(groupMask.Mask) checkedBit = 1;
            checkedBit <<= bitIndex;

            // This is where the many-to-many part of the relationship comes into play.
            // Through the bit mask, the same processors may be listed for other cores,
            // but that would be reverse of HyperThreading - one thread on multiple cores.
            if((groupMask.Mask & checkedBit) != 0) {
              if(bitIndex >= processors.size()) {
                processors.resize(bitIndex + 1);
              }
              processors[bitIndex].CoreIndex = this->CoreCount;
              processors[bitIndex].Efficiency = efficiency;
            }
          }
        }

        break;
      }

      // This record is generated to report a physical CPU and the cores in it.
      case LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorPackage: {
        ++this->PhysicalCpuCount;

        // The processors may be split into groups. The rules for this appear a bit vague
        // and there's hint it has to do with physical proximity and 64 processor limits
        // (but how? a single actual core with >64 HyperThreads or in two physical locations?)
        std::size_t groupCount = logicalProcessor.Processor.GroupCount;
        for(std::size_t index = 0; index < groupCount; ++index) {
          const ::GROUP_AFFINITY &groupMask = logicalProcessor.Processor.GroupMask[index];

          // The bits all reference the processors in the stated group only,
          // so we may need to set up the group first
          if(groupMask.Group >= this->GroupsOfProcessors.size()) {
            this->GroupsOfProcessors.resize(groupMask.Group + 1);
          }
          std::vector<ProcessorInfo> &processors = this->GroupsOfProcessors[groupMask.Group];

          // Assign all processors in the mask to a core (we simply count the cores as
          // there is no core index reported and the ordering is stated to be random)
          constexpr std::size_t bitCount = (
            std::numeric_limits<decltype(groupMask.Mask)>::digits
          );
          for(std::size_t bitIndex = 0; bitIndex < bitCount; ++bitIndex) {
            decltype(groupMask.Mask) checkedBit = 1;
            checkedBit <<= bitIndex;

            if((groupMask.Mask & checkedBit) != 0) {
              if(bitIndex >= processors.size()) {
                processors.resize(bitIndex + 1);
              }
              processors[bitIndex].PhysicalCpuIndex = this->PhysicalCpuCount;
            }
          }
        }

        break;
      }

    } // switch
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Hardware

#endif // !defined(NUCLEX_PLATFORM_WINDOWS)
