#include "PokerTable.h"

PokerTable::PokerTable(SDL_Renderer* renderer, TTF_Font* font)
{
	//SDL
	mRenderer = renderer;
	//font size and font type defined in main Init_SDL()
	mFont = font;
	InitTextures();

	//RNG
	srand(time(NULL));
	//seed random device
	random_device seed; 
	//instantiate random number generator from the seeded device
	mRng = mt19937(seed());
	 
	//misc vars
	mHandCounter = 0;
	mPot = 0;
	mShowStats = false;
	mShowDealPrompt = true;
	//blinds start at 1/2
	mSmallBlind = 1;
	mHandInProgress = false; 
	mLastActionTaken = ACTION_NONE;
	mGameIsOver = false;
	//ai decision vars
	mAiCalledThisHand = false;
	mAiRaisedThisHand = false;

	//objects
	mPlayer = new Player(renderer, mCardsSpritesheet, mCardOutlineTexture);
	mAiPlayer = new AiPlayer(renderer, mCardsSpritesheet, mCardOutlineTexture, mCardBackTexture);
	mPlayers.push_back(mPlayer);
	mPlayers.push_back(mAiPlayer);
	mFuzzyFacade = new FuzzyLogicFacade();
	mFuzzyFacade->SetupFuzzyLogicEngine();
	mDeckHelper = new DeckHelper(mRng, mPlayers);
	mDeckHelper->PopulateOrderedDeck();

	//for prompt rendering
	mPromptFollowsReraise = false;
	mRenderPlayerPrompt = false;
	mRenderHoracePrompt = false;
	mStatPromptShown = false;

	CumulativeRegretSetup();
	DataCollationSetup();

	//console start
	cout << "Press Spacebar key to deal" << endl;
	cout << endl;
}

PokerTable::~PokerTable()
{
	mBackgroundTexture->Free();
	mCardsSpritesheet->Free();
	mCardBackTexture->Free();
	mButtonTexture->Free();
	mCardOutlineTexture->Free();

	for each (BasePlayer* bp in mPlayers)
	{
		delete bp;
	}
	delete mDeckHelper;
	delete mFuzzyFacade;
}

void PokerTable::InitTextures()
{
	mBackgroundTexture = new Texture2D(mRenderer);
	mBackgroundTexture->LoadFromFile("Textures/BackgroundTiledLarge.png");
	mCardsSpritesheet = new Texture2D(mRenderer);
	mCardsSpritesheet->LoadFromFile("Textures/CardSpritesheet.png");
	mCardBackTexture = new Texture2D(mRenderer);
	mCardBackTexture->LoadFromFile("Textures/CardBack.png");
	mButtonTexture = new Texture2D(mRenderer);
	mButtonTexture->LoadFromFile("Textures/Button.png");
	mCardOutlineTexture = new Texture2D(mRenderer);
	mCardOutlineTexture->LoadFromFile("Textures/CardOutline.png");
}

//handles user input for loading a fresh or continued AI
bool PokerTable::ShouldGetExistingAI()
{
	std::string decision = "";
	while (decision[0] != 'Y' && decision[0] != 'y' && decision[0] != 'n' && decision[0] != 'N')
	{
		cout << "Load existing AI memory? (Y/N)" << endl;
		cin >> decision;
	}

	if (decision[0] == 'Y' || decision[0] == 'y')
	{
		return true;
	}
	return false;
}

void PokerTable::CumulativeRegretSetup()
{
	//Load existing AI or start fresh?
	if (ShouldGetExistingAI())
	{
		RegretEngine::Instance()->LoadCumulativeRegretsFromFile();
	}
	else
	{
		RegretEngine::Instance()->ZeroInstantiateCumulativeRegrets();
	}
}

void PokerTable::DataCollationSetup()
{
	//data collation vars
	mCsvFileNameSuffix = 1;
	mCurrentHandNum = 0;
	mCsvHandCount.clear();
	mCsvHandOutcomes.clear();
	mCsvRegretEngineHandOutcomes.clear();
	mCsvHoraceHands.clear();
	mCsvPlayerHands.clear();
	mCsvFoldRegrets.clear();
	mCsvCallRegrets.clear();
	mCsvRaiseRegrets.clear();
	mCsvHoraceActions.clear();
	mCsvPlaystyles.clear();
	mCsvGameWinner.clear();
}

