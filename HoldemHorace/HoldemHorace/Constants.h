#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include <limits>
#include "Structures.h"

//Constants
const int kScreenWidth = 640;
const int kScreenHeight = 480;
const int kCardWidth = 30;
const int kCardHeight = 50;
const int kCardSeperationInPixels = 10;
const double kMsPerUpdate = 6000.0;
const int kHandsPerBlind = 5;
const int kSmallBlindIncrementAmount = 1;


//limits
const int     MaxInt = (std::numeric_limits<int>::max)();
const double  MaxDouble = (std::numeric_limits<double>::max)();
const double  MinDouble = (std::numeric_limits<double>::min)();
const float   MaxFloat = (std::numeric_limits<float>::max)();
const float   MinFloat = (std::numeric_limits<float>::min)();

//angles
const double   Pi = 3.14159;
const double   TwoPi = Pi * 2;
const double   HalfPi = Pi / 2;
const double   QuarterPi = Pi / 4;

inline double DegsToRads(double degrees) { return TwoPi * (degrees / 360.0f); }
inline double RadsToDegs(double radians) { return radians * (180.0f / Pi); }

#endif // !_CONSTANTS_H
