#include "axis.hpp"
#include <bx/math.h>

namespace {
// rotate vec by XYZ Euler (column-major mtx from bx)
inline math::Vec3 rotateXYZ(const math::Vec3& v, const math::Vec3& r) {
    float m[16];
    bx::mtxRotateXYZ(m, r.x, r.y, r.z);
    return math::Vec3{
        v.x * m[0] + v.y * m[4] + v.z * m[8],
        v.x * m[1] + v.y * m[5] + v.z * m[9],
        v.x * m[2] + v.y * m[6] + v.z * m[10],
    };
}
}

void axis::update_mesh()
{
    // local → world (scale -> rotate -> translate)
    const math::Vec3 pos = axis_transform.get_position();
    const math::Vec3 scl = axis_transform.get_scale();
    const math::Vec3 rot = axis_transform.get_rotation();

    auto xform = [&](const math::Vec3& p) -> math::Vec3 {
        math::Vec3 s = p * scl;
        math::Vec3 r = rotateXYZ(s, rot);
        return r + pos;
    };

    std::vector<vertex_info> verts;
    std::vector<uint16_t>   idx;

    verts.reserve(2048);
    idx.reserve(4096);

    auto add_line = [&](const math::Vec3& a, const math::Vec3& b, uint32_t ca, uint32_t cb) {
        const uint16_t i0 = static_cast<uint16_t>(verts.size());
        const uint16_t i1 = i0 + 1;
        const math::Vec3 A = xform(a);
        const math::Vec3 B = xform(b);
        verts.push_back({ A.x, A.y, A.z, A.x, A.y, A.z, ca });
        verts.push_back({ B.x, B.y, B.z, B.x, B.y, B.z, cb });
        idx.push_back(i0); idx.push_back(i1);
    };

    const float L = length_;

    // axes
    if (has_x(axes_)) add_line({-L, 0, 0}, {+L, 0, 0}, col_x_, col_x_);
    if (has_y(axes_)) add_line({0, -L, 0}, {0, +L, 0}, col_y_, col_y_);
    if (has_z(axes_)) add_line({0, 0, -L}, {0, 0, +L}, col_z_, col_z_);

    // floor grid (n lines in each direction, excluding the axis line at 0 to avoid duplicates)
    if (floor_enabled_) {
        const uint32_t n = floor_lines_count_;
        const float step = (n > 0) ? (L / float(n)) : L;

        auto grid_xy = [&]() {
            for (int i = 1; i <= int(n); ++i) {
                float o = step * float(i);
                // lines parallel to X at y = +/-o
                add_line({-L, +o, 0}, {+L, +o, 0}, floor_color_, floor_color_);
                add_line({-L, -o, 0}, {+L, -o, 0}, floor_color_, floor_color_);
                // lines parallel to Y at x = +/-o
                add_line({+o, -L, 0}, {+o, +L, 0}, floor_color_, floor_color_);
                add_line({-o, -L, 0}, {-o, +L, 0}, floor_color_, floor_color_);
            }
        };
        auto grid_xz = [&]() {
            for (int i = 1; i <= int(n); ++i) {
                float o = step * float(i);
                // lines parallel to X at z = +/-o
                add_line({-L, 0, +o}, {+L, 0, +o}, floor_color_, floor_color_);
                add_line({-L, 0, -o}, {+L, 0, -o}, floor_color_, floor_color_);
                // lines parallel to Z at x = +/-o
                add_line({+o, 0, -L}, {+o, 0, +L}, floor_color_, floor_color_);
                add_line({-o, 0, -L}, {-o, 0, +L}, floor_color_, floor_color_);
            }
        };
        auto grid_yz = [&]() {
            for (int i = 1; i <= int(n); ++i) {
                float o = step * float(i);
                // lines parallel to Y at z = +/-o
                add_line({0, -L, +o}, {0, +L, +o}, floor_color_, floor_color_);
                add_line({0, -L, -o}, {0, +L, -o}, floor_color_, floor_color_);
                // lines parallel to Z at y = +/-o
                add_line({0, +o, -L}, {0, +o, +L}, floor_color_, floor_color_);
                add_line({0, -o, -L}, {0, -o, +L}, floor_color_, floor_color_);
            }
        };

        if      (floor_plane_ == XY_AXIS) grid_xy();
        else if (floor_plane_ == XZ_AXIS) grid_xz();
        else if (floor_plane_ == YZ_AXIS) grid_yz();
    }

    // upload
    set_vertices_data(std::move(verts));
    set_lines_data(std::move(idx));     // draw with PT_LINES state
}
