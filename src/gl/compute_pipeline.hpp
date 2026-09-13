#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>

class ComputePipeline
{
public:
    explicit ComputePipeline() = default;
    void init();
    
    void run(
        GLuint output_texture,
        GLuint voxel_texture,
        int frame,
        const glm::vec3& camera_position,
        const glm::vec3& camera_forward
    );

private:
    GLuint program = 0;
};