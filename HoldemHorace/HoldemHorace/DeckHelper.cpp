#include "DeckHelper.h"

DeckHelper::DeckHelper(mt19937 &rng, vector<BasePlayer*> players)
{
	mDeck = NULL;
	mRng = rng;

	mPlayer = (Player*)players.at(0);
	mAiPlayer = (AiPlayer*)players.at(1);
}

DeckHelper::~DeckHelper()
{
	delete mDeck;
}

//Fisher-Yates shuffle algorithm
void DeckHelper::ShuffleDeck()
{
	assert(mDeck != NULL);
	assert(mDeck->size() != 0);

	for (uint32 i = mDeck->size() - 1; i > 0; --i)
	{
		//unbiased random between 0 and i inclusive
		uint32 randSwapPos = GetRandomDeckIndex(0, i);
		//swap vals in the vector at index randSwapPos and i
		iter_swap(mDeck->begin() + randSwapPos, mDeck->begin() + i);
	}
}

//Constructs an ordered deck of cards in mDeck, aces high
void DeckHelper::PopulateOrderedDeck()
{
	if (mDeck != NULL)
	{
		delete mDeck;
	}
	mDeck = new vector<Card>();

	for (int suitIt = 0; suitIt < 4; ++suitIt)
	{
		for (int rankIt = 0; rankIt < 13; ++rankIt)
		{
			mDeck->push_back(Card(SUIT(suitIt), RANK(rankIt)));
		}
	}
}

void DeckHelper::DealHands()
{
	for (int i = 0; i < 2; ++i)
	{
		//deal card to small blind first
		if (mPlayer->GetIsOnButton())
		{
			//human is smallBlind
			DealCard(mPlayer);
			DealCard(mAiPlayer);
		}
		else
		{
			//ai is smallBlind
			DealCard(mAiPlayer);
			DealCard(mPlayer);
		}
	}
}

void DeckHelper::DealCard(BasePlayer* targetPlayer)
{
	uint32 handIndex = GetRandomDeckIndex(0, mDeck->size() - 1);
	targetPlayer->ReceiveCardFromTable(mDeck->at(handIndex));
	mDeck->erase(mDeck->begin() + handIndex);
}

//Return random index between min and max inclusive
uint32 DeckHelper::GetRandomDeckIndex(uint32 min, uint32 max)
{
	return uniform_int_distribution<uint32>{min, max}(mRng);
}

//Output deck contents to std out, ascending index order
void DeckHelper::PrintDeck()
{
	for (uint32 i = 0; i < mDeck->size(); ++i)
	{
		PrintCard(mDeck->at(i));
	}
}

void DeckHelper::PrintHand(vector<Card>* handToPrint)
{
	for (size_t i = 0; i < handToPrint->size(); ++i)
	{
		PrintCard(handToPrint->at(i));
	}
}

string DeckHelper::CardAsString(const Card card) const
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
		result += "ERROR READING CARD";
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
		result += "ERROR READING CARD";
		break;
	}

	return result;
}

//Output a Card's suit and rank to std out
void DeckHelper::PrintCard(const Card card) const
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
		result += "ERROR READING CARD";
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
		result += "ERROR READING CARD";
		break;
	}

	cout << result << endl;
}
