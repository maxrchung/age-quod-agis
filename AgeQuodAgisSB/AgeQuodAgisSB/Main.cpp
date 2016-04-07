#include "Storyboard.hpp"
#include "Sprite.hpp"
#include "Vector2.hpp"
#include "Beatmap.hpp"
#include "Spectrum.hpp"
#include "Variables.hpp"
#include "Helpers.hpp"
#include <iostream>

// Global variable and helpers can be found in Variables.hpp and Helpers.hpp respectively
// I decided to move these out to declutter this Main file. Not sure if this is the 
// best way to separate my files, but I just wanted to make this file cleaner

// Beatmap and Spectrum are singletons that control all the notes of the map and the 
// music spectrum respectively

int main() {
	// Setup spectrum bars
	Spectrum::Instance()->Generate(songPath);

	// Background setup
	background->Color(songStartOffset, songStart, Color(backValue), Color(backValue));
	background->Fade(songStartOffset, songStart, 0.0f, 1.0f);
	background->Fade(songEnd, songEndOffset, 1.0f, 0.0f);

	// Centerpiece setup
	centerpiece->Color(songStartOffset, songStart, Color(frontValue), Color(frontValue));
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
					float radians = dToR(degrees);

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

					float partialRatio = (float) (particleRotateEnd - particleStart) / (particleEnd - particleStart);
					float partialDistance = partialRatio * particleDistance;
					// Because we have to be precise with our start/end times, this move may not go the full distance
					Vector2 startPos(midpoint.x + cos(radians) * particleBuffer, midpoint.y + sin(radians) * particleBuffer);
					Vector2 endPos(midpoint.x + cos(radians) * partialDistance, midpoint.y + sin(radians) * partialDistance);
					particle->Move(particleStart, particleRotateEnd, startPos, endPos);

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

			background->Color(startOffset, note->end, background->color, backColor);
			centerpiece->Color(startOffset, note->end, centerpiece->color, frontColor);
			for (auto particle : particles) {
				particle->Color(startOffset, note->end, particle->color, frontColor);
			}
		}

		// Rotation
		else { // if (note->lane == rotateLane) {
			int degrees = rotateAmount;
			float radians = dToR(degrees);
			centerpiece->Rotate(startOffset, note->end, centerpiece->rotation, centerpiece->rotation + radians);

			for (auto particle : particles) {
				// Finding timeRemaining
				// partialDist / totalDist = (totalTime - timeRemaining) / totalTime
				// partialDist * totalTime / totalDist = totalTime - timeRemaining
				// timeRemaining = totalTime - partialDist * totalTime/totalDist
				float currentDistance = (particle->position - midpoint).Magnitude();
				float partialDist = currentDistance - particleBuffer;
				float totalDist = particleDistance - particleBuffer;
				float totalTime = particleFadeOut;
				float timeRemaining = totalTime - partialDist * totalTime / totalDist;

				int particleEnd = startOffset + timeRemaining;
				int particleRotateEnd = getNextLane(rotateLane, n);

				float absoluteRotation = particle->rotation + radians;
				Vector2 rotatedPos(midpoint.x + cos(absoluteRotation) * particleDistance, midpoint.y + sin(absoluteRotation) * particleDistance);

				if (particleRotateEnd < particleEnd) {
					Vector2 particlePath = rotatedPos - particle->position;
					particlePath.Normalize();

					float partialRatio = (float) (particleRotateEnd - startOffset) / (particleEnd - startOffset);
					particlePath *= partialRatio;
					rotatedPos = particle->position + particlePath;
					particle->Move(startOffset, particleRotateEnd, particle->position, rotatedPos);
				}
				else {
					particle->Move(startOffset, particleEnd, particle->position, rotatedPos);
				}

				particle->Rotate(startOffset, note->end, particle->rotation, particle->rotation + radians);
			}
		}
	}

	// Output to storyboard file
	Storyboard::Instance()->Write(storyboardPath);
	std::cout << "Finished storyboard generation" << std::endl;
	// std::cin.get();
	return 0;
}