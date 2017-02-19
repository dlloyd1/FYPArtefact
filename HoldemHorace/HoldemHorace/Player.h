#ifndef _PLAYER_H
#define _PLAYER_H

#include "BasePlayer.h"

class Texture2D;

class Player : public BasePlayer
{
	
public:
	Player(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture);
	~Player();

	void Render();
	void Update(float deltaTime, SDL_Event event);

	void ResetPlayer() { mIsOnButton = true; mHasFocus = true; }
private:
};
#endif // !_PLAYER_H