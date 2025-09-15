#include "line.hpp"

namespace {
inline math::Vec3 rotateXYZ(const math::Vec3& v, const math::Vec3& r) {
    float m[16];
    bx::mtxRotateXYZ(m, r.x, r.y, r.z); // column-major
    return math::Vec3{
        math::dot(v, math::Vec3{m[0], m[4], m[8]}), 
        math::dot(v, math::Vec3{m[1], m[5], m[9]}), 
        math::dot(v, math::Vec3{m[2], m[6], m[10]})
    };
}
}

line::line(const math::Vec3& a, const math::Vec3& b, uint32_t abgrA, uint32_t abgrB)
    : a_local(a), b_local(b), colA(abgrA), colB(abgrB) {
    update_mesh();
}

void line::update_mesh() {
    const math::Vec3 pos = line_transform.get_position();
    const math::Vec3 scl = line_transform.get_scale();
    const math::Vec3 rot = line_transform.get_rotation();

    // scale → rotate → translate for each endpoint (like cube does) :contentReference[oaicite:3]{index=3}
    auto xform = [&](const math::Vec3& p) -> math::Vec3 {
        math::Vec3 s = p * scl;
        math::Vec3 r = rotateXYZ(s, rot);
        return r + pos;
    };

    math::Vec3 A = xform(a_local);
    math::Vec3 B = xform(b_local);

    std::vector<vertex_info> verts = {
        { A.x, A.y, A.z, colA },
        { B.x, B.y, B.z, colB },
    };

    // one line segment → two indices
    std::vector<uint16_t> idx = { 0, 1 };

    set_vertices_data(std::move(verts));
    set_lines_data(std::move(idx));
}
