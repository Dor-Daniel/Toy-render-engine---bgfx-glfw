#include "shader.hpp"
#include "shader_loader.hpp"


namespace Shader {

//-------------------- ShaderInfo --------------------

ShaderInfo::ShaderInfo(ProgramType t)
: program_type(t), s_id(g_next_id++) {
    v_handle = BGFX_INVALID_HANDLE;
}

ShaderInfo::ShaderInfo(ShaderInfo&& o) noexcept
: program_type(o.program_type),
  view_id(o.view_id),
  s_v_layout(o.s_v_layout),
  v_handle(o.v_handle),
  i_handles(std::move(o.i_handles)),
  s_id(o.s_id) ,
  state(o.state){
    bx::mtxIdentity(transform_matrix);
    o.v_handle = BGFX_INVALID_HANDLE;
    o.i_handles.clear();
}

ShaderInfo& ShaderInfo::operator=(ShaderInfo&& o) noexcept {
    if (this == &o) return *this;

    // destroy ours first
    if (bgfx::isValid(v_handle)) bgfx::destroy(v_handle);
    for (auto h : i_handles) if (bgfx::isValid(h)) bgfx::destroy(h);

    program_type = o.program_type;
    view_id      = o.view_id;
    s_v_layout   = o.s_v_layout;
    v_handle     = o.v_handle;
    i_handles    = std::move(o.i_handles);
    s_id         = o.s_id;
    state        = o.state;

    o.v_handle = BGFX_INVALID_HANDLE;
    o.i_handles.clear();
    return *this;
}

ShaderInfo::~ShaderInfo() {
    if (bgfx::isValid(v_handle)) bgfx::destroy(v_handle);
    v_handle = BGFX_INVALID_HANDLE;
    for (auto h : i_handles) if (bgfx::isValid(h)) bgfx::destroy(h);
    i_handles.clear();
}



void ShaderInfo::set_view_id(viewID_t id) { view_id = id; }

void ShaderInfo::set_vertex_data(const void* data, uint32_t bytes, const bgfx::VertexLayout& layout) {
    if (bgfx::isValid(v_handle)) return; // already set
    const bgfx::Memory* mem = bgfx::copy(data, bytes);
    v_handle = bgfx::createVertexBuffer(mem, layout);
}

void ShaderInfo::add_index_data(const uint16_t* indices, uint32_t buff_size) {
    const bgfx::Memory* mem = bgfx::copy(indices, buff_size);
    i_handles.push_back(bgfx::createIndexBuffer(mem));
}

//-------------------- Shader manager --------------------

shader& shader::Instance() {
    static shader inst;
    return inst;
}

ProgramType shader::add_program(const std::string& vert_stem, const std::string& frag_stem) {
    bgfx::ProgramHandle ph = shader_loader::load_and_create(vert_stem, frag_stem);
    if (!bgfx::isValid(ph)) {
        throw std::runtime_error("Shader::add_program: failed to create program");
    }
    programs_.push_back(ph);
    return static_cast<ProgramType>(programs_.size() - 1);
}

void shader::add_shader_info(ShaderInfo&& info) {
    const auto idx = static_cast<size_t>(info.program_type);
    if (idx >= programs_.size()) {
        throw std::runtime_error("Shader::add_shader_info: unknown program_type");
    }
    shader_infos_.emplace_back(std::move(info));
}
void shader::update() {


    for (auto& si : shader_infos_) {
        if (!bgfx::isValid(si.v_handle)) continue;

        bgfx::setVertexBuffer(0, si.v_handle);
        for (auto ib : si.i_handles) {
            if (!bgfx::isValid(ib)) continue;
            bgfx::setTransform(si.transform_matrix);
            bgfx::setIndexBuffer(ib);
            bgfx::setState(std::move(si.state));
            const auto prog = programs_[static_cast<size_t>(si.program_type)];
            bgfx::submit(si.view_id, prog);
        }
    }
}

shader::~shader()= default;

void shader::cleanup(){
    for (auto ph : programs_) 
        if (bgfx::isValid(ph)) bgfx::destroy(ph);

    shader_infos_.clear();
    programs_.clear();
}

} // namespace Shader
