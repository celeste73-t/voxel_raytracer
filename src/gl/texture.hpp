#pragma once
#include <glad/gl.h>
#include <cstddef>
#include <vector>

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
std::vector<unsigned char> read_voxel_texture(GLuint texture, std::size_t size);
void upload_voxel_texture(GLuint texture, std::size_t size, const std::vector<unsigned char>& voxels);