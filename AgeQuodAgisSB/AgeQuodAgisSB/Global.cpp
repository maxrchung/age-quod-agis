#define _USE_MATH_DEFINES

#include "Global.hpp"
#include <Windows.h>
#include "Beatmap.hpp"

// Set this between 1 to 0 to indicate how much of the map to process
float debugSize = 1.0f;

// Path setup
std::string snowflakeBase = R"(Snowflakes\snowflake)";
std::string beatmapDirectory = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\)";
std::string beatmapTitle = "void - Age quod agis (TheWeirdo9)";
std::string difficultyName = "[Age quod agis]";
std::string beatmapPath = beatmapDirectory + beatmapTitle + " " + difficultyName + ".osu";
std::string storyboardPath = beatmapDirectory + beatmapTitle + ".osb";
// Due to how I analyze the music, the song must be mono/single channel
std::string songPath = R"(X:\Music\void\Age quod agis\ageQuodAgisLeftMono.wav)";
std::string snowflakeDirectory = beatmapDirectory + R"(Snowflakes\)";

// BPM
float bpm = 140.0f;
float mpb = 1 / bpm;
float spb = mpb * 60;
// ~429 ms per beat
float mspb = 1000 * spb;
int offset = mspb / 8;

// Overall timing
int songStart = 1692;
int songStartOffset = songStart - mspb;
int songEnd = 303406;
int songEndOffset = songEnd + mspb;

// Set lanes
int scaleLane = 1;
int particleLane = 2;
int colorLane = 3;
int rotateLane = 4;

// Particles
// Deque because we need to remove front particles if there're too many
std::deque<Sprite*> particles;
int particleCount = 1;
float particleDistance = 500.0f;
float particleBuffer = 40.0f;
float particleScale = 0.03f;
float particleOpacityMin = 0.1f;
float particleOpacityMax = 1.0f;
int particleFadeOut = mspb * 8;
int particleFadeIn = mspb;
int particleFrequency = 2 * mspb;
int particleRotateAmount = 10;

// Other
Vector2 midpoint(320, 240);
float scaleUp = 1.1f;
// In degrees
int rotateAmount = 30;
float centerpieceScale = 0.15f;

// Setup background
std::string backgroundFileName = "blank.png";
Sprite* background = new Sprite(backgroundFileName, midpoint, Layer::Background);

// Setup centerpiece
std::string centerpieceFileName = "Snowflakes/centerpiece.png";
Sprite* centerpiece = new Sprite(centerpieceFileName, midpoint);

// Color info
int frontColorMin = 100;
int frontColorMax = 155;
int backColorMin = 200;
int backColorMax = 255;
// How long before color changes from max to min or vice versa
int colorDuration = mspb * 8;


// Gets the number of snowflakes we can work with by counting the number
// of snowflake images in a directory
int getSnowflakeCount(std::string inputDirectory) {
	WIN32_FIND_DATAA fd;
	std::string inputDirectorySearch = inputDirectory + "snowflake*.png";
	HANDLE hFind = FindFirstFileA(inputDirectorySearch.c_str(), &fd);
	int numSnowflakes = 0;

	do {
		++numSnowflakes;
	} while (FindNextFileA(hFind, &fd));
	FindClose(hFind);

	return numSnowflakes;
}

int snowflakeCount = 0;
// Randomly picks a new snowflake
// Previous snowflake is tracked so we don't repick it
int previousSnowflake = 0;
std::string getSnowflake() {
	int suffix;

	do {
		suffix = rand() % snowflakeCount + 1;
	} while (previousSnowflake == suffix);
	previousSnowflake = suffix;

	return snowflakeBase + std::to_string(suffix);
}

// Convert degrees to radians
float dToR(int degrees) {
	return degrees * M_PI / 180.0f;
}

// Returns the timing of the specified lane from an index
int getNextLane(int lane, int index) {
	for (int i = index + 1; i < Beatmap::Instance()->notes.size(); ++i) {
		if (Beatmap::Instance()->notes[i]->lane == lane) {
			return Beatmap::Instance()->notes[i]->start - offset;
		}
	}
	return songEndOffset;
}