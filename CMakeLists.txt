cmake_minimum_required(VERSION 3.20)
project(KindlePlayer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/sdl)
add_subdirectory(extern/sdl_mixer)

file(GLOB IMGUI_SRC
	extern/imgui/*.cpp
	extern/imgui/backends/imgui_impl_sdl2.cpp
	extern/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC extern/imgui extern/imgui/backends)
target_link_libraries(imgui PUBLIC SDL2 OpenGL::GL)

add_library(gl3w extern/gl3w/src/gl3w.c)
target_include_directories(gl3w PUBLIC extern/gl3w/include)

file(MAKE_DIRECTORY build/assets)
file(MAKE_DIRECTORY build/assets/audio)
file(MAKE_DIRECTORY build/assets/images)
file(MAKE_DIRECTORY build/assets/playlists)
file(MAKE_DIRECTORY temp)

file(GLOB SOURCES "src/*.cpp")
add_executable(KindlePlayer ${SOURCES})
target_include_directories(KindlePlayer
	PRIVATE
	${CMAKE_SOURCE_DIR}/extern/sdl/include
	${CMAKE_SOURCE_DIR}/extern/sdl_mixer/include
)
target_link_libraries(KindlePlayer PRIVATE imgui SDL2 SDL2main SDL2_mixer gl3w OpenGL::GL)
target_compile_definitions(KindlePlayer PRIVATE IMGUI_IMPL_OPENGL_LOADER_GL3W)
find_package(OpenGL REQUIRED)
target_link_libraries(KindlePlayer PRIVATE OpenGL::GL)
