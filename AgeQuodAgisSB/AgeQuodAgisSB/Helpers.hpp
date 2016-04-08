#ifndef HELPERS_HPP
#define HELPERS_HPP

#define _USE_MATH_DEFINES

#include "Variables.hpp"
#include <Windows.h>
#include "Beatmap.hpp"
#include <math.h>

int getSnowflakeCount(std::string inputDirectory);

extern int snowflakeCount;
extern int previousSnowflake;
std::string getSnowflake();

float dToR(int degrees);

extern int frontValueMin;
extern int frontValueMax;
extern int frontValue;
extern int frontValueChange;
extern bool frontValueIncrease;
Color getFrontColor();

extern int backValueMin;
extern int backValueMax;
extern int backValue;
extern int backValueChange;
extern bool backValueIncrease;
Color getBackColor();

int getNextLane(int lane, int index);

#endif//HELPERS_HPP