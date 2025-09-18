#include "plane.hpp"

plane::plane(Color col){

    create_plane(col);
}
void plane::update_transform(){
    if (!si) return;
    const math::Vec3 pos = plane_transform.get_position();
    const math::Vec3 scl = plane_transform.get_scale();      // half-extents
    const math::Vec3 rot = plane_transform.get_rotation();   // radians (X,Y,Z)

    float result[16];
    bx::mtxSRT(
            result,
                scl.x, scl.y, scl.z,
                rot.x, rot.y, rot.z,   
                pos.x, pos.y, pos.z
    );
    for(int i = 0; i < 16; i++)
        si->transform_matrix[i] = result[i];
}

void plane::update_mesh(){
    bgfx::VertexLayout lay; PosNormColorVertex::init(lay);
    if(si) si->set_vertex_data(pos_color_vertex.data(), size_of_vertices_buff, lay);
}
void plane::create_plane(Color col){
    math::Vec3 scl = plane_transform.get_scale();
    math::Vec3 p = plane_transform.get_position();
    // half-size s
    auto V = [](float x,float y,float z, float nx,float ny,float nz, uint32_t c){
        return PosNormColorVertex{x,y,z, nx,ny,nz, c};
    };

    std::vector<PosNormColorVertex> verts;
    std::vector<uint16_t> indices;

    int xSeg = std::max(1, (int)subdivide.x);   // number of quads along X
    int ySeg = std::max(1, (int)subdivide.y);   // number of quads along Y

    float dx = 0.5 * scl.x / xSeg;           // step in X
    float dy = 0.5 * scl.y / ySeg;           // step in Y

    int xCount = xSeg + 1;                      // vertices per row
    int yCount = ySeg + 1;                      // vertices per column

    verts.reserve(size_t(xCount) * size_t(yCount));  
    indices.reserve(size_t(xSeg) * size_t(ySeg) * 6);

    for (int j = 0; j < yCount; ++j) {
        for (int i = 0; i < xCount; ++i) {
            float x = p.x - scl.x + dx * i;
            float y = p.y ;    
            float z = p.z + scl.y - dy * j;
            verts.push_back(V(
                x, y, z,
                0.0f, 0.1f, 0.0f,               // normal +Z
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
    update_mesh();
}
void plane::set_subdivision(math::Vec2 sub){subdivide.x = sub.x, subdivide.y = sub.y; create_plane();}