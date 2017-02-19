#include "PokerTable.h"

//@TODO: prevent other player taking action when at 0 chips
PokerTable::PokerTable(SDL_Renderer* renderer, TTF_Font* font)
{
	srand(time(NULL));

	mRenderer = renderer;
	//font size and font type defined in main Init_SDL()
	mFont = font;
	//seed random device
	random_device seed; 
	//instantiate random number generator from the seeded device
	mRng = mt19937(seed());

	InitTextures();

	mPlayer = new Player(renderer, mCardsSpritesheet, mCardOutlineTexture);
	mAiPlayer = new AiPlayer(renderer, mCardsSpritesheet, mCardOutlineTexture, mCardBackTexture);

	mPlayers.push_back(mPlayer);
	mPlayers.push_back(mAiPlayer);

	mDeckHelper = new DeckHelper(mRng, mPlayers);

	mHandCounter = 0;

	mTurnTimer = 0.0f;

	mPot = 0;
	//blinds start at 1/2
	mSmallBlind = 1;

	mDeckHelper->PopulateOrderedDeck();

	mHandInProgress = false;
	
	mLastActionTaken = ACTION_NONE;

	mGameIsOver = false;
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
}

void PokerTable::InitTextures()
{
	mBackgroundTexture = new Texture2D(mRenderer);
	mBackgroundTexture->LoadFromFile("Textures/BackgroundTiled.png");
	mCardsSpritesheet = new Texture2D(mRenderer);
	mCardsSpritesheet->LoadFromFile("Textures/CardSpritesheet.png");
	mCardBackTexture = new Texture2D(mRenderer);
	mCardBackTexture->LoadFromFile("Textures/CardBack.png");
	mButtonTexture = new Texture2D(mRenderer);
	mButtonTexture->LoadFromFile("Textures/Button.png");
	mCardOutlineTexture = new Texture2D(mRenderer);
	mCardOutlineTexture->LoadFromFile("Textures/CardOutline.png");
}

void PokerTable::OutputPlayerPrompt(bool followsReraise, int currentBet, int costToCall)
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
	}
	else if(mAiPlayer->GetStack() <= 0 && mPlayer->GetStack() > 0)
	{
		//player won
		cout << " You won." << endl;
	}
	else
	{
		//tie?
		cout << "that's weird, breakpoint line105 PokerTable.cpp" << endl;
	}

	cout << "Press R key to restart." << endl;
}

void PokerTable::EndHand()
{
	mHandInProgress = false;
	CountHand();
	//focus handled at next deal, based on button 
	SwapButton();
	if ((mPlayer->GetStack() <= 0 && mAiPlayer->GetStack() > 0)
		|| mAiPlayer->GetStack() <= 0 && mPlayer->GetStack() > 0)
	{
		//a player has lost
		mGameIsOver = true;
		OutputGameWinner();
	}
	else
	{
		cout << endl;
		cout << "Press Spacebar key to deal" << endl;
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
			/*PlayerFold();*/
			Fold(false);
			break;
		case SDLK_c:
			PlayerCall(mLastActionTaken);
			break;
		case SDLK_r:
			PlayerRaise(mLastActionTaken);
			break;
		}
		break;
	}
}

void PokerTable::PlayerCall(ACTION previousAction)
{
	std::string output = "";
	previousAction == ACTION_CALL ? output = "check" : output = "call";
	cout << "you chose " << output << "." << endl;
	mLastActionTaken = ACTION_CALL;

	if (previousAction != ACTION_CALL) //no pot contribution for a check
	{
		int amount = mCurrentBet - mPlayer->GetPotContribution();
		if ((mPlayer->GetStack() - amount) < 0)
		{
			//recalculate down to 0 instead
			int newAmount = mPlayer->GetMaxBetBeforeNegativeStack(amount);
			mPot += newAmount;
			mPlayer->AmendStack(-(newAmount));
			mPlayer->AmendPotContribution(newAmount);
		}
		else
		{
			mPot += amount;
			mPlayer->AmendStack(-(amount));
			mPlayer->AmendPotContribution(amount);
		}
	}

	if (previousAction == ACTION_CALL || previousAction == ACTION_RERAISE || previousAction == ACTION_RAISE && mBetActionCount > 2)
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
}

