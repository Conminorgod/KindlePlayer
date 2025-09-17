#include <fstream>
#include <vector>
#include <iostream>

void savePlaylist(std::vector<std::string> arr, std::string filename) {
	std::ofstream outFile(filename); 
	if (outFile.is_open()) {
		for (std::string song : arr) {
			outFile << song << "\n";
		}
		outFile.close();
	} else {
		std::cerr << "Could not save playlist " << filename;
	}
}

std::vector<std::string> loadPlaylist(const std::string &filename) {
	std::vector<std::string> arr;
	std::ifstream inFile(filename);
	std::string line;
	if (inFile.is_open()) {
		while (std::getline(inFile, line)) {
				if (!line.empty()) {
					arr.push_back(line);
				}
		}
		inFile.close();
	} else {
		std::cerr << "Could not open playlist " << filename;
	}
	return arr;
}
