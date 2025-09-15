#include "shader_loader.hpp"


inline std::string shaderPath(const char* stem, char stage /* 'v' or 'f' */)
{
    // forward slashes work on Windows too
    std::string p = SHADER_BIN_DIR;
    p += "/";
    p += stem;
    p += ".";
    p += stage;
    p += ".bin";
    return p;
}

bgfx::ShaderHandle shader_loader::load_and_create_shader_handle(const std::string &path){
    
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) {
        std::fprintf(stderr, "Failed opening shader: %s\n", path.c_str());
        return BGFX_INVALID_HANDLE;
    }
    const std::streamsize size = f.tellg();
    f.seekg(0, std::ios::beg);

    std::vector<uint8_t> data((size_t)size);
    if (!f.read((char*)data.data(), size)) {
        std::fprintf(stderr, "Failed reading shader: %s\n", path.c_str());
        return BGFX_INVALID_HANDLE;
    }
    const bgfx::Memory* mem = bgfx::copy(data.data(), (uint32_t)data.size());
    return bgfx::createShader(mem);
}

bgfx::ProgramHandle shader_loader::create_program_from_handles(bgfx::ShaderHandle vert_sh, bgfx::ShaderHandle frag_sh){
    if (!bgfx::isValid(vert_sh) || !bgfx::isValid(frag_sh)) {
        std::fprintf(stderr, "Shader load failed\n");
        return BGFX_INVALID_HANDLE;
    }
    return bgfx::createProgram(vert_sh, frag_sh, /*destroyShaders=*/true);

}

bgfx::ProgramHandle shader_loader::load_and_create(const std::string &vert_name, const std::string &frag_name){


    const std::string vert_path = shaderPath(vert_name.c_str(), 'v');
    const std::string frag_path = shaderPath(frag_name.c_str(), 'f');
    
    return create_program_from_handles(
        load_and_create_shader_handle(vert_path),
        load_and_create_shader_handle(frag_path)
    );
}