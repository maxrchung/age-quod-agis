#define _USE_MATH_DEFINES

#include "Storyboard.hpp"
#include "Sprite.hpp"
#include "Vector2.hpp"
#include "Beatmap.hpp"
#include "Spectrum.hpp"
#include <iostream>

// Global variable and helpers can be found in Global.hpp
// I decided to move these out of this Main file to help declutter things. 
// Not sure if this is the best way to space out my code, but I just wanted
// to make this file cleaner
#include "Global.hpp"

// Beatmap and Spectrum are singletons that control all the notes of the map and the 
// music spectrum respectively

int main() {
	// Apparently globals are dangerous, and if you try and set a global variable
	// with a global function, you're going to have a bad time
	snowflakeCount = getSnowflakeCount(snowflakeDirectory);

	// Setup spectrum bars
	// Spectrum::Instance()->Generate(songPath);
	
	// Background setup
	background->ScaleVector(songStartOffset, songEndOffset, backgroundScale, backgroundScale);
	background->Fade(songStartOffset, songStart, 0.0f, 1.0f);
	background->Fade(songEnd, songEndOffset, 1.0f, 0.0f);
	
	// Centerpiece setup
	centerpiece->Fade(songStartOffset, songStart, 0.0f, 1.0f);
	centerpiece->Fade(songEnd, songEndOffset, 1.0f, 0.0f);
	centerpiece->Scale(songStartOffset, songStartOffset, centerpieceScale, centerpieceScale);

	// Parse in hit objects into notes and holds
	std::cout << "Handling particles..." << std::endl;
	Beatmap::Instance()->ParseHitObjects(beatmapPath);

	// Handle notes
	// Use index so we can call getNextLane()
	for (int n = 0; n < Beatmap::Instance()->notes.size() * debugSize; ++n) {
		Note* note = Beatmap::Instance()->notes[n];

		// Particles
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

				// Applies a fade according to which lane it is in
				float opacityRange = particleOpacityMax - particleOpacityMin;
				float opacityScale = note->lane / 4.0f;
				float particleOpacity = (opacityRange * opacityScale) + particleOpacityMin;
				particle->Fade(particleStart, noteStart, 0.0f, particleOpacity);
				int opacityEnd;
				if (particleEnd <= songEndOffset) {
					opacityEnd = particleEnd;
				}
				else {
					opacityEnd = songEndOffset;
				}
				particle->Fade(noteStart, opacityEnd, particleOpacity, 0.0f);

				// Choose a random direction and rotate/move particle 
				int rotationDeg = rand() % 360;
				float rotation = dToR(rotationDeg);
				particle->Rotate(particleStart, particleEnd, rotation, rotation);
				Vector2 startPos(midpoint.x + cos(rotation) * particleBuffer, midpoint.y + sin(rotation) * particleBuffer);
				Vector2 endPos(midpoint.x + cos(rotation) * particleDistance, midpoint.y + sin(rotation) * particleDistance);
				particle->Move(particleStart, particleEnd, startPos, endPos);

				particle->Scale(particleStart, particleStart, particleScale, particleScale);

				particles.push_back(particle);
			}
		}
	}

	// Handle rotation
	std::cout << "Handling rotations..." << std::endl;
	rotationTimings = readRotationTimings(rotationTimingsPath);

	for (int i = 0; i < rotationTimings.size(); ++i) {
		int endTime;
		if (i != rotationTimings.size() - 1) {
			endTime = rotationTimings[i + 1].start;
		}
		else {
			endTime = songEndOffset;
		}
		int timeDiff = endTime - rotationTimings[i].start;

		// power * rotationRate / rotationPeriod = overallRotation / timeDiff
		float overallRotation = rotationTimings[i].power * rotationRate * timeDiff / rotationPeriod;
		float endRotation = centerpiece->rotation + overallRotation;
		centerpiece->Rotate(rotationTimings[i].start, endTime, centerpiece->rotation, endRotation);

		for (auto bar : Spectrum::Instance()->bars) {

		}
	}

	// Handle scale
	std::cout << "Handling scale..." << std::endl;
	scaleOffRanges = readScaleOffRanges(scaleOffRangesPath);
	// Making a copy because I have to reference the particle list in order to find what
	// particles are active at what time, and to make my work more efficient I pop off
	// the first values when I'm done. I don't want to use the original because I may still
	// need it for other things.
	std::deque<Sprite*> particleCopy = particles;
	for (int i = songStart; i <= songEndOffset * debugSize; i += scaleOffset) {
		while (particleCopy.size() > 0 && particleCopy.front()->endTime <= i) {
			particleCopy.pop_front();
		}

		bool skip = false;
		for (auto range : scaleOffRanges) {
			if (i > range.begin && i < range.end) {
				skip = true;
				break;
			}
		}

		if (skip) {
			continue;
		}
		else {
			int startOffset = i - offset;
			int endOffset = i + offset;
			centerpiece->Scale(startOffset, i, centerpieceScale, centerpieceScale * scaleUp);
			centerpiece->Scale(i, endOffset, centerpieceScale * scaleUp, centerpieceScale);

			for (auto bar : Spectrum::Instance()->bars) {
				//bar->Scale(startOffset, i, 1.0f, Spectrum::Instance()->barScaleUp);
				//bar->Scale(i, endOffset, Spectrum::Instance()->barScaleUp, 1.0f);

				Vector2 barPos = bar->position;
				float rotationCorrection = bar->rotation - M_PI / 2;
				float scaledPosX = cos(rotationCorrection) * Spectrum::Instance()->barBuffer * Spectrum::Instance()->barScaleUp + midpoint.x;
				float scaledPosY = sin(rotationCorrection) * Spectrum::Instance()->barBuffer * Spectrum::Instance()->barScaleUp + midpoint.y;
				Vector2 scaledPos(scaledPosX, scaledPosY);

				bar->Move(startOffset, i, barPos, scaledPos);
				bar->Move(i, endOffset, scaledPos, barPos);
			}

			for (auto particle : particleCopy) {
				if (particle->endTime - particleFadeOut <= i) {
					particle->Scale(startOffset, i, particleScale, particleScale * scaleUp);
					particle->Scale(i, endOffset, particleScale * scaleUp, particleScale);
				}
			}
		}
	}

	// Handle color
	std::cout << "Handling color..." << std::endl;
	particleCopy = particles;
	bool colorSwitch = true;
	// Use songStart so that you hit peaks on beats
	for (int i = songStart; i <= songEndOffset * debugSize; i += colorDuration) {
		// I'm not sure I like this repeating of code, but it's the simplest I could do
		if (colorSwitch) {
			background->Color(i, i + colorDuration, backColorMin, backColorMax);
			centerpiece->Color(i, i + colorDuration, frontColorMax, frontColorMin);
		}
		else {
			background->Color(i, i + colorDuration, backColorMax, backColorMin);
			centerpiece->Color(i, i + colorDuration, frontColorMin, frontColorMax);
		}

		// Handling bars and particles separately because loops are a little cleaner
		for (auto bar : Spectrum::Instance()->bars) {
			if (colorSwitch) {
				bar->Color(i, i + colorDuration, frontColorMax, frontColorMin);
			}
			else {
				bar->Color(i, i + colorDuration, frontColorMin, frontColorMax);
			}
		}

		// Removes irrelevant particles
		while (particleCopy.size() > 0 && particleCopy.front()->endTime <= i) {
			particleCopy.pop_front();
		}

		for (auto particle : particleCopy) {
			if (particle->endTime - particleFadeOut <= i + colorDuration) {
				if (colorSwitch) {
					particle->Color(i, i + colorDuration, frontColorMax, frontColorMin);
				}
				else {
					particle->Color(i, i + colorDuration, frontColorMin, frontColorMax);
				}
			}
			else {
				break;
			}
		}

		colorSwitch = !colorSwitch;
	}

	// Output to storyboard file
	Storyboard::Instance()->Write(storyboardPath);
	std::cout << "Finished storyboard generation" << std::endl;
	// std::cin.get();
	return 0;
}