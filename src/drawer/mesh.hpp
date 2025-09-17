#pragma once

#include "../config.hpp"
#include "../shader/shader.hpp"



namespace Mesh{

    enum MeshType{
        MESH_TYPE_VERTEX, MESH_TYPE_TRIANGLE, MESH_TYPE_LINES
    };

    class mesh{
        public:
            mesh(): num_lines{0}, num_triangles{0}, num_verts{0}{};
            void* operator[](MeshType type);
            void create_renderer(const std::string& vert_name, const std::string& frag_name, uint64_t state = Shader::default_state); 

            void set_vertices_data(std::vector<vertex_info> _pos_color_vertex);
            void set_triangles_data(std::vector<uint16_t> _triangles);
            void set_lines_data(std::vector<uint16_t> _lines);
            void set_transform_matrix(float transform_mat[16]){  }

            size_t num_verts;
            size_t num_triangles;
            size_t num_lines;
            size_t size_of_triangles_buff;
            size_t size_of_vertices_buff;
            size_t size_of_lines_buuf;
            void update_shader_info_vertices_data();
        protected:
            std::shared_ptr<Shader::ShaderInfo> si;
            std::vector<uint16_t> lines;
            std::vector<uint16_t> triangles;
            std::vector<vertex_info> pos_color_vertex;

    };
}