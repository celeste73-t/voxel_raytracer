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
        GLuint light_buffer,
        int frame,
        const glm::vec3& camera_position,
        const glm::vec3& camera_forward
    );

private:
    GLuint program = 0;

    GLint voxel_loc;
    GLint volume_size_loc;
    GLint light_count_loc;
    GLint ray_scale_loc;
    GLint inverse_output_size_loc;
    GLint camera_right_loc;
    GLint camera_up_loc;
    GLint frame_loc;
    GLint pos_loc;
    GLint fwd_loc;
};