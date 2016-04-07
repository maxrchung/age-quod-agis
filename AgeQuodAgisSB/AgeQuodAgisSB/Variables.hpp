#ifndef VARIABLES_HPP
#define VARIABLES_HPP

#include <string>
#include <deque>
#include "Sprite.hpp"

// Path setup
std::string snowflakeBase = R"(Snowflakes\snowflake)";
std::string beatmapDirectory = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\)";
std::string beatmapTitle = "void - Age quod agis (TheWeirdo9)";
std::string difficultyName = "[Age quod agis]";
std::string beatmapPath = beatmapDirectory + beatmapTitle + " " + difficultyName + ".osu";
std::string storyboardPath = beatmapDirectory + beatmapTitle + ".osb";
// Due to how I analyze the music, the song must be mono/single channel
std::string songPath = R"(X:\Music\void\Age quod agis\ageQuodAgisLeftMono.wav)";

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

#endif//VARIABLES_HPP