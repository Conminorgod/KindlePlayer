cmake_minimum_required(VERSION 3.20)
project(KindlePlayer LANGUAGES C CXX)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
	freetype
	GIT_REPOSITORY https://github.com/freetype/freetype.git
	GIT_TAG master
)
FetchContent_MakeAvailable(freetype)

set(SDL2MIXER_VENDORED ON CACHE BOOL "" FORCE)
add_subdirectory(extern/sdl)
add_subdirectory(extern/sdl_mixer)
add_subdirectory(extern/sdl_image)
add_subdirectory(extern/gl3w)

file(GLOB IMGUI_SRC
	extern/imgui/*.cpp
	extern/imgui/backends/imgui_impl_sdl2.cpp
	extern/imgui/backends/imgui_impl_opengl3.cpp
	extern/imgui/misc/freetype/imgui_freetype.cpp
)
add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC extern/imgui extern/imgui/backends extern/imgui/misc/freetype)
target_link_libraries(imgui PUBLIC SDL2 OpenGL::GL)
include_directories(/usr/include/freetype2)

file(MAKE_DIRECTORY build/assets)
file(MAKE_DIRECTORY build/assets/audio)
file(MAKE_DIRECTORY build/assets/images)
file(MAKE_DIRECTORY build/assets/playlists)
file(MAKE_DIRECTORY build/temp)

file(GLOB SOURCES "src/*.cpp")
add_executable(KindlePlayer ${SOURCES})
target_include_directories(KindlePlayer
	PRIVATE
	${CMAKE_SOURCE_DIR}/extern/sdl/include
	${CMAKE_SOURCE_DIR}/extern/sdl_mixer/include
	${CMAKE_SOURCE_DIR}/extern/gl3w/include
)
target_link_libraries(KindlePlayer PRIVATE imgui SDL2 SDL2main SDL2_mixer SDL2_image gl3w OpenGL::GL freetype nlohmann_json::nlohmann_json)
target_compile_definitions(KindlePlayer PRIVATE IMGUI_IMPL_OPENGL_LOADER_GL3W)
find_package(OpenGL REQUIRED)
target_link_libraries(KindlePlayer PRIVATE OpenGL::GL)
