#include "compute_pipeline.hpp"
#include "shader_utils.hpp"
#include "../constants.hpp"

void ComputePipeline::init() {
    const std::string src = load_shader_source("shaders/raytracer.glsl");
    program = create_compute_program(src);

    glUseProgram(program);
    voxel_loc = glGetUniformLocation(program, "voxel_texture");
    volume_size_loc = glGetUniformLocation(program, "volume_size");
    light_count_loc = glGetUniformLocation(program, "light_count");
    ray_scale_loc = glGetUniformLocation(program, "ray_scale");
    inverse_output_size_loc = glGetUniformLocation(program, "inverse_output_size");
    camera_right_loc = glGetUniformLocation(program, "camera_right");
    camera_up_loc = glGetUniformLocation(program, "camera_up");
    frame_loc = glGetUniformLocation(program, "frame");
    pos_loc = glGetUniformLocation(program, "camera_position");
    fwd_loc = glGetUniformLocation(program, "camera_forward");

    if (volume_size_loc != -1) {
        glUniform3i(
            volume_size_loc,
            static_cast<GLint>(VOXEL_TEXTURE_SIZE),
            static_cast<GLint>(VOXEL_TEXTURE_SIZE),
            static_cast<GLint>(VOXEL_TEXTURE_SIZE)
        );
    }
    if (ray_scale_loc != -1) {
        const float half_height = tan(glm::radians(60.0f) * 0.5f);
        const float aspect = static_cast<float>(DISPLAY_WIDTH) / static_cast<float>(DISPLAY_HEIGHT);
        glUniform2f(ray_scale_loc, half_height * aspect, half_height);
    }
    if (inverse_output_size_loc != -1) {
        glUniform2f(
            inverse_output_size_loc,
            1.0f / static_cast<float>(DISPLAY_WIDTH),
            1.0f / static_cast<float>(DISPLAY_HEIGHT)
        );
    }
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

    const glm::vec3 forward = glm::normalize(camera_forward);
    const glm::vec3 world_up(0.0f, 1.0f, 0.0f);
    const glm::vec3 right = glm::normalize(glm::cross(forward, world_up));
    const glm::vec3 up = glm::normalize(glm::cross(right, forward));

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
        glUniform3f(fwd_loc, forward.x, forward.y, forward.z);
    }

    if (camera_right_loc != -1) {
        glUniform3f(camera_right_loc, right.x, right.y, right.z);
    }
    if (camera_up_loc != -1) {
        glUniform3f(camera_up_loc, up.x, up.y, up.z);
    }

    const GLuint gx = (DISPLAY_WIDTH + 7) / 8;
    const GLuint gy = (DISPLAY_HEIGHT + 7) / 8;

    glDispatchCompute(gx, gy, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}