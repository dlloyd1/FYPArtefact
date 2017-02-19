#ifndef _AIPLAYER_H
#define _AIPLAYER_H

#include "BasePlayer.h"

class Texture2D;

class AiPlayer : public BasePlayer
{
public:
	AiPlayer(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture, Texture2D* cardBackTexture);
	~AiPlayer();

	void Render();
	void Update(float deltaTime, SDL_Event event);

	void TakeTurn(ACTION lastPlayerAction);
	void ResetAiPlayer() { mIsOnButton = false; mHasFocus = false; }

private:
	Texture2D* mCardBackTexture;
};
#endif // !_AIPLAYER_H
