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
#include <thread>

// required in global space for the songEnded() hook
std::vector<Song> playlist;
uint32_t curSong = 0;
Mix_Music *music;
GLuint textureID;

// flags
bool newThumbnail = false;
bool looping = false;

// hook for when a song ends
void songEnded() {
	if (looping) {
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
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 2880, 1920,
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
	float button_w = 50.0f;
	float button_h = 30.0f;
	double currentTime, maxTime; 
	int volume = 128;
	SDL_Event e;
	int display_w, display_h;

	// main loop
	while (running) {

		currentTime = Mix_GetMusicPosition(music);
		maxTime = Mix_MusicDuration(music);

		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT) {
				running = false;
				break;
			}

			if (e.type == SDL_KEYDOWN ) {
				const SDL_Keycode key = e.key.keysym.sym;
				const SDL_Keymod mod = SDL_GetModState();

				switch (key) {
					case SDLK_SPACE:
						if (paused) {
							Mix_ResumeMusic();
							paused = false;
						} else {
							Mix_PauseMusic();
							paused = true;
						}
						break;
					case SDLK_LEFT:
						if (currentTime > 5.0) {
							Mix_SetMusicPosition(currentTime - 5.0);
						}
						break;
					case SDLK_RIGHT:
						Mix_SetMusicPosition(currentTime + 5.0);
						break;
				}
				
				if ((mod & KMOD_SHIFT) && key == SDLK_n) {
					curSong++;
					music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
					textureID = changeThumbnail(playlist[curSong].thumbnail);
					paused = false;
					looping = false;
					Mix_PlayMusic(music, 1);
					break;
				}
			}
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

		// So all the elements are centered
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - (button_w * 7.0f + 500.0f + 35.0f)) * 0.5f);

		// Previous song
		if (ImGui::Button(u8"\uf04ae", ImVec2(button_w, button_h)) && curSong != 0) {
			curSong -= 1;
			music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
			textureID = changeThumbnail(playlist[curSong].thumbnail);
			paused = false;
			looping = false;
			
			Mix_PlayMusic(music, 1);
		}


		ImGui::SameLine();

		// Pause and unpause
		if (!paused) {
			if (ImGui::Button(u8"\uf04c", ImVec2(button_w, button_h))) {
				Mix_PauseMusic();
				paused = true;
			}
		} else {
			if (ImGui::Button(u8"\uf04b", ImVec2(button_w, button_h))) {
				Mix_ResumeMusic();
				paused = false;
			}
		}


		ImGui::SameLine();
		
		// Go back five seconds
		if (ImGui::Button(u8"\uf177 5s", ImVec2(button_w, button_h)) && (currentTime - 5.0) > 0.0) {
			Mix_SetMusicPosition(currentTime - 5.0);
		}
		
		ImGui::SameLine();
		
		// The current time the song is at over the song length like "n:nn/m:mm"
		std::string currentTimeOverMax;
		if (music) {
			std::string currentTimeString = convertTimeToString(currentTime);
			std::string maxTimeString = convertTimeToString(maxTime);
			currentTimeOverMax = currentTimeString + "/" + maxTimeString;
		} else {
			currentTimeOverMax = "0:00/0:00";
		}


		ImGui::SameLine();

		// Song progress bar
		ImGui::ProgressBar(currentTime/maxTime, ImVec2(500, 0), "");
		ImGui::SameLine();

		// For the current time over max text to be inside the progress bar
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::CalcTextSize(currentTimeOverMax.c_str()).x - 8);
		ImGui::Text(currentTimeOverMax.c_str());
		ImGui::SameLine();

		// Go forward five seconds
		if (ImGui::Button(u8"5s\uf178", ImVec2(button_w, button_h))) {
			Mix_SetMusicPosition(currentTime + 5.0);
		}

		ImGui::SameLine();

		// Next song
		if (ImGui::Button(u8"\uf04e", ImVec2(button_w, button_h)) && curSong + 1 < playlist.size()) {
			curSong++;
			music = Mix_LoadMUS(playlist[curSong].filepath.c_str());
			textureID = changeThumbnail(playlist[curSong].thumbnail);
			paused = false;
			looping = false;
			Mix_PlayMusic(music, 1);
		}

		ImGui::SameLine();

		// Loop
		if (ImGui::Button(u8"\uf021", ImVec2(button_w, button_h))) {
			looping = !looping;
		}
		ImGui::SameLine();

		// Shuffle
		if (ImGui::Button(u8"\uf074", ImVec2(button_w, button_h))) {
			if (!shuffle) {
				copiedPlaylist = shufflePlaylist(playlist, curSong);
				curSong = 0;
			} else {
				curSong = playlist[curSong].index;
				playlist = copiedPlaylist;
			}
			shuffle = !shuffle;
		}

		// Volume slider
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - (button_w * 7.0f + 500.0f + 35.0f)) * 0.5f);
		ImGui::SetNextItemWidth(175);
		if (ImGui::SliderInt(u8"\uf028", &volume, 0, 128)) {
			Mix_VolumeMusic(volume);
		}
		
		// Button that stops the music and triggers the songEnded hook
		if (ImGui::Button("I'm a dev and I don't want to listen to the song")) {
			Mix_HaltMusic();
		}

		ImGui::End();

		// Playlist controls
		ImGui::Begin("Playlists"); 
		// Button to show export window
		if (ImGui::Button("Export")) {
			showExportWindow = true;	
		}

		// Export window
		if (showExportWindow) {
			ImGui::Begin("Exporting", &showExportWindow, ImGuiWindowFlags_NoDocking);

			// User inputted text for the playlist link and it's name
			ImGui::InputText("Enter Playlist Link", inputBuffer, IM_ARRAYSIZE(inputBuffer));
			ImGui::InputText("Enter Playlist Name", playlistName, IM_ARRAYSIZE(playlistName));
			
			// Exports a playlist from the given link
			if (ImGui::Button("Export Playlist")) {
				// conversion required because the input text needs a char array
				std::string playlistLink(inputBuffer);
				std::string s_pName(playlistName);

				// So exporting doesn't freeze the whole program
				std::thread([playlistLink, s_pName]{
					exportPlaylist(playlistLink, s_pName);
				}).detach();

				// reload playlist list
				everyPlaylist = getEveryExt("assets/playlists/", ".json");
			}
			ImGui::SameLine();
			ImGui::Text("*Can take a long time");
			ImGui::End();
		}
		ImGui::NewLine();

		// Section for playlist list
		ImGui::Text("Playlists");
		for (const auto &path : everyPlaylist) {
			if (ImGui::Button(path.stem().string().c_str(), ImVec2(150.0f, button_h))) {
				playlist = loadPlaylist(path);
			}
		}
		ImGui::End();

		// Song list
		ImGui::Begin("Song List"); 
		if (!playlist.empty()) {
			for (uint32_t i = 0; i < playlist.size(); i++) {
				Song song = playlist[i];
				std::string shortText = truncateText(song.title, 250.0f);
				if (song.title == playlist[curSong].title && music) {
					ImGui::Text(shortText.c_str());
				} else if (ImGui::Button(shortText.c_str(), ImVec2(250.0f, 60.0f))) {
					curSong = i;
					paused = false;
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


