#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <iostream>
#include "song.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <random>

using json = nlohmann::json;

// When a new playlist is created, it needs to be saved to a json file
// This json file contains all of the songs' data structures
void savePlaylist(const std::vector<Song> &playlist, const std::string &filename) {
	json jsonObject = json::array();
	for (const auto &song : playlist) {
		jsonObject.push_back({
			{"title", song.title},
			{"artist", song.artist},
			{"filepath", song.filepath},
			{"thumbnail", song.thumbnail},
			{"index", song.index},
		});
	}
	std::ofstream file(filename);
	file << jsonObject.dump(6);
}

// load a playlist json file and return the array of songs
std::vector<Song> loadPlaylist(const std::string &filename) {
	std::vector<Song> playlist;

	std::ifstream file(filename);
	json jsonObject;
	file >> jsonObject;

	if (file.is_open()) {
		for (auto &item : jsonObject) {
			Song song;
			song.title = item.value("title", "");
			song.artist = item.value("artist", "");
			song.filepath = item.value("filepath", "");
			song.thumbnail = item.value("thumbnail", "");
			song.index = item.value("index", 0);

			playlist.push_back(std::move(song));
		}
		file.close();
	} else {
		std::cerr << "Could not open playlist " << filename;
	}
	return playlist;
}

// get every file with a specific extension from a specific file
std::vector<std::filesystem::path> getEveryExt(const std::string &path, const std::string &extension) {
	std::filesystem::path fPath = path;
	std::vector<std::filesystem::path> files;

	for (const auto &file : std::filesystem::directory_iterator(fPath)) {
		if (file.is_regular_file()) {
			if (file.path().extension() == extension) {
				files.push_back(file.path());
			}
		}
	}

	return files;
}

// Exports a whole youtube playlist/song from a link and saves it as a playlist
// I am going to need to multithread this at some point but I have no idea how
// to do anything with threads
void exportPlaylist(const std::string &link, const std::string &name) {
	// uses yt-dlp to download each song in the playlist as audio and also saves their thumbnail. Command varies from system to system
#ifdef _WIN32
	std::string cmd = "yt-dlp.exe -P temp/ -o %(title)s.%(ext)s -x --audio-format mp3 --audio-quality 0 --convert-thumbnails jpg --write-thumbnail " + link;
#elif defined(__linux__)
	std::string cmd = "yt-dlp -P temp/ -o \"%(title)s.%(ext)s\" -x --audio-format mp3 --audio-quality 0 --convert-thumbnails jpg --write-thumbnail " + link;
#else
	std::cout << "Feature not supported on your platform\n";
	return;
#endif
	
	int result = std::system(cmd.c_str());
	if (result != 0) {
		std::cout << "Failed to export playlist\n";
	}

	// gets every mp3 from the temp folder that each song was placed into
	std::vector<std::filesystem::path> everyPath = getEveryExt("temp/", ".mp3");

	// creates a new playlist and for every path in everyPath will create a new song entry and push it to the back
	// also moves each song to the audio folder in assets
	std::vector<Song> newPlaylist;
	uint32_t i = 0;
	for (const auto &path : everyPath) {
		Song song = { path.stem().string(), "auto", "assets/audio/" + path.filename().string(), "assets/images/" + path.stem().string() + ".jpg", i}; 
		newPlaylist.push_back(song);
		std::filesystem::rename(path, "assets/audio/" + path.filename().string());
		i++;
	}
	savePlaylist(newPlaylist, "assets/playlists/" + name + ".json");

	// gets every jpg file and moves it to the images folder in assets
	std::vector<std::filesystem::path> thumbnails = getEveryExt("temp/", ".jpg");
	for (const auto &thumbnail : thumbnails) {
		std::filesystem::rename(thumbnail, "assets/images/" + thumbnail.filename().string()); 
	}
}

// shuffles a playlist and returns a copy of the original version
std::vector<Song> shufflePlaylist(std::vector<Song> &playlist, uint32_t index) {
	std::vector<Song> copiedPlaylist = playlist;

	std::random_device rd;
	std::default_random_engine rng(rd());

	std::shuffle(playlist.begin(), playlist.end(), rng);
	playlist.insert(playlist.begin(), copiedPlaylist[index]);

	return copiedPlaylist;
}
