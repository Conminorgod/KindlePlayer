#include <vector>
#include "song.h"

void savePlaylist(const std::vector<Song> &playlist, const std::string &filename);
std::vector<Song> loadPlaylist(const std::string &filename);
