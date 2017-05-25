#ifndef _OPPONENTMODELLER_H
#define _OPPONENTMODELLER_H

#include "Structures.h"

class OpponentModeller
{
public:
	~OpponentModeller();

	OpponentModeller();

	static OpponentModeller* Instance()
	{
		static OpponentModeller instance;
		return &instance;
	}

	void TrackFoldedHand() { mNumHandsFolded++;	}
	void TrackRaisedHand() { mNumHandsRaised++; }
	void TrackCallHand() { mNumHandsCalled++;	}
	void IncrementHandCounter() { mHandCount++; }

	double CalculateHandFoldPercentage() const;
	double CalculateRaiseToCallRatio() const;

	void SetHasRaisedThisHand(const bool val) { mHasRaisedThisHand = val; }
	bool GetHasRaisedThisHand() const { return mHasRaisedThisHand; }
	void SetHasCalledThisHand(const bool val) { mHasCalledThisHand = val; }
	bool GetHasCalledThisHand() const { return mHasCalledThisHand; }
	bool GetActionTakenThisHand() const { return mHasTakenActionThisHand; }
	void SetActionTakenThisHand(const bool val) { mHasTakenActionThisHand = val; }


	//Debug accessors
	double GetFolded() const { return mNumHandsFolded; }
	double GetRaised() const { return mNumHandsRaised; }
	double GetCalled() const { return mNumHandsCalled; }
	double GetTotalHands() const { return mHandCount; }
	double GetFoldPercentage() const { return mNumHandsFolded / mHandCount; }
	double GetCallPercentage() const { return mNumHandsCalled / mHandCount; }
	double GetRaisePercentage() const { return mNumHandsRaised / mHandCount; }

private:
	OpponentModeller(const OpponentModeller&);
	OpponentModeller& operator=(const OpponentModeller&) {};

	double mNumHandsFolded;
	double mNumHandsRaised;
	double mNumHandsCalled;
	double mHandCount;
	bool mHasRaisedThisHand;
	bool mHasCalledThisHand;
	bool mHasTakenActionThisHand;
};

#endif // !_OPPONENTMODELLER_H