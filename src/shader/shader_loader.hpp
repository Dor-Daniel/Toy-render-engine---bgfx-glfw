#pragma once
#include "../config.hpp"

#ifndef SHADER_BIN_DIR
#define SHADER_BIN_DIR "."
#endif

class shader_loader{
    public:
        static bgfx::ProgramHandle load_and_create(const std::string &vert_name, const std::string &frag_name);
        shader_loader() = default;
        ~shader_loader() = default;
    private:
        static bgfx::ShaderHandle load_and_create_shader_handle(const std::string &path);
        static bgfx::ProgramHandle create_program_from_handles(bgfx::ShaderHandle vert_sh, bgfx::ShaderHandle frag_sh);
};