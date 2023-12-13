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

#include "LinuxFileApi.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include "PosixApi.h" // Linux uses Posix error handling

#include <linux/limits.h> // for PATH_MAX
#include <fcntl.h> // ::open() and flags
#include <unistd.h> // ::read(), ::write(), ::close(), etc.

#include <cerrno> // To access ::errno directly
#include <vector> // std::vector

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>RAII scope that closes a file descriptor upon destruction</summary>
  class FileDescriptorClosingScope {

    /// <summary>Initializes a new file descriptor closing scope</summary>
    /// <param name="fileDescriptor">
    ///   File descriptor that will be closed when the instance is destroyed
    /// </param>
    public: FileDescriptorClosingScope(int fileDescriptor) :
      fileDescriptor(fileDescriptor) {}

    /// <summary>Closes the file descriptor when the instance is destroyed</summary>
    public: ~FileDescriptorClosingScope() {
      Nuclex::Platform::Platform::LinuxFileApi::Close(this->fileDescriptor, false);
    }

    /// <summary>File descriptor that will be closed upon destruction</summary>
    private: int fileDescriptor;

  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  int LinuxFileApi::OpenFileForReading(const std::string &path) {
    int fileDescriptor = ::open(path.c_str(), O_RDONLY | O_LARGEFILE);
    if(unlikely(fileDescriptor < 0)) {
      int errorNumber = errno;

      std::string errorMessage(u8"Could not open file '", 21);
      errorMessage.append(path);
      errorMessage.append(u8"' for reading", 13);

      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }

    return fileDescriptor;
  }

  // ------------------------------------------------------------------------------------------- //

  int LinuxFileApi::OpenFileForWriting(const std::string &path) {
    int fileDescriptor = ::open(
      path.c_str(),
      O_RDWR | O_CREAT | O_LARGEFILE,
      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
    );
    if(unlikely(fileDescriptor < 0)) {
      int errorNumber = errno;

      std::string errorMessage(u8"Could not open file '", 21);
      errorMessage.append(path);
      errorMessage.append(u8"' for writing", 13);

      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }

    return fileDescriptor;
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t LinuxFileApi::Seek(int fileDescriptor, ::off_t offset, int anchor) {
    ::off_t absolutePosition = ::lseek(fileDescriptor, offset, anchor);
    if(absolutePosition == -1) {
      int errorNumber = errno;
      std::string errorMessage(u8"Could not seek within file", 26);
      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }

    return static_cast<std::size_t>(absolutePosition);
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t LinuxFileApi::Read(
    int fileDescriptor, std::uint8_t *buffer, std::size_t count
  ) {
    ssize_t result = ::read(fileDescriptor, buffer, count);
    if(unlikely(result == static_cast<ssize_t>(-1))) {
      int errorNumber = errno;
      std::string errorMessage(u8"Could not read data from file", 29);
      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }

    return static_cast<std::size_t>(result);
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t LinuxFileApi::Write(
    int fileDescriptor, const std::uint8_t *buffer, std::size_t count
  ) {
    ssize_t result = ::write(fileDescriptor, buffer, count);
    if(unlikely(result == static_cast<ssize_t>(-1))) {
      int errorNumber = errno;
      std::string errorMessage(u8"Could not write data to file", 28);
      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  void LinuxFileApi::SetLength(int fileDescriptor, std::size_t byteCount) {
    int result = ::ftruncate(fileDescriptor, static_cast<::off_t>(byteCount));
    if(result == -1) {
      int errorNumber = errno;
      std::string errorMessage(u8"Could not truncate/pad file to specified length", 47);
      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void LinuxFileApi::Flush(int fileDescriptor) {
    int result = ::fsync(fileDescriptor);
    if(unlikely(result == -1)) {
      int errorNumber = errno;
      std::string errorMessage(u8"Could not flush file buffers", 28);
      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void LinuxFileApi::Close(int fileDescriptor, bool throwOnError /* = true */) {
    int result = ::close(fileDescriptor);
    if(throwOnError && unlikely(result == -1)) {
      int errorNumber = errno;
      std::string errorMessage(u8"Could not close file", 20);
      Platform::PosixApi::ThrowExceptionForSystemError(errorMessage, errorNumber);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::vector<std::uint8_t> LinuxFileApi::ReadFileIntoMemory(const std::string &path) {
    std::vector<std::uint8_t> contents;

    {
      int fileDescriptor = Nuclex::Platform::Platform::LinuxFileApi::OpenFileForReading(path);
      FileDescriptorClosingScope closeFileDescriptor(fileDescriptor);

      contents.resize(4096);
      for(std::size_t offset = 0;;) {
        std::size_t readByteCount = Nuclex::Platform::Platform::LinuxFileApi::Read(
          fileDescriptor, contents.data() + offset, 4096
        );
        if(readByteCount == 0) { // 0 bytes are only returned at the end of the file
          contents.resize(offset); // drop the 4k we added for appending
          break;
        } else { // 1 or more bytes returned, increase buffer for another round
          offset += readByteCount;
          contents.resize(offset + 4096); // extend so that 4k bytes are available again
        }
      }
    }

    return contents;
  }

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_LINUX)
