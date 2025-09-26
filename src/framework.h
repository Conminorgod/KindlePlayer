#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

ImGuiIO Imio;
void setup(SDL_Window *window, SDL_GLContext &gl_context);
void showDockSpace();
void cleanup(Mix_Music *music, SDL_GLContext gl_context, SDL_Window *window);
