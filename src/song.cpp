#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl3w.h>
#include "song.h"

SDL_Surface *surface;

// changes the currently loaded image to the current song's thumbnail
GLuint changeThumbnail(const std::string &path) {
	surface = IMG_Load(path.c_str());
	if (!surface) { SDL_Log("IMG_Load: %s", SDL_GetError()); return 0; }
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);

	return tex;
}
