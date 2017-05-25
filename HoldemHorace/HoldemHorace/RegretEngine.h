#ifndef _REGRETENGINE_H
#define _REGRETENGINE_H

#include "Structures.h"
#include "UtilityHelper.h"
#include <iostream>
#include <fstream>

class RegretEngine
{
public:
	~RegretEngine();
	static RegretEngine* Instance()
	{
		static RegretEngine instance;
		return &instance;
	}

	void UpdateRegrets(const ACTION actionChosen, const multimap<HANDOUTCOME, ACTION> actionOutcomeRegretMap);

	int CalculateRegret(const int utilityOfActionToRegret, const int utilityOfActionTaken) const;
	void SetupPossibleActions();
	void SetupPossibleOutcomes();

	vector<ACTION> GetPossibleActions() const { return mPossibleActionsToRegret; }
	vector<HANDOUTCOME> GetPossibleOutcomes() const { return mPossibleHandOutcomes; }

	void SetHandoutcome(const HANDOUTCOME val) { mHandoutcome = val; }
	HANDOUTCOME GetHandoutcome() const { return mHandoutcome; }

	map<ACTION, double> GetProbabilityTriple() const { return mProbabilityTriple; }
	map<ACTION, int> GetCumulativeRegrets() const { return mCumulativeRegrets; }

	void ResetEngine();

	//multimap Allow multiple duplicate entries of a given key mapped to different vals
	multimap<HANDOUTCOME, ACTION> ConstructActionOutcomeRegretMap(const HANDOUTCOME outcome, const ACTION action);
	void ModifyProbTripleByPlaystyle(const PLAYSTYLE playstyle);

	void ZeroInstantiateCumulativeRegrets();
	void SaveCumulativeRegretsToFile();
	void LoadCumulativeRegretsFromFile();

private:
	RegretEngine();
	RegretEngine(const RegretEngine&) {};
	RegretEngine& operator=(const RegretEngine&) {};//no cpy or assign;

	void SetupProbabilityTriple();
	void UpdateProbabilityTriple();
	void SetCumulativeRegrets(std::map<ACTION, int> actionRegretMap);


	map<ACTION, int> mCumulativeRegrets;
	map<ACTION, double> mProbabilityTriple;

	vector<ACTION> mPossibleActionsToRegret;
	vector<HANDOUTCOME> mPossibleHandOutcomes;

	//from the AI perspective
	HANDOUTCOME mHandoutcome;
};
#endif // !_REGRETENGINE_H
