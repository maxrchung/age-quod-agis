#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "RotationTiming.hpp"
#include "Range.hpp"
#include <string>
#include <deque>
#include "Sprite.hpp"

extern float debugSize;
extern float debugScaleSize;
extern bool debugSpectrum;

extern std::string snowflakeBase;
extern std::string beatmapDirectory;
extern std::string beatmapTitle;
extern std::string difficultyName;
extern std::string beatmapPath;
extern std::string storyboardPath;
extern std::string songPath;
extern std::string snowflakeDirectory;

extern float bpm;
extern float mpb;
extern float spb;
extern float mspb;
extern float offset;

extern int songStart;
extern int songStartOffset;
extern int songEnd;
extern int songEndOffset;

extern std::deque<Sprite*> particles;
extern int particleCount;
extern float particleDistance;
extern float particleBuffer;
extern float particleScale;
extern float particleOpacityMin;
extern float particleOpacityMax;
extern int particleFadeOut;
extern int particleFadeIn;
extern int particleFrequency;
extern int particleRotateAmount;
extern int particleDiscretes;

extern Vector2 midpoint;
extern float scaleUp;
extern float scaleOffset;

extern std::string backgroundFileName;
extern Sprite* background;
extern Vector2 backgroundScale;

extern std::string centerpieceFileName;
extern Sprite* centerpiece;
extern float centerpieceScale;

extern int frontColorMin;
extern int frontColorMax;
extern int backColorMin;
extern int backColorMax;
extern int colorDuration;

// Helper functions
int getSnowflakeCount(std::string inputDirectory);

extern int snowflakeCount;
extern int previousSnowflake;
std::string getSnowflake();

float dToR(int degrees);

int getNextLane(int lane, int index);

extern std::vector<Range> scaleOffRanges;
extern std::string scaleOffRangesPath;
std::vector<Range> readScaleOffRanges(std::string filePath);

extern float rotationFreq;
extern float rotationPeriod;
extern float rotationDiscretes;
extern std::vector<RotationTiming> rotationTimings;
extern std::string rotationTimingsPath;
std::vector<RotationTiming> readRotationTimings(std::string filePath);

#endif//GLOBAL_HPP