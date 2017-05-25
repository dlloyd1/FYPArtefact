#ifndef _PLAYER_H
#define _PLAYER_H

#include "BasePlayer.h"

class Texture2D;

class Player : public BasePlayer
{
	
public:
	Player(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture);
	~Player();

	void Render() const;
	void Update(float deltaTime, SDL_Event event);
	//human player starts sb
	void ResetPlayer() { mStack = 100; mHand = NULL; mPotContributionThisHand = 0; mIsOnButton = true; mHasFocus = true; }
	void Call(const ACTION previousAction, const int currentBet, int& pot);
	void Raise(ACTION& mLastActionTaken, int& mCurrentBet, int& mPot);
	void Fold(const int potAmount) const;

private:
};
#endif // !_PLAYER_H