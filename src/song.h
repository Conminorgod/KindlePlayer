#pragma once
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl3w.h>

extern SDL_Surface *surface;

struct Song {
	std::string title;
	std::string artist;
	std::string filepath;
	std::string thumbnail;
	uint32_t index;
	bool looping;
};

extern GLuint changeThumbnail(const std::string &path);
