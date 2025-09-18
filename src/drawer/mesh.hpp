#pragma once

#include "../config.hpp"
#include "../shader/shader.hpp"


namespace Mesh {

enum MeshType {
    MESH_TYPE_VERTEX,
    MESH_TYPE_TRIANGLE,
    MESH_TYPE_LINES
};

template<typename T>
class mesh {
public:
    mesh()
        : num_verts{0}, num_triangles{0}, num_lines{0},
          size_of_triangles_buff{0}, size_of_vertices_buff{0}, size_of_lines_buuf{0} {}

    void* operator[](MeshType type);

    void create_renderer(const std::string& vert_name,
                         const std::string& frag_name,
                         uint64_t state = Shader::default_state);

    void set_vertices_data(std::vector<T> _verts);
    void set_triangles_data(std::vector<uint16_t> _triangles);
    void set_lines_data(std::vector<uint16_t> _lines);

    size_t num_verts;
    size_t num_triangles;
    size_t num_lines;
    size_t size_of_triangles_buff;
    size_t size_of_vertices_buff;
    size_t size_of_lines_buuf;

protected:
    std::shared_ptr<Shader::ShaderInfo> si;
    std::vector<uint16_t> lines;
    std::vector<uint16_t> triangles;
    std::vector<T>        pos_color_vertex;   // <-- remove 'template<typename T>' here
};

// ---------- inline template method definitions ----------

template<typename T>
inline void* mesh<T>::operator[](MeshType type) {
    return type == MESH_TYPE_VERTEX   ? static_cast<void*>(pos_color_vertex.data())
         : type == MESH_TYPE_TRIANGLE ? static_cast<void*>(triangles.data())
                                      : static_cast<void*>(lines.data());
}

template<typename T>
inline void mesh<T>::set_vertices_data(std::vector<T> _verts) {
    pos_color_vertex = std::move(_verts);
    num_verts = pos_color_vertex.size();
    size_of_vertices_buff = num_verts * sizeof(T);
}

template<typename T>
inline void mesh<T>::set_triangles_data(std::vector<uint16_t> _triangles) {
    triangles = std::move(_triangles);
    num_triangles = triangles.size() / 3;
    size_of_triangles_buff = triangles.size() * sizeof(uint16_t);
}

template<typename T>
inline void mesh<T>::set_lines_data(std::vector<uint16_t> _lines) {
    lines = std::move(_lines);
    num_lines = lines.size() / 2;
    size_of_lines_buuf = lines.size() * sizeof(uint16_t);
}

template<typename T>
inline void mesh<T>::create_renderer(const std::string& vert_name,
                                     const std::string& frag_name,
                                     uint64_t state)
{
    auto& shader_renderer = Shader::shader::Instance();

    bgfx::VertexLayout layout;
    T::init(layout); // vertex type T defines the layout

    Shader::ProgramType pt = shader_renderer.add_program(vert_name, frag_name);
    if (!si) si = std::make_shared<Shader::ShaderInfo>(pt);
    else     si->program_type = pt;
    si->si_name = state == Shader::line_state ? "axis" : "cube";
    if (!pos_color_vertex.empty())
        si->set_vertex_data(pos_color_vertex.data(), size_of_vertices_buff, layout);

    if (!triangles.empty())
        si->add_index_data(triangles.data(), size_of_triangles_buff);

    if (!lines.empty())
        si->add_index_data(lines.data(), size_of_lines_buuf);

    si->set_state(state);
    shader_renderer.add_shader_info(si);
}


} // namespace Mesh
