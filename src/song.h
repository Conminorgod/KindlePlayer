#pragma once
#include <string>

struct Song {
	std::string title;
	std::string artist;
	std::string filepath;
	uint32_t index;
	bool looping;
};

