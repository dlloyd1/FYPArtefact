#ifndef _BASEPLAYER_H
#define _BASEPLAYER_H

#include "Structures.h"
#include "Constants.h"

#include "Texture2D.h"
#include "SDL_ttf.h"

class BasePlayer
{
public:
	BasePlayer(SDL_Renderer* renderer, Texture2D* cardSpritesheet, Texture2D* cardOutlineTexture);
	virtual ~BasePlayer(); //virtual destructor - prevent undefined when deleting base class pointer

	virtual void Render() const = 0;
	virtual void ResetPlayer() = 0;
	
	int GetStack() const { return mStack; }
	bool GetIsOnButton() const { return mIsOnButton; }
	bool GetHasFocus() const { return mHasFocus; }
	int GetPotContribution() const { return mPotContributionThisHand; }
	vector<Card>* GetHand() const { return mHand; }

	void SetIsOnButton(const bool val) { mIsOnButton = val; }
	void SetHasFocus(const bool val) { mHasFocus = val; }
	void AmendStack(const int val) { mStack += val; }
	void AmendPotContribution(const int val) { mPotContributionThisHand += val; }
	void ResetPotContribution() { mPotContributionThisHand = 0; }
	


	int GetMaxBetBeforeNegativeStack(const int betAmount) const;
	void ReceiveCardFromTable(const Card card);
	void ClearHand();
	int PostBlind(const int smallBlind);

private:
protected:
	vector<Card>* mHand;
	Texture2D* mCardsSpritesheet;
	Texture2D* mCardOutlineTexture;
	bool mIsOnButton;
	bool mHasFocus;
	int mStack;
	int mPotContributionThisHand;
};

#endif // !_BASEPLAYER_H
