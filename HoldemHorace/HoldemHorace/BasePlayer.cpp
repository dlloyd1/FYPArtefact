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

void BasePlayer::ReceiveCardFromTable(Card card)
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

int BasePlayer::PostBlind(int smallBlind)
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

int BasePlayer::GetMaxBetBeforeNegativeStack(int betAmount)
{
	int temp = betAmount;

	while ((mStack - temp) < 0)
	{
		temp--;
	}
	return temp;
}

void BasePlayer::PrintCard(Card card)
{
	string result = "";
	switch (card.rank)
	{
	case RANK_ACE:
		result += "A";
		break;
	case RANK_TWO:
		result += "2";
		break;
	case RANK_THREE:
		result += "3";
		break;
	case RANK_FOUR:
		result += "4";
		break;
	case RANK_FIVE:
		result += "5";
		break;
	case RANK_SIX:
		result += "6";
		break;
	case RANK_SEVEN:
		result += "7";
		break;
	case RANK_EIGHT:
		result += "8";
		break;
	case RANK_NINE:
		result += "9";
		break;
	case RANK_TEN:
		result += "T";
		break;
	case RANK_JACK:
		result += "J";
		break;
	case RANK_QUEEN:
		result += "Q";
		break;
	case RANK_KING:
		result += "K";
		break;
	default:
		result += "ERROR READING CARD RANK";
		break;
	}

	switch (card.suit)
	{
	case SUIT_HEARTS:
		result += "H";
		break;
	case SUIT_DIAMONDS:
		result += "D";
		break;
	case SUIT_SPADES:
		result += "S";
		break;
	case SUIT_CLUBS:
		result += "C";
		break;
	default:
		result += "ERROR READING CARD SUIT";
		break;
	}

	cout << result << endl;
}
