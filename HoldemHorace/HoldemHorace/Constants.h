#ifndef _CONSTANTS_H
#define _CONSTANTS_H

//Constants
const int kScreenWidth = 1024;
const int kScreenHeight = 768;
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

const double kPlaystyleModifierIncrease = 1.15;
const double kPlaystyleModifierDecrease = 0.85;
const double kPlaystyleModifier = 0.15;

const double kVertTextSeperation = 25.0;

#endif // !_CONSTANTS_H
