# LoggerProject

A C++17 logging library and demonstration applications crafted with elegance and precision, designed to log messages with varying levels of importance, all while maintaining thread-safety and simplicity. Think of it as a trusty scribe for your application's adventures, whether writing to a file or whispering logs across a network socket.

## Overview

This project fulfills the requirements of the C++ Intern Test Task, implementing:
- **Part 1**: A logging library (`mylog`) that writes messages to a file or socket with configurable importance levels.
- **Part 2**: A multithreaded console application (`demo`) for testing the library.
- **Part 3**: A statistics collector (`stats`) for analyzing logs received via socket.
- **Tests**: Unit tests (`test_logger`) to ensure the library's reliability.

The code adheres to C++17, uses only STL and POSIX APIs, follows OOP principles, and avoids exceptions in business logic. The project is built with CMake and tested on Linux (Debian/Ubuntu).

## Project Structure

- **`include/mylog/`**: Header files for the `mylog` library.
  - `Importance.hpp`: Enum for log levels (`LOW`, `MEDIUM`, `HIGH`).
  - `Sink.hpp`: Abstract interface for log sinks.
  - `FileSink.hpp`: Sink for writing logs to a file.
  - `SocketSink.hpp`: Sink for sending logs via TCP socket.
  - `Logger.hpp`: Main logger class.
- **`src/`**:
  - `Logger.cpp`: Implementation of the logger.
  - `FileSink.cpp`: File-based logging.
  - `SocketSink.cpp`: Socket-based logging.
  - `demo.cpp`: Multithreaded demo application (Part 2).
  - `server.cpp`: Simple TCP server for testing socket logging.
  - `stats.cpp`: Statistics collector for socket logs (Part 3).
- **`tests/`**:
  - `test_logger.cpp`: Unit tests for the library (excluding `SocketSink`).
- **`CMakeLists.txt`**: Build configuration for library and applications.

## Prerequisites

- **OS**: Linux (Debian/Ubuntu recommended).
- **Compiler**: GCC with C++17 support.
- **Tools**: CMake 3.10+, `make`.
- **Libraries**: STL, POSIX threads, `stdc++fs` (for tests).

## Building the Project

```bash
mkdir build && cd build
cmake .. -DBUILD_SHARED_LIBS=OFF  # For static library; use ON for dynamic
cmake --build .
