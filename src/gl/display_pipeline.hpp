#pragma once
#include <glad/gl.h>

class DisplayPipeline
{
public:
    DisplayPipeline() = default;
    void init();


    void draw(GLuint tex);

private:
    GLuint program = 0;
    GLuint vao = 0;
};