#include <SDL.h>
#include <SDL_mixer.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/gl3w.h>

#include "framework.h"
#include "playlist.h"
#include "song.h"
#include "ui.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <cmath>

// required in global space for the songEnded() hook
std::vector<Song> playlist;
uint32_t curSong = 0;
Mix_Music *music;
GLuint textureID;

// flags
bool newThumbnail = false;

// hook for when a song ends
void songEnded() {
	if (playlist[curSong].looping) {
		Mix_SetMusicPosition(0.0);
	} else if (curSong + 1 < playlist.size()) {
		curSong++;
		music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
		newThumbnail = true;
	} else {
		return;
	}
	Mix_PlayMusic(music, 1);
}
			

int main(int, char**) {
	// create the SDL context
	SDL_Window *window = SDL_CreateWindow("ImGui + SDL2",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_GLContext gl_context;
	SDL_Surface *surface;
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
	bool showExportWindow = false;
	bool paused = false;
	SDL_Event e;
	int display_w, display_h;

	// main loop
	while (running) {
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT) running = false;
		}

		SDL_GL_GetDrawableSize(window, &display_w, &display_h);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		showDockSpace();

		if (newThumbnail) {
			textureID = changeThumbnail(playlist[curSong].thumbnail);
			newThumbnail = false;
		}

		// Song controls
		ImGui::Begin("Kindle Player");

		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 750.0f) * 0.5f);
		if (ImGui::Button(u8"\uf04ae", ImVec2(50.0f, 20.f)) && curSong != 0) {
			curSong -= 1;
			music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
			textureID = changeThumbnail(playlist[curSong].thumbnail);
			
			Mix_PlayMusic(music, 1);
		}

		ImGui::SameLine();
		if (!paused) {
			if (ImGui::Button(u8"\uf04c", ImVec2(50.0f, 20.0f))) {
				Mix_PauseMusic();
				paused = true;
			}
		} else {
			if (ImGui::Button(u8"\uf04b", ImVec2(50.0f, 20.0f))) {
				Mix_ResumeMusic();
				paused = false;
			}
		}
		
		std::string currentTime = convertTimeToString(Mix_GetMusicPosition(music));
		std::string maxTime = convertTimeToString(Mix_MusicDuration(music));
		std::string currentTimeOverMax = currentTime + "/" + maxTime;

		ImGui::SameLine();
		ImGui::ProgressBar(Mix_GetMusicPosition(music)/Mix_MusicDuration(music), ImVec2(500, 0), "");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::CalcTextSize(currentTimeOverMax.c_str()).x - 8);
		ImGui::Text(currentTimeOverMax.c_str());
		ImGui::SameLine();

		if (ImGui::Button(u8"\uf04e", ImVec2(50.0f, 20.0f)) && curSong + 1 < playlist.size()) {
			curSong++;
			music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
			textureID = changeThumbnail(playlist[curSong].thumbnail);
			Mix_PlayMusic(music, 1);
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"\uf021", ImVec2(50.0f, 20.0f))) {
			playlist[curSong].looping = !playlist[curSong].looping;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"\uf074", ImVec2(50.0f, 20.0f))) {
			if (!shuffle) {
				copiedPlaylist = shufflePlaylist(playlist, curSong);
				curSong = 0;
			} else {
				curSong = playlist[curSong].index;
				playlist = copiedPlaylist;
			}
			shuffle = !shuffle;
		}
		
		if (ImGui::Button("I'm a dev and I don't want to listen to the song")) {
			Mix_HaltMusic();
		}

		ImGui::End();

		// Playlist controls
		ImGui::Begin("Playlists"); 
		if (ImGui::Button("Export")) {
			showExportWindow = true;	
		}
		if (showExportWindow) {
			ImGui::Begin("Exporting", &showExportWindow, ImGuiWindowFlags_NoDocking);
			ImGui::InputText("Enter Playlist Link", inputBuffer, IM_ARRAYSIZE(inputBuffer));
			ImGui::InputText("Enter Playlist Name", playlistName, IM_ARRAYSIZE(playlistName));
			
			if (ImGui::Button("Export Playlist")) {
				std::string playlistLink(inputBuffer);
				std::string s_pName(playlistName);
				exportPlaylist(playlistLink, s_pName);
				everyPlaylist = getEveryExt("assets/playlists/", ".json");
			}
			ImGui::SameLine();
			ImGui::Text("*Can take a long time");
			ImGui::End();
		}
		ImGui::NewLine();
		ImGui::Text("Playlists");
		for (const auto &path : everyPlaylist) {
			if (ImGui::Button(path.stem().string().c_str(), ImVec2(150.0f, 20.0f))) {
				playlist = loadPlaylist(path);
			}
		}
		ImGui::End();

		// Songs
		ImGui::Begin("Song List"); 
		if (!playlist.empty()) {
			for (uint32_t i = 0; i < playlist.size(); i++) {
				Song song = playlist[i];
				std::string shortText = truncateText(song.title, 150.0f);
				if (song.title == playlist[curSong].title && music) {
					ImGui::Text(shortText.c_str());
				} else if (ImGui::Button(shortText.c_str(), ImVec2(150.0f, 30.0f))) {
					curSong = i;
					music = Mix_LoadMUS(song.filepath.c_str());
					textureID = changeThumbnail(playlist[curSong].thumbnail);
					Mix_PlayMusic(music, 1);
				}
			}
		}

		ImGui::End();

		// thumbnail, eventual visualizer, etc.
		ImGui::Begin("Song Viewer");
		if (textureID) {
			ImGui::Image((ImTextureID)textureID, ImGui::GetContentRegionAvail());
		}
		ImGui::End();

		ImGui::Render();

		// I've already forgotten what these do but I think it has to do with the window
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);
	}


	cleanup(music, gl_context, window);
	return 0;
}


