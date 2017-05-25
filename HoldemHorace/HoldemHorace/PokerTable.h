#ifndef _POKERTABLE_H
#define _POKERTABLE_H

#include "Structures.h" //include constants.h
#include "Texture2D.h" //includes sdl includes
#include <stdlib.h>
#include <time.h>
#include "Player.h" 
#include "AiPlayer.h" 
#include "DeckHelper.h"
#include "Enumerator.h"
#include "OpponentModeller.h" 
#include "RegretEngine.h" 
#include "FuzzyLogicFacade.h" 


//Use anttweakbar.sourceforge.net for GUI
//use github.com/actsl/kiss_sdl for gui

class Texture2D;

class PokerTable
{
public:
	PokerTable(SDL_Renderer* renderer, TTF_Font* font);
	~PokerTable();

	void Render();
	void Update(float deltaTime, SDL_Event event);

	//helper methods
	float GetRandomRealNumber(float min, float max);
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
	void SwitchFocus(const bool human, const bool horace);
	void EndHand();
	void HandlePlayerTurn(SDL_Event event);
	void HandleAITurn(SDL_Event event);
	void DetermineHandWinner();
	void DetermineHandContinuationAfterAiCall(const ACTION sourceAction);
	void ResetGame();

	string ConvertActionToString(const ACTION action) const;
	void OutputPlayerPrompt(const bool followsReraise, const int currentBet, const int costToCall) const;
	void OutputGameWinner();
	void OutputDebugText() const;
	void OutputDebugProbTriple() const;

	void SetHumanActionThisHand() const;
	void SetHoraceActionThisHand();

	//Ai decision making helper methods
	void RandomActionChoice();
	void ThreePositiveRegretsChoice(map<ACTION, double> probTriple);
	void TwoPositiveRegretsChoice();
	void SinglePositiveRegretChoice();
	vector<ACTION> GetTheTwoPositiveRegrets() const;
	void DoUnknownHoraceAction(const ACTION action);
	HANDOUTCOME DetermineHandOutcomeForFold() const;
	void SetRandomActionTakenForThisHand();
	bool ShouldGetExistingAI();
	void CumulativeRegretSetup();

	//Data Collation 
	void DataCollationSetup();
	void OutputToCSV();
	void OutputCollationVector(vector<string> theVector, ofstream& csvOutputFile);

	ACTION GetLastActionTaken() { return mLastActionTaken; }

	//SDL vars
	Texture2D* mBackgroundTexture;
	Texture2D* mCardsSpritesheet;
	Texture2D* mCardBackTexture;
	Texture2D* mButtonTexture;
	Texture2D* mCardOutlineTexture;
	SDL_Renderer* mRenderer;
	TTF_Font* mFont;

	//random number generator.
	mt19937 mRng{ random_device{}() };
	
	//poker vars
	vector<Card>* mDeck;
	int mPot;
	int mSmallBlind;
	uint32 mHandCounter;
	bool mHandInProgress;
	ACTION mLastActionTaken;
	int mBetActionCount;
	int mCurrentBet;
	bool mGameIsOver;

	//ai decision
	bool mAiCalledThisHand;
	bool mAiRaisedThisHand;

	//objects
	vector<BasePlayer*> mPlayers;
	Player* mPlayer;
	AiPlayer* mAiPlayer;
	DeckHelper* mDeckHelper;
	FuzzyLogicFacade* mFuzzyFacade;

	bool mShowStats;
	bool mShowDealPrompt;
	bool mStatPromptShown;

	bool mPromptFollowsReraise;
	bool mRenderPlayerPrompt;
	bool mRenderHoracePrompt;

	//dataCollation
	int mCsvFileNameSuffix;
	int mCurrentHandNum;
	vector<string> mCsvHandCount;
	vector<string> mCsvHandOutcomes;
	vector<string> mCsvRegretEngineHandOutcomes;
	vector<string> mCsvHoraceHands;
	vector<string> mCsvPlayerHands;
	vector<string> mCsvFoldRegrets;
	vector<string> mCsvCallRegrets;
	vector<string> mCsvRaiseRegrets;
	vector<string> mCsvHoraceActions;
	vector<string> mCsvPlaystyles;
	string mCsvGameWinner;
};
#endif // !_POKERTABLE_H
