#define _USE_MATH_DEFINES

#include "Storyboard.hpp"
#include "Sprite.hpp"
#include "Vector2.hpp"
#include "Beatmap.hpp"
#include <iostream>
#include <Windows.h>
#include <math.h>
#include <deque>

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

std::string inputDirectory = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\Snowflakes\)";
int snowflakeCount = getSnowflakeCount(inputDirectory);
int previousSnowflake = 0;
std::string snowflakeBase = R"(Snowflakes\snowflake)";
std::string getSnowflake() {
	int suffix;

	do {
		suffix = rand() % snowflakeCount + 1;
	} while (previousSnowflake == suffix);
	previousSnowflake = suffix;

	return snowflakeBase + std::to_string(suffix);
}

float dtoa(int degrees) {
	return (degrees * M_PI / 180.0f);
}

int previousfrontValue = 255;
Color getFrontColor() {
	int frontValue;
	do {
		frontValue = rand() % 255;
	} while (frontValue == previousfrontValue);

	previousfrontValue = frontValue;
	return Color(rand() % 255, rand() % 255, rand() % 255);
}

int previousbackValue = 0;
Color getBackColor() {
	int backValue;
	do {
		backValue = rand() % 255;
	} while (backValue == previousbackValue);

	previousbackValue = backValue;
	return Color(rand() % 55, rand() % 55, rand() % 55);
}

int main() {
#pragma region Setup
	// Path setup
	std::string beatmapDirectory = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\)";
	std::string beatmapTitle = "void - Age quod agis (TheWeirdo9)";
	std::string difficultyName = "[Age quod agis]";
	std::string beatmapPath = beatmapDirectory + beatmapTitle + " " + difficultyName + ".osu";
	std::string storyboardPath = beatmapDirectory + beatmapTitle + ".osb";

	// Calculate storyboard setup values
	float bpm = 140.0f;
	float mpb = 1 / bpm;
	float spb = mpb * 60;
	// ~429 ms per beat
	float mspb = 1000 * spb;
	int offset = mspb / 4;
	float scaleUp = 1.25f;
	int songStart = 1692;
	int songStartOffset = songStart - mspb;
	int songEnd = 358263;
	int songEndOffset = songEnd + mspb;
	Vector2 midpoint(320, 240);
	Vector2 imageSize(1000, 1000);

	// Background setup
	Vector2 backgroundSize(1366, 768);
	Sprite* background = new Sprite("blackbg.png", midpoint, Layer::Background);
	background->Fade(songStartOffset, songStart, 0.0f, 1.0f);
	background->Fade(songEnd, songEndOffset, 1.0f, 0.0f);

	// Centerpiece setup
	float centerpieceScale = 0.15f;
	Sprite* centerpiece = new Sprite("Snowflakes/centerpiece.png", midpoint);
	centerpiece->Fade(songStartOffset, songStart, 0.0f,	1.0f);
	centerpiece->Fade(songEnd, songEndOffset, 1.0f,	0.0f);

	// Parse in hit objects into notes and holds
	Beatmap::Instance()->ParseHitObjects(beatmapPath);
#pragma endregion

#pragma region Note_matching
	// Deque because we need to remove front particles if there's too many
	std::deque<Sprite*> particles;
	int particleCount = 4;
	float particleDistance = 400;
	float particleBuffer = 40;
	float particleScale = 0.03f;
	float particleOpacity = 0.5f;
	int particleFadeOut = mspb * 10;
	int particleFadeIn = mspb;
	int particleFrequency = 2 * mspb;

	for (auto note : Beatmap::Instance()->notes) {
		std::cout << "Processing note at: " << note->start << std::endl;

		int startOffset = note->start - offset;
		int endOffset = note->end + offset;

		// Remove dead particles
		while (particles.size() > 0 && particles.front()->endTime < startOffset) {
			particles.pop_front();
		}

		// Scale
		if (note->lane == 1) {
			centerpiece->Scale(startOffset, note->end, centerpieceScale, centerpieceScale * scaleUp);
			centerpiece->Scale(note->end, endOffset, centerpieceScale * scaleUp, centerpieceScale);

			for (auto particle : particles) {
				particle->Scale(startOffset, note->start, particleScale, particleScale * scaleUp);
				particle->Scale(note->end, endOffset, particleScale * scaleUp, particleScale);
			}
		}

		// Particles
		else if (note->lane == 2) {
			// Repeatedly make new particles if note is a hold
			int heldTime = note->end - note->start;
			for (int i = 0; i <= heldTime; i += particleFrequency) {
				int noteStart = note->start + i;
				for (int j = 0; j < particleCount; ++j) {
					std::string snowflakeName = getSnowflake();
					Sprite* particle = new Sprite(snowflakeName, midpoint, Layer::Background);

					int degrees = rand() % 360;
					float radians = dtoa(degrees);
					Vector2 startPos(midpoint.x + cos(radians) * particleBuffer, midpoint.y + sin(radians) * particleBuffer);
					Vector2 endPos(midpoint.x + cos(radians) * particleDistance, midpoint.y + sin(radians) * particleDistance);
					int particleStart = noteStart - particleFadeIn;
					int particleEnd = noteStart + particleFadeOut;
					particle->Move(particleStart, particleEnd, startPos.x, startPos.y, endPos.x, endPos.y);

					particle->Rotate(particleStart, particleEnd, radians, radians);

					particle->Fade(particleStart, noteStart, 0.0f, particleOpacity);
					particle->Fade(noteStart, particleEnd, particleOpacity, 0.0f);

					//if (particles.size() > 0) {
					//	particle->Color(particleStart,
					//		particleEnd,
					//		particles.front()->color.r,
					//		particles.front()->color.g,
					//		particles.front()->color.b,
					//		particles.front()->color.r,
					//		particles.front()->color.g,
					//		particles.front()->color.b);
					//}

					particles.push_back(particle);
				}
			}
		}

		// Color
		else if (note->lane == 3) {
			Color FrontColor = getFrontColor();
			Color BackColor = getBackColor();

			background->Color(startOffset, endOffset, background->color.r, background->color.g, background->color.b, BackColor.r, BackColor.g, BackColor.b);
			centerpiece->Color(startOffset, endOffset, centerpiece->color.r, centerpiece->color.g, centerpiece->color.b, FrontColor.r, FrontColor.g, FrontColor.b);
			for (auto particle : particles) {
				particle->Color(startOffset, endOffset, particle->color.r, particle->color.g, particle->color.b, FrontColor.r, FrontColor.g, FrontColor.b);
			}
		}

		// Rotation
		else {

		}
	}
#pragma endregion


	// Music spectrum?

	// Output to storyboard file
	Storyboard::Instance()->Write(storyboardPath);
	std::cout << "Finished storyboard generation" << std::endl;
	std::cin.get();
	return 0;
}