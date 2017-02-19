#ifndef _POKERTABLE_H
#define _POKERTABLE_H

#include "Structures.h"
#include "Constants.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <iostream>
#include "Texture2D.h"
#include <cassert> 
#include <algorithm>

#include <random>
#include <stdlib.h>
#include <time.h>
#include "Player.h" //includes BasePlayer
#include "AiPlayer.h" //includes BasePlayer
#include "DeckHelper.h"
#include "Enumerator.h"

using namespace std;

class Texture2D;

#define mEnumerator Enumerator::Instance()

class PokerTable
{
public:
	PokerTable(SDL_Renderer* renderer, TTF_Font* font);
	~PokerTable();

	void Render();
	void Update(float deltaTime, SDL_Event event);
private:
	void InitTextures();
	void RenderTable();
	void RenderInfoText();
	void RenderSomeText(SDL_Surface* surface, SDL_Texture* texture, string text, SDL_Color color, 
		int screenXLoc, int screenYLoc, bool bottomEdge, bool rightEdge);

	void CollectBlinds();
	void IncreaseBlinds();
	void CountHand();
	void SwapButton();
	void SetHandStartVariables();
	void SwitchFocus(bool human, bool horace);
	void EndHand();
	void OutputPlayerPrompt(bool followsReraise, int currentBet, int costToCall);
	void OutputGameWinner();
	void HandlePlayerTurn(SDL_Event event);
	void HandleAITurn(SDL_Event event);
	void DetermineHandWinner();
	void ResetGame();


	void Fold(bool calledByAI);
	void PlayerCall(ACTION previousAction);
	void PlayerRaise(ACTION previousAction);
	void AICall(ACTION previousAction);
	void AIRaise(ACTION previousAction);

	float GetRandomRealNumber(float min, float max);

	ACTION GetLastActionTaken() { return mLastActionTaken; }

	Texture2D* mBackgroundTexture;
	Texture2D* mCardsSpritesheet;
	Texture2D* mCardBackTexture;
	Texture2D* mButtonTexture;
	Texture2D* mCardOutlineTexture;

	SDL_Renderer* mRenderer;
	TTF_Font* mFont;

	float mTurnTimer;

	//random number generator.
	mt19937 mRng{ random_device{}() };


	vector<BasePlayer*> mPlayers; //polymorphic container for referencing both, human is at index0
	Player* mPlayer;
	AiPlayer* mAiPlayer;

	vector<Card>* mDeck;
	int mPot;
	int mSmallBlind;


	//incrementing blinds.
	uint32 mHandCounter;

	bool mHandInProgress;

	ACTION mLastActionTaken;
	int mBetActionCount;
	int mCurrentBet;
	
	DeckHelper* mDeckHelper;

	bool mGameIsOver;
};
#endif // !_POKERTABLE_H
