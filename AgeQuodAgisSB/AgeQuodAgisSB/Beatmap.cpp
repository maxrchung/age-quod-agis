#include "Beatmap.hpp"
#include "Hold.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

Beatmap* Beatmap::instance = NULL;

Beatmap* Beatmap::Instance() {
	if (!instance) {
		instance = new Beatmap();
	}
	return instance;
}

void Beatmap::ParseHitObjects(std::string& beatmapPath) {
	std::ifstream beatmapFile;
	beatmapFile.open(beatmapPath);

	std::string line;
	while (std::getline(beatmapFile, line)) {
		if (line == "[HitObjects]") {
			break;
		}
	}

	while (std::getline(beatmapFile, line)) {
		std::istringstream inputLine(line);

		std::vector<std::string> format;
		std::string token;
		while (std::getline(inputLine, token, ',')) {
			format.push_back(token);
		}

		int x = std::stoi(format[0]);
		int y = std::stoi(format[1]);
		int timing = std::stoi(format[2]);
		int type = std::stoi(format[3]);

		// 4K lane mapping
		// 0-127:	Lane 1
		// 128-255: Lane 2
		// 256-383: Lane 3
		// 384-511: Lane 4
		int lane = x / 128 + 1;

		if (type == 128) {
			std::istringstream options(format.back());
			int holdEnd;
			options >> holdEnd;

			notes.push_back(new Hold(lane, timing, holdEnd));
		}
		else {
			notes.push_back(new Note(lane, timing));
		}

	}
}