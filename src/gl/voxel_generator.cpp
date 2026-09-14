#include "voxel_generator.hpp"
#include "shader_utils.hpp"
#include "texture.hpp"

VoxelGeneratorPipeline::VoxelGeneratorPipeline() {
    const std::string src = load_shader_source("shaders/voxel_generator.glsl");
    program = create_compute_program(src);
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

std::pair<GLuint, int> generate_voxels(std::size_t size) {
    VoxelTextureInfo texture_info(size);
    GLuint texture = create_voxel_texture(texture_info);

    VoxelGeneratorPipeline generator;
    const int occupied = generator.run(texture, size);

    return {texture, occupied};
}