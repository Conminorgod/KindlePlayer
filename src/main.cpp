#include <SDL.h>
#include <SDL_mixer.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/gl3w.h>

#include "framework.h"
#include "playlist.h"

#include <iostream>
#include <vector>

int main(int, char**) {
	SDL_Window *window = SDL_CreateWindow("ImGui + SDL2",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext gl_context;
	setup(window, gl_context);

	std::string playlistPath = "assets/playlists/test.txt";
	std::vector<std::string> playlist = loadPlaylist(playlistPath);
	for (std::string song : playlist) {
		std::cout << song << "\n";
	}

	Mix_Music* music = Mix_LoadMUS(playlist[0].c_str());
	if (!music) {
		std::cerr << "Failed to load music: " << Mix_GetError() << "\n";
	}
	
	bool running = true;
	SDL_Event e;
	int fb_w, fb_h;
	SDL_GL_GetDrawableSize(window, &fb_w, &fb_h);
	while (running) {
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT) running = false;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// UI
		ImGui::Begin("Kindle Player");
		if (ImGui::Button("Start Music")) Mix_PlayMusic(music, -1);
		if (ImGui::Button("Pause")) Mix_PauseMusic();
		if (ImGui::Button("Unpause")) Mix_ResumeMusic();
		if (ImGui::Button("Next Song")) {
			Mix_HaltMusic();
			music = Mix_LoadMUS(playlist[1].c_str());
			Mix_PlayMusic(music, -1);
		}
		if (ImGui::Button("Stop Music")) Mix_HaltMusic();
		ImGui::End();

		ImGui::Render();
		glViewport(0, 0, fb_w, fb_h);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);
	}


	cleanup(music, gl_context, window);
	return 0;
}


