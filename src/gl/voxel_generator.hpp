#pragma once
#include <glad/gl.h>
#include <cstddef>
#include <utility>

struct VoxelGenerationResult {
    GLuint voxel_texture = 0;
    GLuint light_buffer = 0;
    int occupied_voxels = 0;
};

class VoxelGeneratorPipeline
{
public:
    explicit VoxelGeneratorPipeline();

    int run(GLuint voxel_texture, std::size_t size);
    GLuint generate_lights(GLuint voxel_texture);

private:
    GLuint program = 0;
    GLuint light_program = 0;
};

VoxelGenerationResult generate_voxels(std::size_t size);