void PokerTable::OutputToCSV()
{
	ofstream csvOutputFile;
	csvOutputFile.open("csvDataWithMemorySevenTwoOff" + to_string(mCsvFileNameSuffix) + ".csv");

	csvOutputFile << "Game Number " << to_string(mCsvFileNameSuffix) << "\n";

	csvOutputFile << "Hand,";
	OutputCollationVector(mCsvHandCount, csvOutputFile);

	csvOutputFile << "Outcome,";
	OutputCollationVector(mCsvHandOutcomes, csvOutputFile);

	csvOutputFile << "RegretEngine Outcome,";
	OutputCollationVector(mCsvRegretEngineHandOutcomes, csvOutputFile);

	csvOutputFile << "Cards,";
	OutputCollationVector(mCsvHoraceHands, csvOutputFile);

	csvOutputFile << "Opponent Cards,";
	OutputCollationVector(mCsvPlayerHands, csvOutputFile);

	csvOutputFile << "Fold Regret,";
	OutputCollationVector(mCsvFoldRegrets, csvOutputFile);

	csvOutputFile << "Call Regret,";
	OutputCollationVector(mCsvCallRegrets, csvOutputFile);

	csvOutputFile << "Raise Regret,";
	OutputCollationVector(mCsvRaiseRegrets, csvOutputFile);

	csvOutputFile << "Action,";
	OutputCollationVector(mCsvHoraceActions, csvOutputFile);

	csvOutputFile << "PlayStyle,";
	OutputCollationVector(mCsvPlaystyles, csvOutputFile);

	csvOutputFile << "Game Winner,";
	csvOutputFile << mCsvGameWinner << ",\n";
}

//helper method for CSV test output
void PokerTable::OutputCollationVector(vector<string> theVector, ofstream& csvOutputFile)
{
	for (auto s : theVector)
	{
		csvOutputFile << s << ",";
	}
	csvOutputFile << "\n";
}

//bet action taken by human player this hand, uses preference system
void PokerTable::SetHumanActionThisHand() const
{
	if (OpponentModeller::Instance()->GetHasRaisedThisHand())
	{
		OpponentModeller::Instance()->TrackRaisedHand();
	}
	else if (OpponentModeller::Instance()->GetHasCalledThisHand())
	{
		OpponentModeller::Instance()->TrackCallHand();
	}

	OpponentModeller::Instance()->SetHasRaisedThisHand(false);
	OpponentModeller::Instance()->SetHasCalledThisHand(false);
}

//bet action taken by AI this hand, uses preference system
void PokerTable::SetHoraceActionThisHand()
{
	if (mAiRaisedThisHand)
	{
		mAiPlayer->SetActionThisHand(ACTION_RAISE);
	}
	else if (mAiCalledThisHand)
	{
		mAiPlayer->SetActionThisHand(ACTION_CALL);
	}
	mAiCalledThisHand = false;
	mAiRaisedThisHand = false;
}

void PokerTable::EndHand()
{
	if (OpponentModeller::Instance()->GetActionTakenThisHand() == true)
	{
		OpponentModeller::Instance()->IncrementHandCounter();
	}
	SetHumanActionThisHand();

	mHandInProgress = false;
	CountHand();
	mCurrentHandNum++;

	//focus handled at next deal, based on button 
	SwapButton();

	//OutputDebugText();

	RegretEngine::Instance()->ModifyProbTripleByPlaystyle(mFuzzyFacade->GetHumanPlaystyle());
	
	//OutputDebugProbTriple();
	//regret engine csv hand outcomes
	switch (RegretEngine::Instance()->GetHandoutcome())
	{
	case OUTCOME_WIN:
		mCsvRegretEngineHandOutcomes.push_back("W");
		break;
	case OUTCOME_LOSE:
		mCsvRegretEngineHandOutcomes.push_back("L");
		break;
	case OUTCOME_TIE:
		mCsvRegretEngineHandOutcomes.push_back("T");
		break;
	}
	mCsvHandCount.push_back(to_string(mCurrentHandNum));
	mCsvPlayerHands.push_back(mDeckHelper->CardAsString(mPlayer->GetHand()->at(0)) + mDeckHelper->CardAsString(mPlayer->GetHand()->at(1)));
	mCsvHoraceHands.push_back(mDeckHelper->CardAsString(mAiPlayer->GetHand()->at(0)) + mDeckHelper->CardAsString(mAiPlayer->GetHand()->at(1)));

	auto map = RegretEngine::Instance()->GetCumulativeRegrets();
	mCsvFoldRegrets.push_back(to_string(map[ACTION_FOLD]));
	mCsvCallRegrets.push_back(to_string(map[ACTION_CALL]));
	mCsvRaiseRegrets.push_back(to_string(map[ACTION_RAISE]));
	mCsvHoraceActions.push_back(ConvertActionToString(mAiPlayer->GetActionThisHand()));
	mCsvPlaystyles.push_back(mFuzzyFacade->ConvertPlaystyleToString());
	//end csv data for the hand

	if ((mPlayer->GetStack() <= 0 && mAiPlayer->GetStack() > 0)
		|| mAiPlayer->GetStack() <= 0 && mPlayer->GetStack() > 0)
	{
		//a player has lost
		mGameIsOver = true;
		OutputGameWinner();
	}
	else
	{
		mShowDealPrompt = true;
		cout << endl;
		cout << "Press Spacebar key to deal" << endl;
		cout << endl;
	}
}

