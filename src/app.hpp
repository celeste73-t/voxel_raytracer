#pragma once
#include <chrono>
#include <SDL2/SDL.h>
#include <glad/gl.h>
#include "control.hpp"
#include "camera.hpp"
#include "performance.hpp"
#include "gl/compute_pipeline.hpp"
#include "gl/display_pipeline.hpp"

class App
{
public:
    bool running;

    App();
    ~App();

    void update();
    void write_session_report();

private:
    std::chrono::steady_clock::duration render();

    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;

    ComputePipeline compute = ComputePipeline();
    DisplayPipeline display = DisplayPipeline();
    GLuint output_tex = 0;
    GLuint voxel_texture = 0;
    GLuint light_buffer = 0;

    Camera camera;
    Control controls;
    Performance performance;

    int frame = 0;
    int occupied_voxels = 0;
    std::chrono::steady_clock::time_point last_frame;
};