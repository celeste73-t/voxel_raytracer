if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	if(DEFINED ENV{VCPKG_ROOT})
		set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")
	elseif(WIN32 AND EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
		set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")
	endif()
endif()

cmake_minimum_required(VERSION 3.25)

project(c3ddda VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32 AND EXISTS "C:/vcpkg/installed/x64-windows")
	list(PREPEND CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows")
endif()

include(FetchContent)

# ---------------------------------------------------------------------------
# Dépendances
# ---------------------------------------------------------------------------

# GLM (équivalent de glam) — header-only
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# stb_image (équivalent de imagine) — header-only, on ne prend que le header
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# FastNoiseLite (équivalent de noise) — header-only
FetchContent_Declare(
	fastnoiselite
	GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
	GIT_TAG        master
)
FetchContent_MakeAvailable(fastnoiselite)

# GLAD (équivalent de glow) — chargeur OpenGL, généré + vendored dans external/glad
add_library(glad STATIC "${PROJECT_SOURCE_DIR}/external/glad/src/gl.c")
target_include_directories(glad PUBLIC "${PROJECT_SOURCE_DIR}/external/glad/include")

# SDL2 (équivalent de beryllium/fermium)
find_package(SDL2 REQUIRED)

# ---------------------------------------------------------------------------
# Sources du projet
# ---------------------------------------------------------------------------

file(GLOB_RECURSE C3DDDA_SOURCES CONFIGURE_DEPENDS
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)

if(NOT C3DDDA_SOURCES)
	message(FATAL_ERROR "No C++ source files found in ${PROJECT_SOURCE_DIR}/src")
endif()

add_executable(c3ddda ${C3DDDA_SOURCES})

target_include_directories(c3ddda PRIVATE
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/include"
	"${stb_SOURCE_DIR}"
	"${fastnoiselite_SOURCE_DIR}"
)

if(MSVC)
	target_compile_options(c3ddda PRIVATE /W4 /permissive-)
else()
	target_compile_options(c3ddda PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Liens
# ---------------------------------------------------------------------------

target_link_libraries(c3ddda PRIVATE
	glm::glm
	glad
	SDL2::SDL2
	SDL2::SDL2main
)

if(WIN32)
	target_link_libraries(c3ddda PRIVATE opengl32)
else()
	find_package(OpenGL REQUIRED)
	target_link_libraries(c3ddda PRIVATE OpenGL::GL)
endif()

set_target_properties(c3ddda PROPERTIES
	OUTPUT_NAME "c3ddda"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

# ---------------------------------------------------------------------------
# Copie des shaders à côté de l'exécutable
# ---------------------------------------------------------------------------

add_custom_command(TARGET c3ddda POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/src/shaders"
		"$<TARGET_FILE_DIR:c3ddda>/shaders"
)