#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

void setup(SDL_Window *window, SDL_GLContext &gl_context);
void cleanup(Mix_Music *music, SDL_GLContext gl_context, SDL_Window *window);