void PokerTable::PlayerRaise(ACTION previousAction)
{
	std::string output = "";
	previousAction == ACTION_RAISE ? output = "re-raise" : output = "raise";
	cout << "you chose " << output << "." << endl;
	previousAction == ACTION_RAISE ? mLastActionTaken = ACTION_RERAISE : mLastActionTaken = ACTION_RAISE;

	mCurrentBet *= 2;
	int amount = mCurrentBet - mPlayer->GetPotContribution();
	if ((mPlayer->GetStack() - amount) < 0)
	{
		//recalculate
		int newAmount = mPlayer->GetMaxBetBeforeNegativeStack(amount);
		mPot += newAmount;
		mPlayer->AmendStack(-(newAmount));
		mPlayer->AmendPotContribution(newAmount);
	}
	else
	{
		mPot += amount;
		mPlayer->AmendStack(-(amount));
		mPlayer->AmendPotContribution(amount);
	}
	
	SwitchFocus(false, true);
	cout << "Action is with Horace, press H key to progress AI." << endl;
	mBetActionCount++;
}

void PokerTable::HandleAITurn(SDL_Event event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_h:
			if (mLastActionTaken == ACTION_NONE)
			{
				//ai first to act in hand -FOLD
				//AIFold();
				Fold(true);

				//ai first to act in hand - RAISE
				//AIRaise(mLastActionTaken);

				//ai first to act in hand - CALL
				//AICall(mLastActionTaken);
			}
			else if (mLastActionTaken == ACTION_CALL)
			{
				//float decision = GetRandomRealNumber(0.0f, 1.0f);
				float decision = 0.34f;

				if (decision > 0.0f && decision <= 0.33f)
				{
					//33% check
					AICall(mLastActionTaken);
				}
				else if (decision > 0.33f && decision <= 0.66f)
				{
					//33% fold
					/*AIFold();*/
					Fold(true);
				}
				else if (decision > 0.66f && decision <= 1.0f)
				{
					//33% raise
					AIRaise(mLastActionTaken);
				}
			}
			else if (mLastActionTaken == ACTION_RAISE)
			{
				//float decision = GetRandomRealNumber(0.0f, 1.0f);
				float decision = 0.01f;

				if (decision > 0.0f && decision <= 0.33f)
				{
					/*AIFold();*/
					Fold(true);
				}
				else if (decision > 0.33f && decision <= 0.66f)
				{
					//33% call
					AICall(mLastActionTaken);
				}
				else if (decision > 0.66f)
				{
					//33% re-raise
					AIRaise(mLastActionTaken);
				}
			}
			else if (mLastActionTaken == ACTION_RERAISE)
			{
				//cout << "Re-raise logic detected" << endl;

				/*float decision = GetRandomRealNumber(0.0f, 1.0f);*/
				float decision = 0.6f;

				if (decision >= 0.5f)
				{
					//50% fold
					/*AIFold();*/
					Fold(true);
				}
				else
				{
					//50% call
					AICall(mLastActionTaken);
				}
			}
			break;
		}
		break;
	}
}

void PokerTable::Fold(bool calledByAI)
{
	if (calledByAI) //Horace folds to you
	{
		cout << "Horace: I Fold." << endl;
		mPlayer->AmendStack(mPot);
	}
	else //you fold to Horace
	{
		cout << "you chose fold." << endl;
		mAiPlayer->AmendStack(mPot);
	}
	mLastActionTaken = ACTION_FOLD;
	EndHand();
}

void PokerTable::AICall(ACTION previousAction)
{
	std::string output = "";
	previousAction == ACTION_CALL ? output = "check" : output = "call";
	cout << "Horace: I " << output << "." << endl;
	mLastActionTaken = ACTION_CALL;

	if (previousAction != ACTION_CALL) //no pot contribution for a check
	{
		int amount = mCurrentBet - mAiPlayer->GetPotContribution();
		if ((mAiPlayer->GetStack() - amount) < 0)
		{
			//recalculate
			int newAmount = mAiPlayer->GetMaxBetBeforeNegativeStack(amount);
			mPot += newAmount;
			mAiPlayer->AmendStack(-(newAmount));
			mAiPlayer->AmendPotContribution(newAmount);
		}
		else
		{
			mPot += amount;
			mAiPlayer->AmendStack(-(amount));
			mAiPlayer->AmendPotContribution(amount);
		}
	}

	if (previousAction == ACTION_CALL || previousAction == ACTION_RERAISE || previousAction == ACTION_RAISE && mBetActionCount > 2)
	{
		DetermineHandWinner();
		EndHand();
	}
	else //call does not end hand following an ACTION_NONE (first to act in hand), or an ACTION_RAISE with < 3 actions having taken place (check still needed to end hand then)
	{
		mBetActionCount++;
		SwitchFocus(true, false);
		OutputPlayerPrompt(false, mCurrentBet, mCurrentBet - mPlayer->GetPotContribution());
	}
}

