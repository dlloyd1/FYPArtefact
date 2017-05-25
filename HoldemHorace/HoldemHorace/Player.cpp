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

void Player::Render() const
{
	//render player cards here
	for (int i = 0; i < 2; ++i)
	{
		//render relevant card textures here
		Vector2D positionOnScreen = Vector2D((kScreenWidth / 2.0) - kCardWidth + i * kCardWidth, (kScreenHeight * 0.75) - kCardHeight);

		SDL_Rect destRect = { (int)positionOnScreen.x, (int)positionOnScreen.y,  kCardWidth, kCardHeight};

		//space between the cards
		if (i == 1)
		{
			destRect = { (int)positionOnScreen.x + kCardSeperationInPixels, (int)positionOnScreen.y, kCardWidth, kCardHeight};
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

void Player::Call(const ACTION previousAction, const int currentBet, int& pot)
{
	std::string output = "";
	previousAction == ACTION_CALL ? output = "check" : output = "call";
	cout << "you chose " << output << "." << endl;

	if (previousAction != ACTION_CALL) //no pot contribution for a check
	{
		int amount = currentBet - GetPotContribution();
		if ((GetStack() - amount) < 0)
		{
			//recalculate down to 0 instead
			int newAmount = GetMaxBetBeforeNegativeStack(amount);
			pot += newAmount;
			AmendStack(-(newAmount));
			AmendPotContribution(newAmount);
			cout << "You are all-in." << endl;
		}
		else
		{
			pot += amount;
			AmendStack(-(amount));
			AmendPotContribution(amount);
		}
	}
}

void Player::Raise(ACTION& mLastActionTaken, int& mCurrentBet, int& mPot)
{
	std::string output = "";
	mLastActionTaken == ACTION_RAISE ? output = "re-raise" : output = "raise";
	cout << "you chose " << output << "." << endl;
	mLastActionTaken == ACTION_RAISE ? mLastActionTaken = ACTION_RERAISE : mLastActionTaken = ACTION_RAISE;

	mCurrentBet *= 2;
	int amount = mCurrentBet - GetPotContribution();
	if ((GetStack() - amount) < 0)
	{
		//recalculate
		int newAmount = GetMaxBetBeforeNegativeStack(amount);
		mPot += newAmount;
		AmendStack(-(newAmount));
		AmendPotContribution(newAmount);
		cout << "You are all-in." << endl;
	}
	else
	{
		mPot += amount;
		AmendStack(-(amount));
		AmendPotContribution(amount);
	}
}

void Player::Fold(const int potAmount) const
{
	cout << "you chose fold." << endl;
	cout << "Horace gains the pot of " << potAmount << "chips." << endl;
}


