#pragma once
#include <glad/gl.h>
#include <cstddef>

struct VoxelTextureInfo
{
    std::size_t width;
    std::size_t height;
    std::size_t depth;

    explicit VoxelTextureInfo(std::size_t size)
        : width(size), height(size), depth(size) {}
};

GLuint create_storage_texture(int width, int height);
GLuint create_voxel_texture(const VoxelTextureInfo& texture);
