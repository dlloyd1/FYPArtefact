#include "AiPlayer.h"

AiPlayer::AiPlayer(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture, Texture2D* cardBackTexture)
	: BasePlayer(renderer, cardSpritesheet, cardOutlineTexture)
{
	mCardBackTexture = cardBackTexture;

	//Ai player is second to act
	mIsOnButton = false;
	mHasFocus = false;
	mShowCards = false;
}

AiPlayer::~AiPlayer()
{
	delete mHand;
}

void AiPlayer::TakeTurn(ACTION lastPlayerAction)
{
	
}

void AiPlayer::Render() const
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
			if (!mShowCards) //card backs
			{
				SDL_Rect spriteRect = { 0, 0, kCardWidth, kCardHeight };
				mCardBackTexture->Render(spriteRect, destRect);
			}
			else //card fronts
			{
				SDL_Rect spriteSheetRect = { mHand->at(i).suit * kCardWidth, mHand->at(i).rank * kCardHeight, kCardWidth, kCardHeight };
				mCardsSpritesheet->Render(spriteSheetRect, destRect);
			}
		}
	}
}

void AiPlayer::Call(const ACTION previousAction, int& mPot, int currentBet)
{
	std::string output = "";
	previousAction == ACTION_CALL ? output = "check" : output = "call";
	cout << "Horace: I " << output << "." << endl;
	cout << endl;

	if (previousAction != ACTION_CALL) //no pot contribution for a check
	{
		int amount = currentBet - GetPotContribution();
		if ((GetStack() - amount) < 0)
		{
			//recalculate
			int newAmount = GetMaxBetBeforeNegativeStack(amount);
			mPot += newAmount;
			AmendStack(-(newAmount));
			AmendPotContribution(newAmount);
			cout << "Horace is all-in." << endl;
		}
		else
		{
			mPot += amount;
			AmendStack(-(amount));
			AmendPotContribution(amount);
		}
	}
}

void AiPlayer::Fold(const int potAmount)
{
	cout << "Horace: I Fold." << endl;
	cout << "You gain the pot of " << potAmount << "chips." << endl;
	cout << endl; 

	mActionThisHand = ACTION_FOLD;
}

bool AiPlayer::Raise(ACTION& mLastActionTaken, int& mCurrentBet, int& mPot)
{
	std::string output = "";
	bool isReRaise;
	if (mLastActionTaken == ACTION_RAISE)
	{
		output = "re-raise";
		mLastActionTaken = ACTION_RERAISE;
		isReRaise = true;
	}
	else
	{
		output = "raise";
		mLastActionTaken = ACTION_RAISE;
		isReRaise = false;
	}
	cout << "Horace: I " << output << "." << endl;
	cout << endl;

	mCurrentBet *= 2;
	int amount = mCurrentBet - GetPotContribution();
	if ((GetStack() - amount) < 0)
	{
		//recalculate
		int newAmount = GetMaxBetBeforeNegativeStack(amount);
		mPot += newAmount;
		AmendStack(-(newAmount));
		AmendPotContribution(newAmount);
		cout << "Horace is all-in." << endl;
	}
	else
	{
		mPot += amount;
		AmendStack(-(amount));
		AmendPotContribution(amount);
	}
	return isReRaise;
}
