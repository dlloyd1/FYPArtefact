#ifndef _FUZZYLOGICFACADE_H
#define _FUZZYLOGICFACADE_H

#include "FuzzyController.h"
#include "OpponentModeller.h"
#include "Structures.h"

class FuzzyLogicFacade
{
public:
	FuzzyLogicFacade();
	~FuzzyLogicFacade();

	void SetupFuzzyLogicEngine() const;
	map<string, double> ConstructDefuzzResultsMap() const;
	void CategoriseHumanPlaystyle();
	string ConvertPlaystyleToString() const;

	//playstyle accessors
	PLAYSTYLE GetHumanPlaystyle() const { return mHumanPlaystyle; }
	void SetHumanPlaystyle(const PLAYSTYLE val) { mHumanPlaystyle = val; }

private:
	PLAYSTYLE mHumanPlaystyle;
};


#endif // !_FUZZYLOGICFACADE_H


