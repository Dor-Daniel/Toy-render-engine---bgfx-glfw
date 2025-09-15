#pragma once
#include "../config.hpp"
#include "mesh.hpp"
#include "../components/transform.hpp"

class line : public Mesh::mesh {
private:
    Components::transform line_transform;
    math::Vec3 a_local{0.0f, 0.0f, 0.0f};
    math::Vec3 b_local{1.0f, 0.0f, 0.0f};
    uint32_t colA{0xffffffff}; // ABGR (bgfx)
    uint32_t colB{0xffffffff};

    void update_mesh();

public:
    line() = default;
    line(const math::Vec3& a, const math::Vec3& b, uint32_t abgrA = 0xffffffff, uint32_t abgrB = 0xffffffff);

    void set_transform(const Components::transform& t) { line_transform = t; update_mesh(); }
    void set_endpoints(const math::Vec3& a, const math::Vec3& b) { a_local = a; b_local = b; update_mesh(); }
    void set_colors(uint32_t abgrA, uint32_t abgrB) { colA = abgrA; colB = abgrB; update_mesh(); }
};
