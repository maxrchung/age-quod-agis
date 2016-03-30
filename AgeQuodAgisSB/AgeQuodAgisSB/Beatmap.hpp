#ifndef BEATMAP_HPP
#define BEATMAP_HPP

#include <vector>
#include "Note.hpp"
#include "Hold.hpp"

class Beatmap {
public:
	static Beatmap* Instance();
	std::vector<Note*> notes;
	std::vector<Hold*> holds;
	void ParseHitObjects(std::string& beatmapPath);
private:
	Beatmap() {};
	Beatmap(const Beatmap&) {};
	Beatmap& operator=(const Beatmap&) {};
	static Beatmap* instance;
};

#endif//BEATMAP_HPP