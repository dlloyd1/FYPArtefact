#ifndef _BASEPLAYER_H
#define _BASEPLAYER_H

#include "Structures.h"
#include "Constants.h"

#include <vector>
#include <iostream>
#include <cassert> 

#include "Texture2D.h"
#include "SDL_ttf.h"

using namespace std;

class BasePlayer
{
public:
	BasePlayer(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture);
	virtual ~BasePlayer(); //reminder: virtual destructor, prevent undefined behaviour when calling delete on base class pointer

	virtual void Render() = 0;

	void ReceiveCardFromTable(Card card);
	void ClearHand();
	int PostBlind(int smallBlind);
	int GetStack() { return mStack; }
	bool GetIsOnButton() { return mIsOnButton; }
	bool GetHasFocus() { return mHasFocus; }
	int GetPotContribution() { return mPotContributionThisHand; }
	vector<Card>* GetHand() { return mHand; }

	void SetIsOnButton(bool val) { mIsOnButton = val; }
	void SetHasFocus(bool val) { mHasFocus = val; }
	void AmendStack(int val) { mStack += val; }
	void AmendPotContribution(int val) { mPotContributionThisHand += val; }
	void ResetPotContribution() { mPotContributionThisHand = 0; }
	void ResetPlayer() { mStack = 100; mHand = NULL; mPotContributionThisHand = 0; }
	int GetMaxBetBeforeNegativeStack(int betAmount);

private:
	void PrintCard(Card card);
	int mStack;
	int mPotContributionThisHand;

protected:
	vector<Card>* mHand;
	Texture2D* mCardsSpritesheet;
	Texture2D* mCardOutlineTexture;
	bool mIsOnButton;
	bool mHasFocus;
};

#endif // !_BASEPLAYER_H
