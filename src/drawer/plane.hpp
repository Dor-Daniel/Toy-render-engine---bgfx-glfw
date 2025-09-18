#pragma once

#include "../config.hpp"
#include "mesh.hpp"
#include "../components/transform.hpp"

class plane : public Mesh::mesh<PosNormColorVertex>
{
private:
    Components::transform plane_transform{math::Vec3{0}, math::Vec3{0}, math::Vec3{1}};
    math::Vec2 subdivide{200, 200};
    void update_transform();
    void update_mesh();
    void create_plane(Color col=0xffffffff);
public:
    plane(Color col);
    ~plane() = default;
    void set_transform(Components::transform _transform) { plane_transform = _transform; update_transform(); }
    Components::transform get_transform(){ return plane_transform; }
    void set_subdivision(math::Vec2 sub);
};