void PokerTable::CollectBlinds()
{
	//Take from player's stack (smallBlind if on button, big blind if not)
	for each (BasePlayer* bp in mPlayers)
	{
		int val = bp->PostBlind(mSmallBlind);
		mPot += val;
		//track these blinds in each players pot contribution this hand
		bp->AmendPotContribution(val);
	}
}

void PokerTable::HandlePlayerTurn(SDL_Event event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_f:
			mPlayer->Fold(mPot);
			OpponentModeller::Instance()->SetActionTakenThisHand(true);
			mAiPlayer->AmendStack(mPot);
			mLastActionTaken = ACTION_FOLD;
			OpponentModeller::Instance()->TrackFoldedHand();
			RegretEngine::Instance()->SetHandoutcome(DetermineHandOutcomeForFold());
			RegretEngine::Instance()->UpdateRegrets(mAiPlayer->GetActionThisHand(), RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), mAiPlayer->GetActionThisHand()));
			EndHand();
			break;
		case SDLK_c:
			mPlayer->Call(mLastActionTaken, mCurrentBet, mPot);
			OpponentModeller::Instance()->SetActionTakenThisHand(true);
			OpponentModeller::Instance()->SetHasCalledThisHand(true);
			if (mLastActionTaken == ACTION_CALL || mLastActionTaken == ACTION_RERAISE || (mLastActionTaken == ACTION_RAISE && mBetActionCount > 2))
			{
				DetermineHandWinner();
				EndHand();
			}
			else
			{
				SwitchFocus(false, true);
				cout << "Action is with Horace, press H key to progress AI" << endl;
				mBetActionCount++;
			}
			mLastActionTaken = ACTION_CALL;
			break;
		case SDLK_r:
			mPlayer->Raise(mLastActionTaken, mCurrentBet, mPot);
			OpponentModeller::Instance()->SetActionTakenThisHand(true);
			SwitchFocus(false, true);
			cout << "Action is with Horace, press H key to progress AI." << endl;
			mBetActionCount++;
			OpponentModeller::Instance()->SetHasRaisedThisHand(true);
			break;
		}
		break;
	}
}

/////////////////start AI decision handling/////////////////
void PokerTable::HandleAITurn(SDL_Event event)
{				
	if (!mGameIsOver)
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_h:

				//modifiable copy to adjust for -ve regrets
				map<ACTION, double> probTriple = RegretEngine::Instance()->GetProbabilityTriple();

				//Count number of +ve regrets.
				int positiveRegrets = 0;
				for (const auto& pair : probTriple)
				{
					if (pair.second > 0)
					{
						positiveRegrets++;
					}
				}
				//If it's 1, take that action
				//if it's two, calculate with both probabilities
				//it it's three, take action proportionate to positive regrets, normalized
				//or if it's 0, just pick an action with equal prob% distribution
				switch (positiveRegrets)
				{
				case 3:
					ThreePositiveRegretsChoice(probTriple);
					break;
				case 2:
					//cout << "NOTE: TWO POSITIVE TRIGGERED" << endl;
					TwoPositiveRegretsChoice();
					break;
				case 1:
					//cout << "NOTE: SINGLE POSITIVE TRIGGERED" << endl;
					SinglePositiveRegretChoice();

					break;
				case 0:
					//cout << "NOTE: ZERO POSITIVE TRIGGERED" << endl;
					RandomActionChoice();
					break;
				}
				break;
			}
			break;
		}
	}
}

vector<ACTION> PokerTable::GetTheTwoPositiveRegrets() const
{
	vector<ACTION> positives;

	map <ACTION, double> probTriple = RegretEngine::Instance()->GetProbabilityTriple();
	for(const auto& pair : probTriple)
	{
		if (pair.second > 0)
		{
			positives.push_back(pair.first);
		}
	}

	return positives;
}

void PokerTable::SinglePositiveRegretChoice()
{
	map<ACTION, double> probTripleCopy = RegretEngine::Instance()->GetProbabilityTriple();

	if (probTripleCopy[ACTION_FOLD] > 0)
	{
		mAiPlayer->Fold(mPot);
		RegretEngine::Instance()->SetHandoutcome(DetermineHandOutcomeForFold());
		RegretEngine::Instance()->UpdateRegrets(ACTION_FOLD, RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), ACTION_FOLD));
		mPlayer->AmendStack(mPot);
		mLastActionTaken = ACTION_FOLD;
		mCsvHandOutcomes.push_back("L");
		EndHand();
	}
	else if (probTripleCopy[ACTION_CALL] > 0)
	{
		mAiPlayer->Call(mLastActionTaken, mPot, mCurrentBet);
		mAiCalledThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_CALL);
	}
	else if(probTripleCopy[ACTION_RAISE] > 0)
	{
		mAiPlayer->Raise(mLastActionTaken, mCurrentBet, mPot);
		mAiRaisedThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_RAISE);
	}
	else
	{
		cout << "ERROR: Regret Engine is broken, please investigate." << endl;
	}
}

