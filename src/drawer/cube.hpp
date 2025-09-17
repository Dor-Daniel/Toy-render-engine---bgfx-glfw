#pragma once

#include "../config.hpp"
#include "mesh.hpp"
#include "../components/transform.hpp"

class cube : public Mesh::mesh
{
private:
    Components::transform cube_transform;
    void update_mesh();
public:
    cube();
    ~cube() = default;
    void set_transform(Components::transform _transform) { cube_transform = _transform; update_mesh(); }
    Components::transform get_transform(){ return cube_transform; }
};


