#include "voxel_generator.hpp"
#include "shader_utils.hpp"
#include "texture.hpp"
#include "../constants.hpp"
#include <chrono>

VoxelGeneratorPipeline::VoxelGeneratorPipeline() {
    const std::string src = load_shader_source("shaders/voxel_generator.glsl");
    program = create_compute_program(src);

    const std::string light_src = load_shader_source("shaders/light_generator.glsl");
    light_program = create_compute_program(light_src);
}

int VoxelGeneratorPipeline::run(GLuint voxel_texture, std::size_t size) {
    GLuint counter_buffer = 0;
    glGenBuffers(1, &counter_buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counter_buffer);
    const GLuint initial_count = 0;
    glBufferData(
        GL_ATOMIC_COUNTER_BUFFER,
        sizeof(initial_count),
        &initial_count,
        GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, counter_buffer);

    glUseProgram(program);
    glBindImageTexture(0, voxel_texture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);

    const GLuint groups = static_cast<GLuint>((size + 7) / 8);
    glDispatchCompute(groups, groups, groups);
    glMemoryBarrier(
        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
        GL_TEXTURE_FETCH_BARRIER_BIT |
        GL_ATOMIC_COUNTER_BARRIER_BIT
    );

    GLuint occupied = 0;
    glGetBufferSubData(
        GL_ATOMIC_COUNTER_BUFFER,
        0,
        sizeof(occupied),
        &occupied
    );

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, 0);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    glDeleteBuffers(1, &counter_buffer);

    return static_cast<int>(occupied);
}

GLuint VoxelGeneratorPipeline::generate_lights(GLuint texture) {
    GLuint light_buffer = 0;
    glGenBuffers(1, &light_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_buffer);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        static_cast<GLsizeiptr>(LIGHT_NUMBER * sizeof(float) * 4),
        nullptr,
        GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, light_buffer);

    glUseProgram(light_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texture);

    const GLint voxel_loc = glGetUniformLocation(light_program, "voxel_texture");
    const GLint count_loc = glGetUniformLocation(light_program, "light_count");
    const GLint seed_loc = glGetUniformLocation(light_program, "seed");
    if (voxel_loc != -1) {
        glUniform1i(voxel_loc, 0);
    }
    if (count_loc != -1) {
        glUniform1i(count_loc, LIGHT_NUMBER);
    }
    if (seed_loc != -1) {
        const auto seed = static_cast<GLuint>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
        glUniform1ui(seed_loc, seed);
    }

    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return light_buffer;
}

VoxelGenerationResult generate_voxels(std::size_t size) {
    VoxelTextureInfo texture_info(size);
    GLuint texture = create_voxel_texture(texture_info);

    VoxelGeneratorPipeline generator;
    const int occupied = generator.run(texture, size);
    const GLuint light_buffer = generator.generate_lights(texture);

    return {texture, light_buffer, occupied};
}