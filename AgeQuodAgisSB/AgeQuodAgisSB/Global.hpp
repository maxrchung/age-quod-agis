#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <string>
#include <deque>
#include "Sprite.hpp"

extern float debugSize;

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
extern int offset;

extern int songStart;
extern int songStartOffset;
extern int songEnd;
extern int songEndOffset;

extern int scaleLane;
extern int particleLane;
extern int colorLane;
extern int rotateLane;

extern std::deque<Sprite*> particles;
extern int particleCount;
extern float particleDistance;
extern float particleBuffer;
extern float particleScale;
extern float particleOpacity;
extern int particleFadeOut;
extern int particleFadeIn;
extern int particleFrequency;
extern int particleRotateAmount;

extern Vector2 midpoint;
extern float scaleUp;
extern int rotateAmount;
extern float centerpieceScale;
extern std::string backgroundFileName;
extern Sprite* background;
 
extern std::string centerpieceFileName;
extern Sprite* centerpiece;

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

#endif//GLOBAL_HPP