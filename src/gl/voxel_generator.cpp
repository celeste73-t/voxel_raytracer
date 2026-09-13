#include "voxel_generator.hpp"
#include "shader_utils.hpp"
#include "texture.hpp"
#include <vector>
#include <algorithm>

VoxelGeneratorPipeline::VoxelGeneratorPipeline() {
    const std::string src = load_shader_source("shaders/voxel_generator.glsl");
    program = create_compute_program(src);
}

void VoxelGeneratorPipeline::run(GLuint voxel_texture, std::size_t size) {
    glUseProgram(program);
    glBindImageTexture(0, voxel_texture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);

    const GLuint groups = static_cast<GLuint>((size + 7) / 8);
    glDispatchCompute(groups, groups, groups);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

std::pair<GLuint, std::size_t> generate_voxels(std::size_t size) {
    VoxelTextureInfo texture_info(size);
    GLuint texture = create_voxel_texture(texture_info);

    VoxelGeneratorPipeline generator;
    generator.run(texture, size);

    // Lecture + comptage des voxels occupés + ré-upload
    // (implémentation dépendante de tes fonctions read/upload de texture)
    std::vector<unsigned char> voxels(size * size * size);
    glBindTexture(GL_TEXTURE_3D, texture);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, voxels.data());

    const std::size_t occupied = std::count_if(
        voxels.begin(), voxels.end(),
        [](unsigned char v) { return v != 0; }
    );

    glTexSubImage3D(
        GL_TEXTURE_3D, 0, 0, 0, 0,
        static_cast<GLsizei>(size), static_cast<GLsizei>(size), static_cast<GLsizei>(size),
        GL_RED_INTEGER, GL_UNSIGNED_BYTE, voxels.data()
    );

    return {texture, occupied};
}