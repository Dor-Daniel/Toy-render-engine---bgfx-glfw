#pragma once
#include "../config.hpp"
#include "../time/time.hpp"

// when creating a new renderer lets say u want to create a cube so u do
/*
    cube c{}; 
    
    auto& inst = Shader::shader::Instance();
    
    s_layout.begin(bgfx::getRendererType());
    s_layout.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
    s_layout.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
    s_layout.end();
    
    Shader::ProgramType pt = inst.add_program(vert_name, frag_name); // we need program id in order to init the shader info
    Shader::ShaderInfo si(pt);
    si.set_vertex_data(c.mesh(VERTEX), c.mesh.size_of_vertices_buff, s_layout);
    si.add_index_data((triangles_info*)c.mesh(TRIANGLE),c.mesh.size_of_triangles_buff);
    inst.add_shader_info(std::move(si));

    remember u need main to include 
    // --- inside the main loop ---//
    bgfx::set...
    ...
    inst.update();
    bgfx::frame();

    // --- before bgfx::destroy --- //
    inst.cleanup();
    bgfx::destroy();

*/ 


namespace Shader {

    // used in order to maintain this code when increases and then change the type according to the needs
    using viewID_t = uint16_t;

    // inline used to suugest for the compiler to change the type at compilation
    inline uint16_t g_next_id = 0;

    // constexpr used to calculate values in compile time insted of runtime which increase preformance
    constexpr uint64_t default_state =
        BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B |
        BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
        BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA;
    constexpr uint64_t line_state = default_state | BGFX_STATE_PT_LINES;

    // Add more as you create programs
    enum ProgramType : uint16_t { BACKGROUND = 0 };

    struct ShaderInfo {
        ProgramType program_type;
        viewID_t    view_id = 0;

        bgfx::VertexLayout        s_v_layout{};
        bgfx::VertexBufferHandle  v_handle = BGFX_INVALID_HANDLE;
        std::vector<bgfx::IndexBufferHandle> i_handles;

        uint16_t s_id;
        float transform_matrix[16];

        uint64_t state = default_state;
        void set_state(uint64_t s) { state = s; }

        explicit ShaderInfo(ProgramType t); // to avoid init by ShaderInfo si = some program type; cleaner code. 

        // No copy (BGFX handles are unique)
        ShaderInfo(const ShaderInfo&) = delete;
        ShaderInfo& operator=(const ShaderInfo&) = delete;

        // Movable
        ShaderInfo(ShaderInfo&& other) noexcept;
        ShaderInfo& operator=(ShaderInfo&& other) noexcept;

        ~ShaderInfo();

        void set_view_id(viewID_t id);

        // Geometry
        void set_vertex_data(const void* data, uint32_t bytes, const bgfx::VertexLayout& layout);
        void add_index_data(const uint16_t* indices, uint32_t buff_size /*bytes*/);

    };

    class shader {
    public:
        static shader& Instance();

        shader(const shader&)            = delete;
        shader& operator=(const shader&) = delete;
        shader(shader&&)                 = delete;
        shader& operator=(shader&&)      = delete;

        // Register a program (returns its ProgramType enum value/index)
        ProgramType add_program(const std::string& vert_stem, const std::string& frag_stem);

        // Track things to draw
        std::shared_ptr<ShaderInfo> add_shader_info(std::shared_ptr<Shader::ShaderInfo> info); // returns the shared object

        // Issue draw calls
        void update();

        void cleanup();

        ~shader();

    private:
        shader() = default;

        std::vector<std::shared_ptr<ShaderInfo>> shader_infos_;
        std::vector<bgfx::ProgramHandle>     programs_;
    };

} // namespace Shader
