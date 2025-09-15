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

//########################//
//      bgfx, GLFW        //
//########################//

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "./utils/utils.cpp"
#include "./utils/definitions.hpp"
#include "utils/math.hpp"