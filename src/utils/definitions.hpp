#pragma once
#include <cstdint>
#include <GLFW/glfw3.h>

typedef uint32_t Color;

struct PosColorVertex {
    float x, y, z;
    Color abgr;

    static void init(bgfx::VertexLayout& l) {
        l.begin(bgfx::getRendererType());
        l.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
        l.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true); // normalized
        l.end();
    }
};

struct PosNormColorVertex {
    float x, y, z;
    float nx, ny, nz;
    Color abgr;

    static void init(bgfx::VertexLayout& l) {
        l.begin(bgfx::getRendererType());
        l.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
        l.add(bgfx::Attrib::Normal,   3, bgfx::AttribType::Float);
        l.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true);
        l.end();
    }
};

typedef PosNormColorVertex vertex_info;
typedef uint16_t triangles_info;

