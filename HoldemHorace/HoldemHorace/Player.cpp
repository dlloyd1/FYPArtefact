#include "Player.h"

Player::Player(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture) 
	: BasePlayer(renderer, cardSpritesheet, cardOutlineTexture)
{
	//human player is first to act
	mIsOnButton = true;
	mHasFocus = true;
}

Player::~Player()
{
	delete mHand;
}

void Player::Render()
{
	//render player cards here
	for (int i = 0; i < 2; ++i)
	{
		//render relevant card textures here
		Vector2D positionOnScreen = Vector2D((kScreenWidth / 2.0) - kCardWidth + i * kCardWidth, (kScreenHeight * 0.75) - kCardHeight);

		SDL_Rect destRect = { (int)positionOnScreen.x, (int)positionOnScreen.y,  kCardWidth, kCardHeight };

		//space between the cards
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
			SDL_Rect spriteSheetRect = { mHand->at(i).suit * kCardWidth, mHand->at(i).rank * kCardHeight, kCardWidth, kCardHeight };
			mCardsSpritesheet->Render(spriteSheetRect, destRect);
		}
	}
}

