#include "Helpers.hpp"

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

// Colors decrease and increase by a set amount each time a certain lane is pressed
// Front colors (centerpiece + particles) are lighter, while the background is darker
int frontValueMin = 100;
int frontValueMax = 155;
int frontValue = frontValueMin;
int frontValueChange = 5;
bool frontValueIncrease;
Color getFrontColor() {
	if (frontValue == frontValueMin) {
		frontValueIncrease = true;
	}
	else if (frontValue == frontValueMax) {
		frontValueIncrease = false;
	}

	if (frontValueIncrease) {
		frontValue += frontValueChange;
	}
	else {
		frontValue -= frontValueChange;
	}

	return Color(frontValue);
}

int backValueMin = 200;
int backValueMax = 255;
int backValue = backValueMax;
int backValueChange = 5;
bool backValueIncrease;
Color getBackColor() {
	if (backValue == backValueMin) {
		backValueIncrease = true;
	}
	else if (backValue == backValueMax) {
		backValueIncrease = false;
	}

	if (backValueIncrease) {
		backValue += backValueChange;
	}
	else {
		backValue -= backValueChange;
	}

	return Color(backValue);
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