#include "Storyboard.hpp"
#include "Sprite.hpp"
#include "Vector2.hpp"
#include "Beatmap.hpp"
#include <iostream>

int main() {
	// Path setup
	std::string beatmapDirectory = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\)";
	std::string beatmapTitle = "void - Age quod agis (TheWeirdo9)";
	std::string difficultyName = "[Age quod agis]";
	std::string beatmapPath = beatmapDirectory + beatmapTitle + " " + difficultyName + ".osu";
	std::string storyboardPath = beatmapDirectory + beatmapTitle + ".osb";

	// Calculate BPM related values

	// Parse in hit objects into notes and holds
	Beatmap::Instance()->ParseHitObjects(beatmapPath);

	// Deal with notes
	for (auto note : Beatmap::Instance()->notes) {

	}

	// Deal with holds
	for (auto hold : Beatmap::Instance()->holds) {

	}

	// Music spectrum?

	// Output to storyboard file
	Storyboard::Instance()->Write(storyboardPath);
	std::cin.get();
	return 0;
}