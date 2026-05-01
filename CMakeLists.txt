cmake_minimum_required(VERSION 3.25)

if(APPLE AND (NOT DEFINED CMAKE_OSX_SYSROOT OR CMAKE_OSX_SYSROOT STREQUAL ""))
  execute_process(
    COMMAND xcrun --show-sdk-path
    OUTPUT_VARIABLE FEMSOLVER_MACOS_SDK
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
  if(FEMSOLVER_MACOS_SDK)
    set(CMAKE_OSX_SYSROOT
        "${FEMSOLVER_MACOS_SDK}"
        CACHE PATH "macOS SDK used for local builds" FORCE)
  endif()
endif()

project(
  femsolver
  VERSION 2.0.0
  DESCRIPTION "Finite element solver and validation platform for 3D electrical machines"
  LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not supported. Use a build/ directory or a CMake preset.")
endif()

if(APPLE)
  set(FEMSOLVER_CLT_LIBCXX_STRING "/Library/Developer/CommandLineTools/usr/include/c++/v1/string")
  set(FEMSOLVER_XCODE_LIBCXX_INCLUDE
      "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1")
  if(NOT EXISTS "${FEMSOLVER_CLT_LIBCXX_STRING}" AND EXISTS "${FEMSOLVER_XCODE_LIBCXX_INCLUDE}/string")
    include_directories(SYSTEM "${FEMSOLVER_XCODE_LIBCXX_INCLUDE}")
    message(STATUS "Using Xcode libc++ headers from ${FEMSOLVER_XCODE_LIBCXX_INCLUDE}")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)
include(ProjectOptions)

femsolver_print_configuration()

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
