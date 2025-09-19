#pragma once


//########################//
//        standard        //
//########################//

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <cstddef>
#include <array>
#include <map>
#include <deque>

//########################//
//      bgfx, GLFW        //
//########################//

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <bx/allocator.h>
// Pick the native header path per-OS:
#if defined(_WIN32)
  #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
  // Use X11; if you run pure Wayland, switch to GLFW_EXPOSE_NATIVE_WAYLAND
  #define GLFW_EXPOSE_NATIVE_X11
#elif defined(__APPLE__)
  #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3.h>


#include "./utils/utils.cpp"
#include "./utils/definitions.hpp"
#include "utils/math.hpp"