#define _USE_MATH_DEFINES

#include "Storyboard.hpp"
#include "Sprite.hpp"
#include "Vector2.hpp"
#include "Beatmap.hpp"
#include <iostream>
#include <Windows.h>
#include <math.h>
#include <deque>

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

// Path setup
std::string snowflakeBase = R"(Snowflakes\snowflake)";
std::string beatmapDirectory = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\)";
std::string beatmapTitle = "void - Age quod agis (TheWeirdo9)";
std::string difficultyName = "[Age quod agis]";
std::string beatmapPath = beatmapDirectory + beatmapTitle + " " + difficultyName + ".osu";
std::string storyboardPath = beatmapDirectory + beatmapTitle + ".osb";
std::string inputDirectory = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\Snowflakes\)";
int snowflakeCount = getSnowflakeCount(inputDirectory);
int previousSnowflake = 0;

// Randomly picks a new snowflake
std::string getSnowflake() {
	int suffix;

	do {
		suffix = rand() % snowflakeCount + 1;
	} while (previousSnowflake == suffix);
	previousSnowflake = suffix;

	return snowflakeBase + std::to_string(suffix);
}

// Convert degrees to radians
float dtor(int degrees) {
	return (degrees * M_PI / 180.0f);
}

// Colors decrease and increase by a set amount each time a certain lane is pressed
// Front colors (centerpiece + particles) are lighter, while the background is darker
int frontValueMin = 200;
int frontValueMax = 255;
int frontValue = frontValueMax;
int frontValueChange = 5;
bool frontValueIncrease = false;

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

int backValueMin = 0;
int backValueMax = 55;
int backValue = backValueMin;
int backValueChange = 5;
bool backValueIncrease = true;

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

// BPM
float bpm = 140.0f;
float mpb = 1 / bpm;
float spb = mpb * 60;
// ~429 ms per beat
float mspb = 1000 * spb;
int offset = mspb / 4;

// Overall timing
int songStart = 1692;
int songStartOffset = songStart - mspb;
int songEnd = 358263;
int songEndOffset = songEnd + mspb;

// Set lanes
int scaleLane = 1;
int particleLane = 2;
int colorLane = 3;
int rotateLane = 4;

// Particles
// Deque because we need to remove front particles if there're too many
std::deque<Sprite*> particles;
int particleCount = 3;
float particleDistance = 500;
float particleBuffer = 40;
float particleScale = 0.03f;
float particleOpacity = 0.25f;
int particleFadeOut = mspb * 10;
int particleFadeIn = mspb;
int particleFrequency = 2 * mspb;

// Other
Vector2 midpoint(320, 240);
float scaleUp = 1.25f;
// In degrees
int rotateAmount = 15;
float centerpieceScale = 0.15f;
Sprite* background = new Sprite("blackbg.png", midpoint, Layer::Background);
Sprite* centerpiece = new Sprite("Snowflakes/centerpiece.png", midpoint);

// Returns the timing of the specified lane from an index
int getNextLane(int lane, int index) {
	for (int i = index; i < Beatmap::Instance()->notes.size(); ++i) {
		if (Beatmap::Instance()->notes[i]->lane == lane) {
			return Beatmap::Instance()->notes[i]->start - offset;
		}
	}
	return songEndOffset;
}

