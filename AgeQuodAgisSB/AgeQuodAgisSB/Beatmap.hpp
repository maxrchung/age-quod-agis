#ifndef BEATMAP_HPP
#define BEATMAP_HPP

#include <vector>
#include "Note.hpp"

class Beatmap {
public:
	static Beatmap* Instance();
	std::vector<Note*> notes;
	void ParseHitObjects(const std::string& beatmapPath);
private:
	Beatmap() {};
	Beatmap(const Beatmap&) {};
	Beatmap& operator=(const Beatmap&) {};
	static Beatmap* instance;
};

#endif//BEATMAP_HPP