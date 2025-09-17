#include "cube.hpp"

cube::cube(){
    
    float s = cube_transform.get_scale().x /2;
    // half-size s
    auto V = [](float x,float y,float z, float nx,float ny,float nz, uint32_t c){
        return vertex_info{x,y,z, nx,ny,nz, c};
    };
    Color col = 0xff00ff00;
    std::vector<vertex_info> verts = {
        // +Z (front)
        V(-s,+s,+s, 0, 0, 1, col), V(+s,+s,+s, 0,0, 1, col),
        V(-s,-s,+s, 0, 0, 1, col), V(+s,-s,+s, 0,0, 1, col),
        // -Z (back)
        V(-s,+s,-s, 0, 0,-1, col), V(+s,+s,-s, 0,0,-1, col),
        V(-s,-s,-s, 0, 0,-1, col), V(+s,-s,-s, 0,0,-1, col),
        // +X (right)
        V(+s,+s,+s, 1, 0, 0, col), V(+s,+s,-s, 1,0, 0, col),
        V(+s,-s,+s, 1, 0, 0, col), V(+s,-s,-s, 1,0, 0, col),
        // -X (left)
        V(-s,+s,+s,-1, 0, 0, col), V(-s,+s,-s,-1,0, 0, col),
        V(-s,-s,+s,-1, 0, 0, col), V(-s,-s,-s,-1,0, 0, col),
        // +Y (top)
        V(-s,+s,+s, 0, 1, 0, col), V(+s,+s,+s, 0,1, 0, col),
        V(-s,+s,-s, 0, 1, 0, col), V(+s,+s,-s, 0,1, 0, col),
        // -Y (bottom)
        V(-s,-s,+s, 0,-1, 0, col), V(+s,-s,+s, 0,-1,0, col),
        V(-s,-s,-s, 0,-1, 0, col), V(+s,-s,-s, 0,-1,0, col),
    };

    std::vector<uint16_t> idx = {
        0,1,2, 1,3,2,     // +Z
        4,6,5, 5,6,7,     // -Z
        8,9,10, 9,11,10,  // +X
        12,14,13, 13,14,15, // -X
        16,18,17, 17,18,19, // +Y
        20,21,22, 21,23,22  // -Y
    };


    set_vertices_data(std::move(verts));
    set_triangles_data(std::move(idx));
    //set_lines_data(lines);
}

void cube::update_mesh() {
     if (!si) return;
    const math::Vec3 pos = cube_transform.get_position();
    const math::Vec3 scl = cube_transform.get_scale();      // half-extents
    const math::Vec3 rot = cube_transform.get_rotation();   // radians (X,Y,Z)

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


