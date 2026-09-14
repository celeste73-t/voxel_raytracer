#pragma once
#include <glad/gl.h>
#include <cstddef>
#include <utility>

class VoxelGeneratorPipeline
{
public:
    explicit VoxelGeneratorPipeline();

    int run(GLuint voxel_texture, std::size_t size);

private:
    GLuint program;
};

// Génère la texture sur le GPU et retourne le nombre occupé via un seul int GPU -> CPU.
std::pair<GLuint, int> generate_voxels(std::size_t size);