#pragma once
#include <cstdint>
#include <GLFW/glfw3.h>

typedef uint32_t Color;

struct PosColorVertex {
    float x, y, z;
    Color abgr;
};

typedef PosColorVertex vertex_info;
typedef uint16_t triangles_info;