void PokerTable::DoUnknownHoraceAction(const ACTION action)
{
	if (action == ACTION_FOLD)
	{
		mAiPlayer->Fold(mPot);
		RegretEngine::Instance()->SetHandoutcome(DetermineHandOutcomeForFold());
		RegretEngine::Instance()->UpdateRegrets(ACTION_FOLD, RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), ACTION_FOLD));
		mPlayer->AmendStack(mPot);
		mLastActionTaken = ACTION_FOLD;
		mCsvHandOutcomes.push_back("L");
		EndHand();
	}
	else if (action == ACTION_CALL)
	{
		mAiPlayer->Call(mLastActionTaken, mPot, mCurrentBet);
		mAiCalledThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_CALL);
	}
	else if (action == ACTION_RAISE)
	{
		mAiPlayer->Raise(mLastActionTaken, mCurrentBet, mPot);
		mAiRaisedThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_RAISE);
	}
}

void PokerTable::TwoPositiveRegretsChoice()
{
	vector<ACTION> positives = GetTheTwoPositiveRegrets();
	map<ACTION, double> probTriple = RegretEngine::Instance()->GetProbabilityTriple();

	double denominator = probTriple[positives[0]] + probTriple[positives[1]];
	for (auto a : positives)
	{
		probTriple[a] /= denominator; //normalise their occurence %
	}

	double decision = (double)GetRandomRealNumber(0.01f, 1.0f);
	if (decision < probTriple[positives[0]])
	{
		DoUnknownHoraceAction(positives[0]);

	}
	else if (decision < (probTriple[positives[0]] + probTriple[positives[1]]))
	{
		DoUnknownHoraceAction(positives[1]);
	}
}

void PokerTable::ThreePositiveRegretsChoice(map<ACTION, double> probTriple)
{
	double dDecision = (double)GetRandomRealNumber(0.01f, 1.0f);

	if (dDecision < probTriple[ACTION_FOLD])
	{
		mAiPlayer->Fold(mPot);
		RegretEngine::Instance()->SetHandoutcome(DetermineHandOutcomeForFold());
		RegretEngine::Instance()->UpdateRegrets(ACTION_FOLD, RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), ACTION_FOLD));
		mPlayer->AmendStack(mPot);
		mLastActionTaken = ACTION_FOLD;
		mCsvHandOutcomes.push_back("L");
		EndHand();
	}
	else if (dDecision < (probTriple[ACTION_CALL] + probTriple[ACTION_FOLD]))
	{
		mAiPlayer->Call(mLastActionTaken, mPot, mCurrentBet);
		mAiCalledThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_CALL);
	}
	else if (dDecision < (probTriple[ACTION_RAISE] + probTriple[ACTION_CALL] + probTriple[ACTION_FOLD]))
	{
		mAiPlayer->Raise(mLastActionTaken, mCurrentBet, mPot);
		mAiRaisedThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_RAISE);
	}
}

void PokerTable::RandomActionChoice()
{
	float decision = GetRandomRealNumber(0.01f, 1.0f);
	if (decision < 0.33f)
	{
		mAiPlayer->Fold(mPot);
		RegretEngine::Instance()->SetHandoutcome(DetermineHandOutcomeForFold());
		RegretEngine::Instance()->UpdateRegrets(ACTION_FOLD, RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), ACTION_FOLD));
		mPlayer->AmendStack(mPot);
		mLastActionTaken = ACTION_FOLD;
		mCsvHandOutcomes.push_back("L");
		EndHand();
	}
	else if (decision < 0.66f)
	{
		mAiPlayer->Call(mLastActionTaken, mPot, mCurrentBet);
		mAiCalledThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_CALL);
	}
	else
	{
		mAiPlayer->Raise(mLastActionTaken, mCurrentBet, mPot);
		mAiRaisedThisHand = true;
		DetermineHandContinuationAfterAiCall(ACTION_RAISE);
	}
}
/////////////////end AI decision handling/////////////////

