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

#ifndef NUCLEX_PLATFORM_PLATFORM_LINUXFILEAPI_H
#define NUCLEX_PLATFORM_PLATFORM_LINUXFILEAPI_H

#include "Nuclex/Platform/Config.h"

#if defined(NUCLEX_PLATFORM_LINUX)

#include <string> // std::string
#include <cstdint> // std::uint8_t and std::size_t
#include <vector> // for std::vector

#include <sys/stat.h> // ::fstat() and permission flags
#include <dirent.h> // struct ::dirent

namespace Nuclex { namespace Platform { namespace Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wraps the Linux file system API</summary>
  /// <remarks>
  ///   <para>
  ///     This is just a small helper class that reduces the amount of boilerplate code
  ///     required when calling the file system API functions, such as checking
  ///     result codes over and over again.
  ///   </para>
  ///   <para>
  ///     It is not intended to hide operating system details or make this API platform
  ///     neutral (the File and Container classes do that), so depending on the amount
  ///     of noise required by the file system APIs, only some methods will be wrapped here.
  ///   </para>
  /// </remarks>
  class LinuxFileApi {

    /// <summary>Opens the specified file for shared reading</summary>
    /// <param name="path">Path of the file that will be opened</param>
    /// <returns>The descriptor (numeric handle) of the opened file</returns>
    public: static int OpenFileForReading(const std::string &path);

    /// <summary>Creates or opens the specified file for exclusive writing</summary>
    /// <param name="path">Path of the file that will be opened</param>
    /// <returns>The descriptor (numeric handle) of the opened file</returns>
    public: static int OpenFileForWriting(const std::string &path);

    /// <summary>Changes the position of the file cursor</summary>
    /// <param name="fileDescriptor">File handle whose file cursor will be moved</param>
    /// <param name="offset">Relative position, in bytes, to move the file cursor to</param>
    /// <param name="anchor">Position to which the offset is relative</param>
    /// <returns>The absolute position in bytes from the beginning of the file</returns>
    public: static std::size_t Seek(int fileDescriptor, ::off_t offset, int anchor);

    /// <summary>Reads data from the specified file</summary>
    /// <param name="fileDescriptor">Handle of the file from which data will be read</param>
    /// <param name="buffer">Buffer into which the data will be put</param>
    /// <param name="count">Number of bytes that will be read from the file</param>
    /// <returns>The number of bytes that were actually read</returns>
    public: static std::size_t Read(
      int fileDescriptor, std::uint8_t *buffer, std::size_t count
    );

    /// <summary>Writes data into the specified file</summary>
    /// <param name="fileDescriptor">Handle of the file into which data will be written</param>
    /// <param name="buffer">Buffer containing the data that will be written</param>
    /// <param name="count">Number of bytes that will be written into the file</param>
    /// <returns>The number of bytes that were actually written</returns>
    public: static std::size_t Write(
      int fileDescriptor, const std::uint8_t *buffer, std::size_t count
    );

    /// <summary>Truncates or pads the file to the specified length</summary>
    /// <param name="fileDescriptor">Handle of the file whose length will be set</param>
    /// <param name="byteCount">New length fo the file in bytes</param>
    public: static void SetLength(int fileDescriptor, std::size_t byteCount);

    /// <summary>Flushes all buffered output to the hard drive<summary>
    /// <param name="fileDescriptor">
    ///   File descriptor whose buffered output will be flushed
    /// </param>
    public: static void Flush(int fileDescriptor);

    /// <summary>Closes the specified file</summary>
    /// <param name="fileDescriptor">Handle of the file that will be closed</param>
    /// <param name="throwOnError">
    ///   Whether to throw an exception if the file cannot be closed
    /// </param>
    public: static void Close(int fileDescriptor, bool throwOnError = true);

    /// <summary>Reads the target file or directory pointed to by a symlink</summary>
    /// <param name="path">Path to the symlink whose target will be read</param>
    /// <param name="target">String that will receive the link target path</param>
    /// <param name="causingErrorNumber">
    ///   Will be filled with the errno value in case the method returns false
    /// </param>
    /// <returns>
    ///   True if the path was written into the target string, false if the link
    ///   didn't exist or couldn't be accessed (permissions). Any other problem will
    ///   still result in an exception being thrown.
    /// </returns>
    public: static bool TryReadLink(
      const std::string &path, std::string &target, int *causingErrorNumber = nullptr
    );

    /// <summary>Loads a whole file into memory and returns its contents an std::vector</summary>
    /// <param name="path">Absolute path of the file that will be loaded</param>
    /// <returns>A vector with the full file contents</returns>
    public: static std::vector<std::uint8_t> ReadFileIntoMemory(const std::string &path);

    /// <summary>
    ///   Attempts to read an entire file into a string with a single read() call
    /// </summary>
    /// <param name="path">Path of the file that will be read</param>
    /// <param name="contents">String that will receive the contents of the file</param>
    /// <returns>True if the read suceeded, false otherwise</returns>
    /// <remarks>
    ///   If the read fails, the contents and length of the output string are undefined.
    ///   This method makes an effort to read the whole file specified in one go into
    ///   the provided output string. This is useful to minimize the chance of mixed-up
    ///   file data when a file might be modified during the read. It's only used for
    ///   the XDG settings file, a small text file.
    /// </remarks>
    public: static bool TryReadFileInOneReadCall(
      const std::string &path, std::string &contents
    ) noexcept;

    /// <summary>Joins two paths together, inserted a forward slash when needed</summary>
    /// <param name="base">Base path, typically an absolute path or directory name</param>
    /// <param name="sub">Sub path, typically a filename with or without subdirectory</param>
    /// <returns>The joined path</returns>
    public: static std::string JoinPaths(const std::string &base, const std::string &sub);

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::Platform::Platform

#endif // defined(NUCLEX_PLATFORM_LINUX)

#endif // NUCLEX_PLATFORM_PLATFORM_LINUXFILEAPI_H
