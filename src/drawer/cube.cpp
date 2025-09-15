#include "cube.hpp"

cube::cube(){
    std::vector<vertex_info> pos_color_verts ={
        {-1.0f,  1.0f,  1.0f, 0xff000000 },
        { 1.0f,  1.0f,  1.0f, 0xff0000ff },
        {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
        { 1.0f, -1.0f,  1.0f, 0xff00ffff },
        {-1.0f,  1.0f, -1.0f, 0xffff0000 },
        { 1.0f,  1.0f, -1.0f, 0xffff00ff },
        {-1.0f, -1.0f, -1.0f, 0xffffff00 },
        { 1.0f, -1.0f, -1.0f, 0xffffffff },
    };

std::vector<uint16_t> lines = {
    // front face (z=+1)
    0,1,  1,3,  3,2,  2,0,
    // back face (z=-1)
    4,5,  5,7,  7,6,  6,4,
    // vertical edges connecting front/back
    0,4,  1,5,  2,6,  3,7,
};

    // anti clockwise order
    std::vector<uint16_t> triangles = {
        0, 1, 2, // 0
        1, 3, 2,
        4, 6, 5, // 2
        5, 6, 7,
        0, 2, 4, // 4
        4, 2, 6,
        1, 5, 3, // 6
        5, 7, 3,
        0, 4, 1, // 8
        4, 5, 1,
        2, 3, 6, // 10
        6, 3, 7,
    };

    set_vertices_data(std::move(pos_color_verts));
    //set_triangles_data(std::move(triangles));
    set_lines_data(lines);
}

void cube::update_mesh() {
    const math::Vec3 pos = cube_transform.get_position();
    const math::Vec3 scl = cube_transform.get_scale();      // half-extents
    const math::Vec3 rot = cube_transform.get_rotation();   // radians (X,Y,Z)

    // Rotate a vector by XYZ Euler using math's column-major 4x4
    auto rotateXYZ = [](const math::Vec3& v, const math::Vec3& r) -> math::Vec3 {
        float m[16];
        bx::mtxRotateXYZ(m, r.x, r.y, r.z);
        return math::Vec3{
            v.x * m[0] + v.y * m[4] + v.z * m[8],
            v.x * m[1] + v.y * m[5] + v.z * m[9],
            v.x * m[2] + v.y * m[6] + v.z * m[10],
        };
    };

    // Local corners relative to the cube center (so rotation is about center)
    const math::Vec3 v1{-scl.x, +scl.y, +scl.z};
    const math::Vec3 v2{+scl.x, +scl.y, +scl.z};
    const math::Vec3 v3{-scl.x, -scl.y, +scl.z};
    const math::Vec3 v4{+scl.x, -scl.y, +scl.z};
    const math::Vec3 v5{-scl.x, +scl.y, -scl.z};
    const math::Vec3 v6{+scl.x, +scl.y, -scl.z};
    const math::Vec3 v7{-scl.x, -scl.y, -scl.z};
    const math::Vec3 v8{+scl.x, -scl.y, -scl.z};

    // Rotate, then translate to world position
    math::Vec3 u1 = rotateXYZ(v1, rot);
    math::Vec3 u2 = rotateXYZ(v2, rot);
    math::Vec3 u3 = rotateXYZ(v3, rot);
    math::Vec3 u4 = rotateXYZ(v4, rot);
    math::Vec3 u5 = rotateXYZ(v5, rot);
    math::Vec3 u6 = rotateXYZ(v6, rot);
    math::Vec3 u7 = rotateXYZ(v7, rot);
    math::Vec3 u8 = rotateXYZ(v8, rot);

    auto translate = [&](math::Vec3& v) { v.x += pos.x; v.y += pos.y; v.z += pos.z; };
    translate(u1); translate(u2); translate(u3); translate(u4);
    translate(u5); translate(u6); translate(u7); translate(u8);

    std::vector<vertex_info> pos_color_verts = {
        {u1.x, u1.y, u1.z, 0xff000000},
        {u2.x, u2.y, u2.z, 0xff0000ff},
        {u3.x, u3.y, u3.z, 0xff00ff00},
        {u4.x, u4.y, u4.z, 0xff00ffff},
        {u5.x, u5.y, u5.z, 0xffff0000},
        {u6.x, u6.y, u6.z, 0xffff00ff},
        {u7.x, u7.y, u7.z, 0xffffff00},
        {u8.x, u8.y, u8.z, 0xffffffff},
    };

    // Re-upload vertices; indices don't change
    set_vertices_data(std::move(pos_color_verts));
}


