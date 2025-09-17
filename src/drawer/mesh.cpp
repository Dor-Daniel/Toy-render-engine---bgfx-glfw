#include "mesh.hpp"
#include "../shader/shader.hpp"

namespace Mesh{
        
        void mesh::set_vertices_data(std::vector<vertex_info> _pos_color_vertex){
            pos_color_vertex = _pos_color_vertex;
            num_verts = pos_color_vertex.size();
            size_of_vertices_buff = num_verts * sizeof(vertex_info);
        }
        void mesh::set_triangles_data(std::vector<uint16_t> _triangles){
            triangles = _triangles;
            num_triangles =(size_t)(triangles.size() / 3.0f);
            size_of_triangles_buff = triangles.size() * sizeof(triangles_info);
        }
        void mesh::set_lines_data(std::vector<uint16_t> _lines){
            lines = _lines;
            num_lines =(size_t)(lines.size() / 2.0f);
            size_of_lines_buuf = lines.size() * sizeof(uint16_t);
        }

        void* mesh::operator[](MeshType type){ 
            return type == MESH_TYPE_VERTEX ? (void*) pos_color_vertex.data() 
                : type == MESH_TYPE_TRIANGLE ? (void*) triangles.data() 
                : (void*) lines.data(); 
        }
        void mesh::create_renderer(const std::string &vert_name, const std::string &frag_name, uint64_t state){
            auto& shader_renderer = Shader::shader::Instance();
            bgfx::VertexLayout s_layout;

            s_layout.begin(bgfx::getRendererType());
            s_layout.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
            s_layout.add(bgfx::Attrib::Normal,3,bgfx::AttribType::Float);
            s_layout.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
            s_layout.end();
            Shader::ProgramType pt = shader_renderer.add_program(vert_name, frag_name); // we need program id in order to init the shader info
            
            if(!si)si = std::make_shared<Shader::ShaderInfo>(pt);
            else si->program_type = pt;

            if(!pos_color_vertex.empty())
                si->set_vertex_data(pos_color_vertex.data(), size_of_vertices_buff, s_layout);
            if(!triangles.empty())
                si->add_index_data((triangles_info*)triangles.data(),size_of_triangles_buff);
            if(!lines.empty())
                si->add_index_data((uint16_t*)lines.data(), size_of_lines_buuf);
                
            si->set_state(state);

            shader_renderer.add_shader_info(si);
        }

        void Mesh::mesh::update_shader_info_vertices_data(){
             bgfx::VertexLayout s_layout;

            s_layout.begin(bgfx::getRendererType());
            s_layout.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
            //if(typeid(vertex_info)== typeid(PosNormColorVertex)) 
            s_layout.add(bgfx::Attrib::Normal,3,bgfx::AttribType::Float);
            s_layout.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
            s_layout.end();
            si->set_vertex_data(pos_color_vertex.data(), size_of_vertices_buff, s_layout);
        };

    }