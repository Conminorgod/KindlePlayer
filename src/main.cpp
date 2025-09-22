#include <SDL.h>
#include <SDL_mixer.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/gl3w.h>

#include "framework.h"
#include "playlist.h"
#include "song.h"

#include <iostream>
#include <vector>
#include <filesystem>

// required in global space for the songEnded() hook
std::vector<Song> playlist;
uint32_t curSong = 0;
Mix_Music *music;

// flags
bool manualStop = false;

// hook for when a song ends
void songEnded() {
	if (manualStop) {
		return;
	} else if (playlist[curSong].looping) {
		Mix_SetMusicPosition(0.0);
	} else if (curSong + 1 < playlist.size()) {
		curSong++;
		music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
	} else {
		return;
	}
	Mix_PlayMusic(music, 1);
}
			

int main(int, char**) {
	// create the SDL context
	SDL_Window *window = SDL_CreateWindow("ImGui + SDL2",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext gl_context;
	setup(window, gl_context);
	Mix_HookMusicFinished(songEnded); // connect the hook for when a song ends
	
	// needed to make a list of all the playlists
	std::vector<std::filesystem::path> everyPlaylist = getEveryExt("assets/playlists/", ".json");

	// variables for the shuffle feature
	bool shuffle = false;
	std::vector<Song> copiedPlaylist;
	
	// random runtime variables
	bool running = true;
	char inputBuffer[256] = "";
	char playlistName[256] = "";
	SDL_Event e;
	int fb_w, fb_h;
	SDL_GL_GetDrawableSize(window, &fb_w, &fb_h);

	// main loop
	while (running) {
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT) running = false;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		showDockSpace();

		// Song controls
		ImGui::Begin("Kindle Player");

		if (ImGui::Button("Start Music")) {
			Mix_PlayMusic(music, 1);
			manualStop = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop Music")) {
			manualStop = true;
			Mix_HaltMusic();
		}

		if (ImGui::Button("Previous Song") && curSong != 0) {
			curSong -= 1;
			music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
			
			Mix_PlayMusic(music, 1);
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Song") && curSong + 1 < playlist.size()) {
			curSong++;
			music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
			Mix_PlayMusic(music, 1);
		}

		if (ImGui::Button("Pause")) Mix_PauseMusic();
		ImGui::SameLine();
		if (ImGui::Button("Unpause")) Mix_ResumeMusic();

		if (ImGui::Button("Loop")) {
			playlist[curSong].looping = !playlist[curSong].looping;
		}
		if (ImGui::Button("Shuffle")) {
			if (!shuffle) {
				copiedPlaylist = shufflePlaylist(playlist, curSong);
				curSong = 0;
			} else {
				curSong = playlist[curSong].index;
				playlist = copiedPlaylist;
			}
			shuffle = !shuffle;
		}
		
		/*
		if (ImGui::Button("I'm a dev and I don't want to listen to the song")) {
			Mix_HaltMusic();
		}
		*/


		ImGui::End();

		// Playlist controls
		ImGui::Begin("playlists");
		ImGui::InputText("Enter Playlist Link", inputBuffer, IM_ARRAYSIZE(inputBuffer));
		ImGui::InputText("Enter Playlist Name", playlistName, IM_ARRAYSIZE(playlistName));
		
		if (ImGui::Button("Export Playlist")) {
			std::string playlistLink(inputBuffer);
			std::string s_pName(playlistName);
			exportPlaylist(playlistLink, s_pName);
		}
		ImGui::SameLine();
		ImGui::Text("*Can take a VERY long time");
		
		ImGui::NewLine();
		ImGui::Text("Playlists");
		for (const auto &path : everyPlaylist) {
			if (ImGui::Button(path.stem().string().c_str())) {
				playlist = loadPlaylist(path);
				music = Mix_LoadMUS(playlist[0].filepath.c_str());
				if (!music) {
					std::cerr << "Failed to load music: " << Mix_GetError() << "\n";
				}
			}
		}
		ImGui::End();


		ImGui::Render();

		// I've already forgotten what these do but I think it has to do with the window
		glViewport(0, 0, fb_w, fb_h);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);
	}


	cleanup(music, gl_context, window);
	return 0;
}


