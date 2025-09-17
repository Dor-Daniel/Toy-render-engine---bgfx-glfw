#pragma once
#include <cstdint>
#include <GLFW/glfw3.h>

typedef uint32_t Color;

struct PosColorVertex {
    float x, y, z;
    Color abgr;
};

struct PosNormColorVertex {
    float x, y, z;
    float nx, ny, nz;
    Color abgr;
};

typedef PosNormColorVertex vertex_info;
typedef uint16_t triangles_info;

