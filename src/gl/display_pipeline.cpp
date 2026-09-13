#include "display_pipeline.hpp"
#include "shader_utils.hpp"

void DisplayPipeline::init() {
    const std::string vs_src = load_shader_source("shaders/display.vert");
    const std::string fs_src = load_shader_source("shaders/display.frag");

    program = create_program(vs_src, fs_src);

    glGenVertexArrays(1, &vao);
}

void DisplayPipeline::draw(GLuint tex) {
    glUseProgram(program);
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLint tex_loc = glGetUniformLocation(program, "tex");
    if (tex_loc != -1) {
        glUniform1i(tex_loc, 0);
    }

    glDrawArrays(GL_TRIANGLES, 0, 3);
}