#include <iostream>
#include <glad/gl.h>
#include "app.hpp"
#include "constants.hpp"
#include "gl/texture.hpp"
#include "gl/voxel_generator.hpp"

// constructor
App::App() {
  std::cout << "1. SDL_Init...\n" << std::flush;

  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  std::cout << "2. SDL_CreateWindow...\n" << std::flush;

  window = SDL_CreateWindow(
      "OpenGL + C++",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      DISPLAY_WIDTH, DISPLAY_HEIGHT,
      SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI
  );
  if (!window) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
    std::exit(1);
  }

  std::cout << "3. SDL_GL_CreateContext...\n" << std::flush;

  gl_context = SDL_GL_CreateContext(window);

  if (!gl_context) {
    std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << "\n";
    std::exit(1);
  }
  std::cout << "4. gladLoadGL...\n";

  int gl_version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

  if (gl_version == 0) {
    std::cerr << "gladLoadGL failed to load OpenGL functions\n";
    std::exit(1);
  }

  std::cout << "OpenGL version loaded: " << GLAD_VERSION_MAJOR(gl_version)
           << "." << GLAD_VERSION_MINOR(gl_version) << "\n";

  std::cout << "5. create_storage_texture...\n";
  output_tex = create_storage_texture(DISPLAY_WIDTH, DISPLAY_HEIGHT);

  std::cout << "6. generate_voxels...\n";

  const auto [voxel_tex, generated] = generate_voxels(VOXEL_TEXTURE_SIZE);
  voxel_texture = voxel_tex;
  occupied_voxels = generated;

  std::cout << "Generated voxel texture: "
            << VOXEL_TEXTURE_SIZE << "x" << VOXEL_TEXTURE_SIZE << "x" << VOXEL_TEXTURE_SIZE
            << " (" << (VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE)
            << " voxels, " << occupied_voxels << " occupied)\n";

  compute.init();
  display.init();

  SDL_ShowWindow(window);

  running = true;
  last_frame = std::chrono::steady_clock::now();
}

App::~App() {
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

// methods
void App::update() {
  const auto frame_start = std::chrono::steady_clock::now();
  const float delta = std::chrono::duration<float>(frame_start - last_frame).count();
  last_frame = frame_start;

  running = !controls.update();
  if (!running) {
    return;
  }

  camera.update(controls, delta);

  const auto compute_time = render();
  const auto frame_time = std::chrono::steady_clock::now() - frame_start;
  performance.record_frame(frame_time, compute_time);

  frame++;
}

void App::write_session_report() {
  performance.write_report(frame, occupied_voxels);
}

std::chrono::steady_clock::duration App::render() {
  const auto compute_start = std::chrono::steady_clock::now();

  compute.run(output_tex, voxel_texture, frame, camera.position, camera.forward);

  const auto compute_time = std::chrono::steady_clock::now() - compute_start;

  display.draw(output_tex);

  SDL_GL_SwapWindow(window);

  return compute_time;
}