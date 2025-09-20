#include <vector>
#include "song.h"

void savePlaylist(const std::vector<Song> &playlist, const std::string &filename);
std::vector<Song> loadPlaylist(const std::string &filename);
void exportPlaylist(const std::string &link, const std::string &name);
std::vector<Song> shufflePlaylist(std::vector<Song> &playlist, uint32_t index);
