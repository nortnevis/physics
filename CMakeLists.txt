cmake_minimum_required(VERSION 3.31 FATAL_ERROR)
cmake_policy(VERSION 3.31)

set(CMAKE_LANG "en_US.UTF-8")

if(NOT DEFINED ENV{VCPKG_ROOT})
  message(FATAL_ERROR "VCPKG_ROOT is not defined!")
else()
  message("VCPKG_ROOT is defined: $ENV{VCPKG_ROOT}")
endif()

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_TOOLCHAIN_FILE)

project(
  physics
  DESCRIPTION "Physics emulation using GPU"
  VERSION 0.1.0
  LANGUAGES CXX)

cmake_print_variables(CMAKE_CXX_COMPILER_ID)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/W4 /WX /EHs-c-)
  add_compile_definitions(_HAS_EXCEPTIONS=0)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Werror -fno-exceptions -fno-unwind-tables
                      -fno-asynchronous-unwind-tables)
endif()

find_package(raylib CONFIG REQUIRED)
find_package(OpenCL CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

file(GLOB pch_files CONFIGURE_DEPENDS src/pch/*.[hc]pp)

add_library(pch STATIC ${pch_files})
target_precompile_headers(pch PUBLIC src/pch/pch.hpp)
target_link_libraries(pch PUBLIC raylib glfw OpenCL::OpenCL)

file(GLOB hpp_files CONFIGURE_DEPENDS src/*.hpp)
file(GLOB cpp_files CONFIGURE_DEPENDS src/*.cpp)

add_executable(app ${hpp_files} ${cpp_files})
target_precompile_headers(app REUSE_FROM pch)
target_link_libraries(app PRIVATE pch)

if(EXISTS "${PROJECT_SOURCE_DIR}/snippets/")
  add_subdirectory(snippets)
endif()

if(EXISTS ${PROJECT_BINARY_DIR}/compile_commands.json)
  message(
    "Copying ${PROJECT_BINARY_DIR}/compile_commands.json to root directory")
  file(COPY ${PROJECT_BINARY_DIR}/compile_commands.json
       DESTINATION ${PROJECT_SOURCE_DIR})
else()
  # message(WARNING "File ${PROJECT_BINARY_DIR}/compile_commands.json not found.
  # Maybe this file didn't created yet, so reconfigure CMake project.")
endif()