void PokerTable::DetermineHandContinuationAfterAiCall(const ACTION sourceAction)
{
	if (mLastActionTaken == ACTION_CALL || mLastActionTaken == ACTION_RERAISE || mLastActionTaken == ACTION_RAISE && mBetActionCount > 2)
	{
		DetermineHandWinner();
		EndHand();
	}
	else //call does not end hand following an ACTION_NONE (first to act in hand), or an ACTION_RAISE with < 3 actions having taken place (check still needed to end hand then)
	{
		mBetActionCount++;
		SwitchFocus(true, false);

		mPromptFollowsReraise = false;
		OutputPlayerPrompt(false, mCurrentBet, mCurrentBet - mPlayer->GetPotContribution());
		mLastActionTaken = sourceAction;
	}
}

//handoutcome for a fold from AI's perspective, for the regret engine
HANDOUTCOME PokerTable::DetermineHandOutcomeForFold() const
{
	float playerStr = Enumerator::Instance()->GetWinPercentageOfHand(mPlayer->GetHand());
	float horaceStr = Enumerator::Instance()->GetWinPercentageOfHand(mAiPlayer->GetHand());
	if (playerStr > horaceStr)
	{
		return OUTCOME_LOSE;
	}
	else if (horaceStr > playerStr)
	{
		return OUTCOME_WIN;
	}
	else if (playerStr == horaceStr)
	{
		return OUTCOME_TIE;
	}
}

void PokerTable::DetermineHandWinner()
{
	float playerStr = Enumerator::Instance()->GetWinPercentageOfHand(mPlayer->GetHand());
	float horaceStr = Enumerator::Instance()->GetWinPercentageOfHand(mAiPlayer->GetHand());

	SetHoraceActionThisHand();

	if (playerStr > horaceStr)
	{
		//player wins hand
		//So set Ai's outcome for the regrets
		RegretEngine::Instance()->SetHandoutcome(OUTCOME_LOSE);
		RegretEngine::Instance()->UpdateRegrets(mAiPlayer->GetActionThisHand(), RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), mAiPlayer->GetActionThisHand()));

		cout << "You won the hand." << endl;
		mPlayer->AmendStack(mPot);

		cout << "Horace had: " << endl;
		mDeckHelper->PrintHand(mAiPlayer->GetHand());
		mCsvHandOutcomes.push_back("L");
	}
	else if (horaceStr > playerStr)
	{
		//Ai wins hand
		RegretEngine::Instance()->SetHandoutcome(OUTCOME_WIN);
		RegretEngine::Instance()->UpdateRegrets(mAiPlayer->GetActionThisHand(), RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), mAiPlayer->GetActionThisHand()));

		mAiPlayer->AmendStack(mPot);
		cout << "Horace won the hand, with: " << endl;
		mDeckHelper->PrintHand(mAiPlayer->GetHand());
		mCsvHandOutcomes.push_back("W");
	}
	else if (playerStr == horaceStr)
	{
		RegretEngine::Instance()->SetHandoutcome(OUTCOME_TIE);
		RegretEngine::Instance()->UpdateRegrets(mAiPlayer->GetActionThisHand(), RegretEngine::Instance()->ConstructActionOutcomeRegretMap(RegretEngine::Instance()->GetHandoutcome(), mAiPlayer->GetActionThisHand()));

		cout << "pot was split, tied hand" << endl;
		//split pot
		if (mPot % 2 == 0)
		{
			mPlayer->AmendStack(mPot / 2);
			mAiPlayer->AmendStack(mPot / 2);
		}
		else
		{
			float halfVal = (float)mPot / 2.0f;
			mPlayer->AmendStack((int)(halfVal + 0.5f));
			mAiPlayer->AmendStack((int)(halfVal + 0.5f));
		}
		mCsvHandOutcomes.push_back("T");
	}
	mPot = 0;
}

void PokerTable::ResetGame()
{
	for each (BasePlayer* bp in mPlayers)
	{
		bp->ResetPlayer();
	}

	mHandCounter = 0;
	mPot = 0;
	mSmallBlind = 1;
	mHandInProgress = false;
	mLastActionTaken = ACTION_NONE;
	mGameIsOver = false;
	mDeckHelper->PopulateOrderedDeck();

	//data collation:
	mCsvFileNameSuffix++;
	mCurrentHandNum = 0;
	mCsvHandCount.clear();
	mCsvHandOutcomes.clear();
	mCsvRegretEngineHandOutcomes.clear();
	mCsvHoraceHands.clear();
	mCsvPlayerHands.clear();
	mCsvFoldRegrets.clear();
	mCsvCallRegrets.clear();
	mCsvRaiseRegrets.clear();
	mCsvHoraceActions.clear();
	mCsvPlaystyles.clear();
	mCsvGameWinner.clear();
}

void PokerTable::Render()
{
	RenderTable();
	mPlayer->Render();
	mAiPlayer->Render();
	RenderInfoText();
}

