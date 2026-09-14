#include "texture.hpp"

GLuint create_storage_texture(int width, int height) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    return tex;
}

GLuint create_voxel_texture(const VoxelTextureInfo& texture) {
    GLuint gpu_texture;
    glGenTextures(1, &gpu_texture);
    glBindTexture(GL_TEXTURE_3D, gpu_texture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage3D(
        GL_TEXTURE_3D, 0, GL_R8UI,
        static_cast<GLsizei>(texture.width),
        static_cast<GLsizei>(texture.height),
        static_cast<GLsizei>(texture.depth),
        0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr
    );

    glBindTexture(GL_TEXTURE_3D, 0);
    return gpu_texture;
}

