#ifndef _DECKHELPER_H
#define _DECKHELPER_H

#include "Structures.h"
#include "Player.h"
#include "AiPlayer.h"

class DeckHelper
{
public:
	DeckHelper(mt19937 &rng, vector<BasePlayer*> players);
	~DeckHelper();

	void ShuffleDeck();
	void PopulateOrderedDeck();
	void DealHands();

	void DealCard(BasePlayer* targetPlayer); 
	uint32 GetRandomDeckIndex(uint32 min, uint32 max);

	void PrintDeck();
	void PrintCard(const Card card) const;
	string CardAsString(const Card card) const;
	void PrintHand(vector<Card>* handToPrint);

	vector<Card>* GetDeck() const { return mDeck; };

private:
	vector<Card>* mDeck;

	mt19937 mRng;
	Player* mPlayer;
	AiPlayer* mAiPlayer;
};

#endif // !_DECKHELPER_H
