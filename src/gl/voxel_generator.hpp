#pragma once
#include <glad/gl.h>
#include <cstddef>
#include <utility>

class VoxelGeneratorPipeline
{
public:
    explicit VoxelGeneratorPipeline();

    void run(GLuint voxel_texture, std::size_t size);

private:
    GLuint program;
};

// Génère une texture de voxels et retourne (texture, nombre de voxels occupés)
std::pair<GLuint, std::size_t> generate_voxels(std::size_t size);