void PokerTable::AIRaise(ACTION previousAction)
{
	std::string output = "";
	bool isReRaise;
	if (previousAction == ACTION_RAISE)
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

	mCurrentBet *= 2;
	int amount = mCurrentBet - mAiPlayer->GetPotContribution();
	if ((mAiPlayer->GetStack() - amount) < 0)
	{
		//recalculate
		int newAmount = mAiPlayer->GetMaxBetBeforeNegativeStack(amount);
		mPot += newAmount;
		mAiPlayer->AmendStack(-(newAmount));
		mAiPlayer->AmendPotContribution(newAmount);
	}
	else
	{
		mPot += amount;
		mAiPlayer->AmendStack(-(amount));
		mAiPlayer->AmendPotContribution(amount);
	}

	
	SwitchFocus(true, false);
	
	OutputPlayerPrompt(isReRaise, mCurrentBet, mCurrentBet - mPlayer->GetPotContribution());
	
	mBetActionCount++;
}

void PokerTable::DetermineHandWinner()
{
	float playerStr = mEnumerator->GetHandStrength(mPlayer->GetHand(), mAiPlayer->GetHand());
	float horaceStr = mEnumerator->GetHandStrength(mAiPlayer->GetHand(), mPlayer->GetHand());

	if (playerStr > horaceStr)
	{
		//player wins hand
		cout << "You won the hand." << endl;
		mPlayer->AmendStack(mPot);
	}
	else if (horaceStr > playerStr)
	{
		//Ai wins hand
		mAiPlayer->AmendStack(mPot);
		cout << "Horace won the hand, with: " << endl;
		mDeckHelper->PrintCard(mAiPlayer->GetHand()->at(0));
		mDeckHelper->PrintCard(mAiPlayer->GetHand()->at(1));
	}
	else if (playerStr == horaceStr)
	{
		cout << "pot was split, tied hand" << endl;
		//split pot
		if (mPot % 2 == 0)
		{
			mPlayer->AmendStack(mPot / 2);
			mAiPlayer->AmendStack(mPot / 2);
		}
		else
		{
			//@TODO rework pots to be floats to allow true split plot
			float halfVal = (float)mPot / 2.0f;
			mPlayer->AmendStack((int)(halfVal + 0.5f));
			mAiPlayer->AmendStack((int)(halfVal + 0.5f));
		}
	}
}

void PokerTable::ResetGame()
{
	for each (BasePlayer* bp in mPlayers)
	{
		bp->ResetPlayer();
	}
	mAiPlayer->ResetAiPlayer();
	mPlayer->ResetPlayer();
	mHandCounter = 0;
	mTurnTimer = 0.0f;
	mPot = 0;
	mSmallBlind = 1;
	mHandInProgress = false;
	mLastActionTaken = ACTION_NONE;
	mGameIsOver = false;
	mDeckHelper->PopulateOrderedDeck();
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
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				if (!mGameIsOver)
				{
				case SDLK_SPACE: //spacebar to deal hand
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
						OutputPlayerPrompt(false, mCurrentBet, (mCurrentBet - mPlayer->GetPotContribution()));
					}
					else // Horace on button
					{
						cout << "Action is with Horace, press H key to progress AI." << endl;
					}

					SwitchFocus(mPlayer->GetIsOnButton(), mAiPlayer->GetIsOnButton()); //small blind starts betting each hand
					break;
				}
				else 				
				{
				case SDLK_r: //r key to reset
					ResetGame();
					mGameIsOver = false;
					break;
				}
			}
			break;
		}
	}
	else 
	{
		if (mPlayer->GetHasFocus())
		{
			HandlePlayerTurn(event);
		}
		else
		{
			HandleAITurn(event);
		}
	}
}

void PokerTable::SwitchFocus(bool human, bool horace)
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
	mBetActionCount = 0;
	mCurrentBet = mSmallBlind * 2;
	mLastActionTaken = ACTION_NONE;
	mHandInProgress = true;
	mPot = 0;
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
	//white
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface* textSurface = NULL;
	SDL_Texture* textTexture = NULL;

	//Pot
	RenderSomeText(textSurface, textTexture, std::string("Pot: " + to_string(mPot)), color, kScreenWidth / 2, 0, false, false);

	//blinds
	RenderSomeText(textSurface, textTexture, std::string("Blinds: " + to_string(mSmallBlind) + "/" + to_string(mSmallBlind * 2)).c_str(), color, 0, 0, false, false);

	//@TODO: should each player be responsible for rendering their own text?
	//human stack
	RenderSomeText(textSurface, textTexture, std::string("Chips: " + to_string(mPlayer->GetStack())).c_str(), color, 0, 0, true, false);

	//ai stack
	RenderSomeText(textSurface, textTexture, std::string("AI Chips: " + to_string(mAiPlayer->GetStack())).c_str(), color, 0, kScreenHeight / 2, false, true);

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

float PokerTable::GetRandomRealNumber(float min, float max)
{
	return uniform_real_distribution<float>{min, max}(mRng);
}



