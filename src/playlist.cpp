#include <fstream>
#include <vector>
#include <iostream>
#include "song.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void savePlaylist(const std::vector<Song> &playlist, const std::string &filename) {
	json jsonObject = json::array();
	for (const auto &song : playlist) {
		jsonObject.push_back({
			{"title", song.title},
			{"artist", song.artist},
			{"filepath", song.filepath}
		});
	}
	std::ofstream file(filename);
	file << jsonObject.dump(3);
}

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

			playlist.push_back(std::move(song));
		}
		file.close();
	} else {
		std::cerr << "Could not open playlist " << filename;
	}
	return playlist;
}
