#include "AiPlayer.h"

AiPlayer::AiPlayer(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture, Texture2D* cardBackTexture)
	: BasePlayer(renderer, cardSpritesheet, cardOutlineTexture)
{
	mCardBackTexture = cardBackTexture;

	//Ai player is second to act
	mIsOnButton = false;
	mHasFocus = false;
}

//common params = SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture
//AI Param = Texture2D* cardBackTexture

AiPlayer::~AiPlayer()
{
	delete mHand;
}

void AiPlayer::TakeTurn(ACTION lastPlayerAction)
{
	
}

void AiPlayer::Render()
{
	//render AI card backs unless shown to player, then render the fronts
	for (int i = 0; i < 2; ++i)
	{
		Vector2D positionOnScreen = Vector2D((kScreenWidth / 2.0) - kCardWidth + i * kCardWidth, (kScreenHeight * 0.25) + kCardHeight);
		SDL_Rect destRect = { (int)positionOnScreen.x, (int)positionOnScreen.y, kCardWidth, kCardHeight };

		//space cards out
		if (i == 1)
		{
			destRect = { (int)positionOnScreen.x + kCardSeperationInPixels, (int)positionOnScreen.y, kCardWidth, kCardHeight };
			mCardOutlineTexture->Render(Vector2D((int)(positionOnScreen.x + 5), (int)(positionOnScreen.y - 5)));
		}
		else
		{
			mCardOutlineTexture->Render(Vector2D((int)(positionOnScreen.x - 5), (int)(positionOnScreen.y - 5)));
		}

		if (mHand != NULL)
		{
			SDL_Rect spriteRect = { 0, 0, kCardWidth, kCardHeight };
			mCardBackTexture->Render(spriteRect, destRect);
		}
	}

}
