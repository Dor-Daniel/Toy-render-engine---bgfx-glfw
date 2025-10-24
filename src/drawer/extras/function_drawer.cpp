#include <vector>
#include "../mesh.hpp"

class function_drawer : public Mesh::mesh<PosNormColorVertex> {
public:
    function_drawer( Color col, float (*func)(float x, float y), math::Vec3 (*norm)(float x, float y)) {
    // half-size s
    auto V = [](float x,float y,float z, float nx,float ny,float nz, uint32_t c){
        return PosNormColorVertex{x,y,z, nx,ny,nz, c};
    };
    math::Vec2 scl = math::Vec2(50,50);
    math::Vec3 p = math::Vec3(0);

    std::vector<PosNormColorVertex> verts;
    std::vector<uint16_t> indices;

    int xSeg = 200;   // number of quads along X
    int ySeg = 200;   // number of quads along Y

    float dx = 0.5 * scl.x / xSeg;           // step in X
    float dy = 0.5 * scl.y / ySeg;           // step in Y

    int xCount = xSeg + 1;                      // vertices per row
    int yCount = ySeg + 1;                      // vertices per column

    verts.reserve(size_t(xCount) * size_t(yCount));  
    indices.reserve(size_t(xSeg) * size_t(ySeg) * 6);

    for (int j = 0; j < yCount; ++j) {
        for (int i = 0; i < xCount; ++i) {
            float x = p.x - scl.x/4 + dx * i;
            float z = p.z + scl.y/4 - dy * j;
            float y = (float)func(x, z) + 1;  
            math::Vec3 n =norm(x, z);  
            verts.push_back(V(
                x, y, z,
                n.x, n.y, n.z,              // normal +Z
                col                              
            ));
        }
    }
    auto idx = [xCount](int i, int j) -> uint16_t {
        return (uint16_t)(j * xCount + i);
    };

    for (int j = 0; j < ySeg; ++j) {
        for (int i = 0; i < xSeg; ++i) {
            uint16_t i0 = idx(i,     j);
            uint16_t i1 = idx(i + 1, j);
            uint16_t i2 = idx(i,     j + 1);
            uint16_t i3 = idx(i + 1, j + 1);

            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
        }
    }
    set_vertices_data(std::move(verts));
    set_triangles_data(std::move(indices));

    }
    ~function_drawer() = default;
};