void PokerTable::Update(float deltaTime, SDL_Event event)
{
	if (!mHandInProgress)
	{
		if (mShowDealPrompt && (mAiPlayer->GetHand() != NULL))
		{
			mAiPlayer->SetShowCards(true);
		}
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				if (!mGameIsOver)
				{
				case SDLK_SPACE: //spacebar to deal hand
					if (mStatPromptShown == false)
					{
						cout << "Press S to toggle showing AI stats" << endl;
						cout << endl;
						mStatPromptShown = true;
					}

					SetHandStartVariables();

					for each (BasePlayer* bp in mPlayers)
					{
						bp->ClearHand();
					}

					mDeckHelper->PopulateOrderedDeck();
					mDeckHelper->ShuffleDeck();

					CollectBlinds();
					mBetActionCount++; //allow the blinds to put us at action 1, so first betting action check returns 1

					mDeckHelper->DealHands();
				
					if (mPlayer->GetIsOnButton())
					{
						mRenderPlayerPrompt = true;
						mRenderHoracePrompt = false;
						mPromptFollowsReraise = false;
						OutputPlayerPrompt(false, mCurrentBet, (mCurrentBet - mPlayer->GetPotContribution()));
					}
					else // Horace on button
					{
						mRenderPlayerPrompt = false;
						mRenderHoracePrompt = true;
						cout << "Action is with Horace, press H key to progress AI." << endl;
					}

					SwitchFocus(mPlayer->GetIsOnButton(), mAiPlayer->GetIsOnButton()); //small blind starts betting each hand
					break;
				}
				else 				
				{
				case SDLK_p: //r key to reset
					ResetGame();
					mGameIsOver = false;
					break;

					//automation functionality
				/*	if (mCsvFileNameSuffix != 11)
					{
						OutputToCSV();
						ResetGame();
					}
					else
					{
						cout << "Automation finished" << endl;
					}*/
				}
			}
			break;
		}
	}
	else 
	{
		if (mPlayer->GetHasFocus())
		{
			mRenderPlayerPrompt = true;
			mRenderHoracePrompt = false;
			HandlePlayerTurn(event);
		}
		else
		{
			mRenderPlayerPrompt = false;
			mRenderHoracePrompt = true;
			//Determine Human Playstyle
			mFuzzyFacade->CategoriseHumanPlaystyle();
			HandleAITurn(event);
		}
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{

			case SDLK_s: //toggling stat tracking display
				mShowStats = !mShowStats;
				break;
			}
			break;
		}
	}
}

void PokerTable::SwitchFocus(const bool human, const bool horace)
{
	mPlayer->SetHasFocus(human);
	mAiPlayer->SetHasFocus(horace);
}

void PokerTable::SetHandStartVariables()
{
	for each (BasePlayer* bp in mPlayers)
	{
		bp->ResetPotContribution();
	}
	mShowDealPrompt = false;
	mAiPlayer->SetShowCards(false);
	mBetActionCount = 0;
	mCurrentBet = mSmallBlind * 2;
	mLastActionTaken = ACTION_NONE;
	SetRandomActionTakenForThisHand();

	mHandInProgress = true;
	mPot = 0;
	OpponentModeller::Instance()->SetActionTakenThisHand(false);
}

//choose random action for regret calculation if player folds before Ai takes an action
void PokerTable::SetRandomActionTakenForThisHand()
{
	float randomDec = GetRandomRealNumber(0.1f, 0.99f);
	if (randomDec <= 0.33f)
	{
		mAiPlayer->SetActionThisHand(ACTION_FOLD);
	}
	else if (randomDec <= 0.66f)
	{
		mAiPlayer->SetActionThisHand(ACTION_CALL);
	}
	else if (randomDec <= 0.99f)
	{
		mAiPlayer->SetActionThisHand(ACTION_RAISE);
	}
}

void PokerTable::CountHand()
{
	//blinds go up
	mHandCounter++;
	if (mHandCounter >= kHandsPerBlind)
	{
		IncreaseBlinds();
	}
}

void PokerTable::IncreaseBlinds()
{
	cout << "Blinds going up." << endl;
	mHandCounter = 0;
	mSmallBlind += kSmallBlindIncrementAmount; 
}

void PokerTable::SwapButton()
{
	//invert booleans
	for each (BasePlayer* bp in mPlayers)
	{
		bp->SetIsOnButton(!bp->GetIsOnButton());
	}
}

