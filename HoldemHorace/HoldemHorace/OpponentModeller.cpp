#include "OpponentModeller.h"


OpponentModeller::OpponentModeller()
{
	mNumHandsFolded = 0.0;
	mNumHandsRaised = 0.0;
	mNumHandsCalled = 0.0;
	mHandCount = 0.0;
	mHasRaisedThisHand = false;
	mHasCalledThisHand = false;
	mHasTakenActionThisHand = false;
}

OpponentModeller::~OpponentModeller()
{
}

double OpponentModeller::CalculateHandFoldPercentage() const
{
	return mNumHandsFolded / mHandCount;
}

double OpponentModeller::CalculateRaiseToCallRatio() const
{
	double raisePercentage = mNumHandsRaised / mHandCount;
	double callPercentage = mNumHandsCalled / mHandCount;

	//divide by 0 protection
	if (raisePercentage == 0.0 && callPercentage == 0.0)
	{
		return 0.0;
	}

	return 1 + ((raisePercentage - callPercentage) / (raisePercentage + callPercentage));
}
