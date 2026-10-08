#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <print>
#include <random>
#include <ranges>
#include <sstream>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#if defined(_MSC_VER)
#pragma warning(push)    // save warning flags state
#pragma warning(push, 0) // suppress all warnings
#include <boost/capy.hpp>
#pragma warning(pop) // remove suppressions
#pragma warning(pop) // restore original warning flags and clear pragma stack

#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push              // save warning flags state
#pragma GCC diagnostic warning "-Werror" // disable warning-to-error convertation
// #pragma GCC diagnostic ignored "-Wall"
// #pragma GCC diagnostic ignored "-Wextra"
// #if defined(__clang__)
// #pragma clang diagnostic ignored "-Weverything"
// #endif
#endif

#include <raylib.h>

#define CL_HPP_TARGET_OPENCL_VERSION 300 // Specifies target OpenCL version
#include <CL/opencl.hpp>                 // Core OpenCL C header

#include "utils.hpp"