void PokerTable::RenderSomeText(SDL_Surface* surface, SDL_Texture* texture, string text, SDL_Color color, int screenXLoc, int screenYLoc, bool bottomEdge, bool rightEdge)
{
	surface = TTF_RenderText_Solid(mFont, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(mRenderer, surface);
	int width = 0;
	int height = 0;
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
	if (bottomEdge)
	{
		screenYLoc = kScreenHeight - height;
	}
	if (rightEdge)
	{
		screenXLoc = kScreenWidth - width;
	}
	SDL_Rect destRect = { screenXLoc, screenYLoc, width, height };
	SDL_RenderCopy(mRenderer, texture, NULL, &destRect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

void PokerTable::RenderInfoText()
{
	SDL_Color color = { 224, 224, 224 };
	SDL_Surface* textSurface = NULL;
	SDL_Texture* textTexture = NULL;

	RenderSomeText(textSurface, textTexture, std::string("Pot: " + to_string(mPot)), color, 0 + (kScreenWidth * 0.475), kScreenHeight / 2, false, false);
	RenderSomeText(textSurface, textTexture, std::string("Blinds: " + to_string(mSmallBlind) + "/" + to_string(mSmallBlind * 2)).c_str(), color, 0 + (kScreenWidth * 0.462), 0 + (kScreenHeight * 0.15), false, false);
	RenderSomeText(textSurface, textTexture, std::string("Chips: " + to_string(mPlayer->GetStack())).c_str(), color, 0 + (kScreenWidth * 0.3), 0 + (kScreenHeight * 0.7), false, false);
	RenderSomeText(textSurface, textTexture, std::string("AI Chips: " + to_string(mAiPlayer->GetStack())).c_str(), color, 0 + (kScreenWidth * 0.60), 0 + (kScreenHeight * 0.325), false, false);
	//if show deal prompt
	//render spacebar key to deal
	if (mShowDealPrompt)
	{
		RenderSomeText(textSurface, textTexture, "Press Spacebar key to deal.", color, (kScreenWidth / 2) - 110, 0, true, false);
	}
	else if (mRenderPlayerPrompt)
	{
		RenderSomeText(textSurface, textTexture, "Bet is at: " + to_string(mCurrentBet), color, (kScreenWidth / 2) - 40, kScreenHeight - 6*kVertTextSeperation, false, false);
		RenderSomeText(textSurface, textTexture, "F = Fold", color, (kScreenWidth / 2) - 40, kScreenHeight - 5*kVertTextSeperation, false, false);
		if ((mCurrentBet - mPlayer->GetPotContribution()) > 0)
		{
			RenderSomeText(textSurface, textTexture, "C = Call (-" + to_string(mCurrentBet - mPlayer->GetPotContribution()) + ")", color, (kScreenWidth / 2) - 40, kScreenHeight - 4*kVertTextSeperation, false, false);
		}
		else
		{
			RenderSomeText(textSurface, textTexture, "C = Check ", color, (kScreenWidth / 2) - 40, kScreenHeight - 4*kVertTextSeperation, false, false);
		}
		if (!mPromptFollowsReraise)
		{
			RenderSomeText(textSurface, textTexture, "R = Raise (-" + to_string((2*mCurrentBet) - mPlayer->GetPotContribution()) + ")" , color, (kScreenWidth / 2) - 40, kScreenHeight - 3*kVertTextSeperation, false, false);
		}
	}
	else if (mRenderHoracePrompt)
	{
		RenderSomeText(textSurface, textTexture, "H = Progress AI decision", color, (kScreenWidth / 2) - 100, kScreenHeight - 3*kVertTextSeperation, false, false);
	}

	//top right stat info
	color = { 255,255, 20 };
	if (mShowStats)
	{
		//topright stats
		RenderSomeText(textSurface, textTexture, std::string("Playstyle tracked: " + mFuzzyFacade->ConvertPlaystyleToString()), color, 0, 0, false, true);
		RenderSomeText(textSurface, textTexture, std::string("Call%: " + to_string(OpponentModeller::Instance()->GetCallPercentage())).c_str(), color, 0, kVertTextSeperation, false, true);
		RenderSomeText(textSurface, textTexture, std::string("Raise%: " + to_string(OpponentModeller::Instance()->GetRaisePercentage())).c_str(), color, 0, 2 * kVertTextSeperation, false, true);
		RenderSomeText(textSurface, textTexture, std::string("Fold%: " + to_string(OpponentModeller::Instance()->GetFoldPercentage())).c_str(), color, 0, 3 * kVertTextSeperation, false, true);
		RenderSomeText(textSurface, textTexture, std::string("Raise|Call Ratio: " + to_string(OpponentModeller::Instance()->CalculateRaiseToCallRatio())).c_str(), color, 0, 4 * kVertTextSeperation, false, true);
		map<ACTION, int> regretMap = RegretEngine::Instance()->GetCumulativeRegrets();
		map<ACTION, double> probTriple = RegretEngine::Instance()->GetProbabilityTriple();
		RenderSomeText(textSurface, textTexture, std::string("Fold Regret: " + to_string(regretMap[ACTION_FOLD]) + " (" + to_string(probTriple[ACTION_FOLD]) + "%)").c_str(), color, 0, 5 * kVertTextSeperation, false, true);
		RenderSomeText(textSurface, textTexture, std::string("Call Regret: " + to_string(regretMap[ACTION_CALL]) + " (" + to_string( probTriple[ACTION_CALL]) + "%)").c_str(), color, 0, 6 * kVertTextSeperation, false, true);
		RenderSomeText(textSurface, textTexture, std::string("Raise Regret: " + to_string(regretMap[ACTION_RAISE]) + " (" + to_string(probTriple[ACTION_RAISE]) + "%)").c_str(), color, 0, 7 * kVertTextSeperation, false, true);
	}

	//cleanup
	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);
}

void PokerTable::RenderTable()
{
	mBackgroundTexture->Render(Vector2D());

	if (mPlayer->GetIsOnButton())
	{
		mButtonTexture->Render(Vector2D((kScreenWidth / 2.0) - kCardWidth * 2, (kScreenHeight * 0.75) - kCardHeight));
	}
	else
	{
		mButtonTexture->Render(Vector2D((kScreenWidth / 2.0) - kCardWidth * 2, (kScreenHeight * 0.25) + kCardHeight));
	}
}

//HELPER METHODS BELOW//
float PokerTable::GetRandomRealNumber(float min, float max)
{
	return uniform_real_distribution<float>{min, max}(mRng);
}

void PokerTable::OutputPlayerPrompt(const bool followsReraise, const int currentBet, const int costToCall) const
{
	cout << "Action is with you. Bet is at : " << to_string(currentBet) << " Press: " << endl;
	cout << "F to Fold" << endl;
	if (costToCall > 0)
	{
		cout << "C to Call, cost to call: " << to_string(costToCall) << endl;
	}
	else
	{
		cout << "C to check." << endl;
	}
	if (!followsReraise)
	{
		cout << "R to Raise to: " << to_string(2 * currentBet) << ", cost to raise: " << to_string((2 * currentBet) - mPlayer->GetPotContribution()) << endl;
	}
}

void PokerTable::OutputGameWinner()
{
	cout << "Game is over.";
	if (mPlayer->GetStack() <= 0 && mAiPlayer->GetStack() > 0)
	{
		//horace won
		cout << " Horace won." << endl;
		mCsvGameWinner = std::string("AI");
	}
	else if (mAiPlayer->GetStack() <= 0 && mPlayer->GetStack() > 0)
	{
		//player won
		cout << " You won." << endl;
		mCsvGameWinner = std::string("Player");
	}
	else
	{
		//tie?
		cout << "that's weird, you shouldn't see this output" << endl;
	}

	cout << "Press P key to restart." << endl;
}

void PokerTable::OutputDebugProbTriple() const
{
	auto pt = RegretEngine::Instance()->GetProbabilityTriple();

	cout << "probability triple is now: " << endl;
	for (auto p : pt)
	{
		cout << p.second << endl;
	}
}

void PokerTable::OutputDebugText() const
{
	cout << endl;
	cout << "Total Hands so far - " << to_string(OpponentModeller::Instance()->GetTotalHands()) << endl;
	cout << endl;
	cout << "Called so far - " << to_string(OpponentModeller::Instance()->GetCalled()) << endl;
	cout << "Call% - " << to_string(OpponentModeller::Instance()->GetCallPercentage()) << endl;
	cout << endl;
	cout << "Raised so far - " << to_string(OpponentModeller::Instance()->GetRaised()) << endl;
	cout << "Raise% - " << to_string(OpponentModeller::Instance()->GetRaisePercentage()) << endl;
	cout << endl;
	cout << "Folded so far - " << to_string(OpponentModeller::Instance()->GetFolded()) << endl;
	cout << "Fold% - " << to_string(OpponentModeller::Instance()->CalculateHandFoldPercentage()) << endl;
	cout << endl;
	cout << "Raise to Call Ratio - " << to_string(OpponentModeller::Instance()->CalculateRaiseToCallRatio()) << endl;
	cout << endl;
	cout << "Human playstyle - " << mFuzzyFacade->ConvertPlaystyleToString() << endl;
	cout << endl;
	cout << "Horace action this hand recorded as - " << ConvertActionToString(mAiPlayer->GetActionThisHand()) << endl;
}

string PokerTable::ConvertActionToString(const ACTION action) const
{
	switch (action)
	{
	case ACTION_CALL:
		return "CALL";
	case ACTION_FOLD:
		return "FOLD";
	case ACTION_RAISE:
		return "RAISE";
	default:
		return "Not one of the main three actions";
	}
}