int main() {
	// Background setup
	background->Fade(songStartOffset, songStart, 0.0f, 1.0f);
	background->Fade(songEnd, songEndOffset, 1.0f, 0.0f);

	// Centerpiece setup
	centerpiece->Color(songStartOffset, songStart, Color(255), Color(255));
	centerpiece->Fade(songStartOffset, songStart, 0.0f, 1.0f);
	centerpiece->Fade(songEnd, songEndOffset, 1.0f, 0.0f);

	// Parse in hit objects into notes and holds
	Beatmap::Instance()->ParseHitObjects(beatmapPath);

	// Handle notes
	// Use index so we can call getNextLane()
	for (int n = 0; n < Beatmap::Instance()->notes.size(); ++n) {
		Note* note = Beatmap::Instance()->notes[n];
		std::cout << "Processing note at: " << note->start << std::endl;

		int startOffset = note->start - offset;
		int endOffset = note->end + offset;

		// Remove dead particles
		while (particles.size() > 0 && particles.front()->endTime < startOffset) {
			particles.pop_front();
		}

		// Scale
		if (note->lane == scaleLane) {
			centerpiece->Scale(startOffset, note->end, centerpieceScale, centerpieceScale * scaleUp);
			centerpiece->Scale(note->end, endOffset, centerpieceScale * scaleUp, centerpieceScale);

			for (auto particle : particles) {
				particle->Scale(startOffset, note->start, particleScale, particleScale * scaleUp);
				particle->Scale(note->end, endOffset, particleScale * scaleUp, particleScale);
			}
		}

		// Particles
		else if (note->lane == particleLane) {
			// Repeatedly make new particles if note is a hold
			int heldTime = note->end - note->start;

			for (int i = 0; i <= heldTime; i += particleFrequency) {
				int noteStart = note->start + i;
				for (int j = 0; j < particleCount; ++j) {
					std::string snowflakeName = getSnowflake();
					Sprite* particle = new Sprite(snowflakeName, midpoint, Layer::Background);

					// Appearing too fast seems a little awkward for particles, so I set a longer
					// fade in/out period
					int particleStart = noteStart - particleFadeIn;
					int particleEnd = noteStart + particleFadeOut;
					particle->Fade(particleStart, noteStart, 0.0f, particleOpacity);
					particle->Fade(noteStart, particleEnd, particleOpacity, 0.0f);

					// Choose a random direction and move particle to the end
					int degrees = rand() % 360;
					float radians = dtor(degrees);

					// The way the osu! storyboard works is that commands that end later take precedent over
					// commands that take place earlier. This can be annoying to work with since you run into
					// conflicts if you set a command that stretches a long period of time and try to set another
					// command in a smaller interval within it. In order to help fight this, I made a getNextLane()
					// function that finds the next timing of a lane.
					int particleRotateEnd = getNextLane(rotateLane, n);
					// Need to reset end times to have the same end times across all commands
					if (particleRotateEnd > particleEnd) {
						particleRotateEnd = particleEnd;
					}
					particle->Rotate(particleStart, particleRotateEnd, radians, radians);

					// Because we have to be precise with our start/end times, this move may not go the full distance
					Vector2 startPos(midpoint.x + cos(radians) * particleBuffer, midpoint.y + sin(radians) * particleBuffer);
					Vector2 endPos(midpoint.x + cos(radians) * particleDistance, midpoint.y + sin(radians) * particleDistance);
					particle->Move(particleStart, particleEnd, startPos, endPos);

					int particleColorEnd = getNextLane(colorLane, n);
					if (particleColorEnd > particleEnd) {
						particleColorEnd = particleEnd;
					}
					if (particles.size() > 0) {
						particle->Color(particleStart, particleColorEnd, particles.front()->color, particles.front()->color);
					}

					particles.push_back(particle);
				}
			}
		}

		// Color
		else if (note->lane == colorLane) {
			Color frontColor = getFrontColor();
			Color backColor = getBackColor();

			background->Color(startOffset, endOffset, background->color, backColor);
			centerpiece->Color(startOffset, endOffset, centerpiece->color, frontColor);
			for (auto particle : particles) {
				particle->Color(startOffset, endOffset, particle->color, frontColor);
			}
		}

		// Rotation
		else if (note->lane == rotateLane) {
			int degrees = rotateAmount;
			float radians = dtor(degrees);
			centerpiece->Rotate(startOffset, endOffset, centerpiece->rotation, centerpiece->rotation + radians);

			for (auto particle : particles) {
				particle->Rotate(startOffset, endOffset, particle->rotation, particle->rotation + radians);
				particle->Move(note->start, particle->endTime, particle->position, Vector2(500, 500));
			}
		}
	}

	// Music spectrum?

	// Output to storyboard file
	Storyboard::Instance()->Write(storyboardPath);
	std::cout << "Finished storyboard generation" << std::endl;
	std::cin.get();
	return 0;
}