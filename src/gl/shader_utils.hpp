#pragma once
#include <glad/gl.h>
#include <string>

std::string load_shader_source(const std::string& path);
GLuint create_compute_program(const std::string& src);
GLuint create_program(const std::string& vs_src, const std::string& fs_src);