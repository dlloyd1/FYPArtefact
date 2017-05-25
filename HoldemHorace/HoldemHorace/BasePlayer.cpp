#include "BasePlayer.h"

BasePlayer::BasePlayer(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture)
{
	mHand = NULL;
	mCardsSpritesheet = cardSpritesheet;
	mCardOutlineTexture = cardOutlineTexture;

	mStack = 100;
	mPotContributionThisHand = 0;
}

BasePlayer::~BasePlayer()
{
}

void BasePlayer::ReceiveCardFromTable(const Card card)
{
	if (mHand == NULL)
	{
		mHand = new vector<Card>();
	}
	mHand->push_back(card);
}

void BasePlayer::ClearHand()
{
	delete mHand;
	mHand = new vector<Card>();
}

int BasePlayer::PostBlind(const int smallBlind)
{
	if (mIsOnButton)
	{
		mStack -= smallBlind;
		return smallBlind;
	}
	else
	{
		//BB
		mStack -= smallBlind * 2;
		return smallBlind * 2;
	}
}

int BasePlayer::GetMaxBetBeforeNegativeStack(const int betAmount) const
{
	int temp = betAmount;

	while ((mStack - temp) < 0)
	{
		temp--;
	}
	return temp;
}