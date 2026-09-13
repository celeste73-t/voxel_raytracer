#include "shader_utils.hpp"
#include <SDL2/SDL.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstdlib>

std::string load_shader_source(const std::string& path) {
    std::filesystem::path shader_path(path);
    std::ifstream file(shader_path);

    if (!file) {
        char* base_path = SDL_GetBasePath();
        if (base_path != nullptr) {
            shader_path = std::filesystem::path(base_path) / path;
            SDL_free(base_path);
            file.open(shader_path);
        }
    }

    if (!file) {
        throw std::runtime_error("Unable to load shader: " + shader_path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

namespace {
GLuint compile_shader(GLenum type, const std::string& src) {
    GLuint shader = glCreateShader(type);
    const char* src_ptr = src.c_str();
    glShaderSource(shader, 1, &src_ptr, nullptr);
    glCompileShader(shader);

    GLint compile_status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<char> log(log_length);
        glGetShaderInfoLog(shader, log_length, nullptr, log.data());
        std::cerr << log.data() << "\n";
        std::exit(EXIT_FAILURE);
    }

    return shader;
}
}

GLuint create_compute_program(const std::string& src) {
    GLuint shader = compile_shader(GL_COMPUTE_SHADER, src);

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    GLint link_status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<char> log(log_length);
        glGetProgramInfoLog(program, log_length, nullptr, log.data());
        std::cerr << log.data() << "\n";
        std::exit(EXIT_FAILURE);
    }

    glDeleteShader(shader);
    return program;
}

GLuint create_program(const std::string& vs_src, const std::string& fs_src) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint link_status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<char> log(log_length);
        glGetProgramInfoLog(program, log_length, nullptr, log.data());
        std::cerr << log.data() << "\n";
        std::exit(EXIT_FAILURE);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}