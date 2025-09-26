#pragma once
#include <vector>
#include "song.h"
#include <filesystem>

void savePlaylist(const std::vector<Song> &playlist, const std::string &filename);
std::vector<Song> loadPlaylist(const std::string &filename);
void exportPlaylist(const std::string &link, const std::string &name);
std::vector<std::filesystem::path> getEveryExt(const std::string &path, const std::string &extension);
std::vector<Song> shufflePlaylist(std::vector<Song> &playlist, uint32_t index);
