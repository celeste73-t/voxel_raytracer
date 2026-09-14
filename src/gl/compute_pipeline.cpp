#include "compute_pipeline.hpp"
#include "shader_utils.hpp"
#include "../constants.hpp"

void ComputePipeline::init() {
    const std::string src = load_shader_source("shaders/raytracer.glsl");
    program = create_compute_program(src);

    voxel_loc = glGetUniformLocation(program, "voxel_texture");
    light_count_loc = glGetUniformLocation(program, "light_count");
    frame_loc = glGetUniformLocation(program, "frame");
    pos_loc = glGetUniformLocation(program, "camera_position");
    fwd_loc = glGetUniformLocation(program, "camera_forward");

}

void ComputePipeline::run(
    GLuint output_texture,
    GLuint voxel_texture,
    GLuint light_buffer,
    int frame,
    const glm::vec3& camera_position,
    const glm::vec3& camera_forward
) {
    glUseProgram(program);

    glBindImageTexture(0, output_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, voxel_texture);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, light_buffer);
    if (voxel_loc != -1) {
        glUniform1i(voxel_loc, 1);
    }
    if (light_count_loc != -1) {
        glUniform1i(light_count_loc, LIGHT_NUMBER);
    }

    if (frame_loc != -1) {
        glUniform1i(frame_loc, frame);
    }

    if (pos_loc != -1) {
        glUniform3f(pos_loc, camera_position.x, camera_position.y, camera_position.z);
    }

    if (fwd_loc != -1) {
        glUniform3f(fwd_loc, camera_forward.x, camera_forward.y, camera_forward.z);
    }

    const GLuint gx = (DISPLAY_WIDTH + 7) / 8;
    const GLuint gy = (DISPLAY_HEIGHT + 7) / 8;

    glDispatchCompute(gx, gy, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}