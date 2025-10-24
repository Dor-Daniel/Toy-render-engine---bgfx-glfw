#include <vector>
#include "../mesh.hpp"

class sphere : public Mesh::mesh<PosNormColorVertex> {
public:
    sphere(float r, Color col) {
        const int slices = 64;   // longitude (θ) divisions
        const int stacks = 64;   // latitude (φ) divisions

        const int cols = slices + 1; // wrap seam
        const int rows = stacks + 1;

        std::vector<PosNormColorVertex> verts;
        std::vector<uint16_t> indices; // use 32-bit indices for safety

        verts.reserve(cols * rows);
        indices.reserve(slices * stacks * 6);

        // Build vertices
        for (int j = 0; j < rows; ++j) {
            float v = float(j) / stacks;               // [0,1]
            float phi = v * bx::kPi;                   // [0, π]
            float sinPhi = bx::sin(phi);
            float cosPhi = bx::cos(phi);

            for (int i = 0; i < cols; ++i) {
                float u = float(i) / slices;           // [0,1]
                float theta = u * (bx::kPi * 2.0f);    // [0, 2π)
                float sinTheta = bx::sin(theta);
                float cosTheta = bx::cos(theta);

                // Unit direction
                math::Vec3 n(
                    cosTheta * sinPhi,
                    cosPhi,
                    sinTheta * sinPhi
                );

                math::Vec3 p = n * r; // position at radius r

                verts.push_back(PosNormColorVertex{
                    p.x, p.y, p.z,
                    n.x, n.y, n.z,
                    col // ensure this matches PosNormColorVertex color type
                });
            }
        }

        auto idx = [cols](int i, int j) -> uint16_t {
            return uint16_t(j * cols + i);
        };

        // Build indices (two triangles per quad)
        for (int j = 0; j < stacks; ++j) {
            for (int i = 0; i < slices; ++i) {
                uint16_t i0 = idx(i,     j);
                uint16_t i1 = idx(i + 1, j);
                uint16_t i2 = idx(i,     j + 1);
                uint16_t i3 = idx(i + 1, j + 1);

                indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
                indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
            }
        }

        set_vertices_data(verts);
        set_triangles_data(indices);
    }
    ~sphere() = default;
};
