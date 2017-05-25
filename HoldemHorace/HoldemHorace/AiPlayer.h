#ifndef _AIPLAYER_H
#define _AIPLAYER_H

#include "BasePlayer.h"

class Texture2D;

class AiPlayer : public BasePlayer
{
public:
	AiPlayer(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture, Texture2D* cardBackTexture);
	~AiPlayer();

	void Render() const;
	void Update(float deltaTime, SDL_Event event);
	void Call(const ACTION previousAction, int& mPot, int currentBet);
	void Fold(const int potAmount);
	bool Raise(ACTION& mLastActionTaken, int& mCurrentBet, int& mPot);

	void TakeTurn(ACTION lastPlayerAction);

	ACTION GetActionThisHand() const { return mActionThisHand; }
	void SetActionThisHand(const ACTION val) { mActionThisHand = val; }
	void SetShowCards(const bool val) { mShowCards = val; }

	//ai player starts bb
	void ResetPlayer() { mStack = 100; mHand = NULL; mPotContributionThisHand = 0; mIsOnButton = false; mHasFocus = false; mActionThisHand = ACTION_NONE; }


private:
	Texture2D* mCardBackTexture;
	ACTION mActionThisHand;
	bool mShowCards;
};
#endif // !_AIPLAYER_H
