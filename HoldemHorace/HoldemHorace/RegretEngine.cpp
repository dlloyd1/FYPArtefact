#include "RegretEngine.h"


RegretEngine::RegretEngine()
{
	//arbitrary
	mHandoutcome = OUTCOME_WIN;

	SetupProbabilityTriple();
	SetupPossibleActions();
	SetupPossibleOutcomes();
}

RegretEngine::~RegretEngine()
{
	mPossibleActionsToRegret.clear();
	mProbabilityTriple.clear();
	mCumulativeRegrets.clear();
	mPossibleHandOutcomes.clear();
}

//no AI memory
void RegretEngine::ZeroInstantiateCumulativeRegrets()
{
	mCumulativeRegrets = std::map<ACTION, int>();
	for (auto a : mPossibleActionsToRegret)
	{
		mCumulativeRegrets[a] = 0;
	}
}

//load in AI memory
void RegretEngine::LoadCumulativeRegretsFromFile()
{
	ifstream regretsFile("cumulativeRegrets.txt");
	int regret;
	regretsFile >> regret;
	mCumulativeRegrets[ACTION_FOLD] = regret;
	regretsFile >> regret;
	mCumulativeRegrets[ACTION_CALL] = regret;
	regretsFile >> regret;
	mCumulativeRegrets[ACTION_RAISE] = regret;

	regretsFile.close();
}

void RegretEngine::SaveCumulativeRegretsToFile()
{
	if (!mCumulativeRegrets.empty())
	{
		ofstream regretsFile;
		regretsFile.open("cumulativeRegrets.txt");
		if (regretsFile.is_open())
		{
			regretsFile << to_string(mCumulativeRegrets[ACTION_FOLD]) << endl;
			regretsFile << to_string(mCumulativeRegrets[ACTION_CALL]) << endl;
			regretsFile << to_string(mCumulativeRegrets[ACTION_RAISE]) << endl;
			regretsFile.close();
		}
		else
		{
			cout << "ERROR: unable to open cumulativeRegrets.txt for saving." << endl;
		}
	}
}

void RegretEngine::SetCumulativeRegrets(std::map<ACTION, int> actionRegretMap)
{
	mCumulativeRegrets = std::map<ACTION, int>();
	for (auto a : actionRegretMap)
	{
		mCumulativeRegrets[a.first] = a.second;
	}
}

void RegretEngine::SetupProbabilityTriple()
{
	mProbabilityTriple = std::map<ACTION, double>();
	mProbabilityTriple[ACTION_FOLD] = 0.33;
	mProbabilityTriple[ACTION_CALL] = 0.33;
	mProbabilityTriple[ACTION_RAISE] = 0.33;
}

void RegretEngine::SetupPossibleOutcomes()
{
	mPossibleHandOutcomes = std::vector<HANDOUTCOME>();
	mPossibleHandOutcomes.push_back(OUTCOME_WIN);
	mPossibleHandOutcomes.push_back(OUTCOME_LOSE);
	mPossibleHandOutcomes.push_back(OUTCOME_TIE);
}

void RegretEngine::SetupPossibleActions()
{
	mPossibleActionsToRegret = std::vector<ACTION>();
	mPossibleActionsToRegret.push_back(ACTION_FOLD);
	mPossibleActionsToRegret.push_back(ACTION_CALL);
	mPossibleActionsToRegret.push_back(ACTION_RAISE);
}

void RegretEngine::ResetEngine()
{
	mCumulativeRegrets.clear();
	mProbabilityTriple.clear();
}

void RegretEngine::UpdateRegrets(const ACTION actionChosen, const multimap<HANDOUTCOME, ACTION> actionOutcomeRegretMap)
{
	mCumulativeRegrets[actionChosen] += 0;

	for (const auto& pair : actionOutcomeRegretMap)
	{
		int uActionToRegret = UtilityHelper::Instance()->GetUtilityOfAction(pair.first, pair.second);
		int uActionTaken = UtilityHelper::Instance()->GetUtilityOfAction(mHandoutcome, actionChosen);
		
		mCumulativeRegrets[std::get<1>(pair)] += CalculateRegret(uActionToRegret, uActionTaken);
	}
	UpdateProbabilityTriple();
	SaveCumulativeRegretsToFile();
}

int RegretEngine::CalculateRegret(const int utilityOfActionToRegret, const int utilityOfActionTaken) const
{
	assert(utilityOfActionToRegret >= -1 && utilityOfActionToRegret <= 1
		&& utilityOfActionTaken >= -1 && utilityOfActionTaken <= 1);

	return utilityOfActionToRegret - utilityOfActionTaken;
}

void RegretEngine::UpdateProbabilityTriple()
{
	double regretSum = 0.0;
	for (const auto& pair : mCumulativeRegrets)
	{
		if(pair.second > 0)
		{
			regretSum += pair.second;
		}
	}

	if (regretSum > 1.0) 
	{
		for (auto a : GetPossibleActions())
		{
			if (mCumulativeRegrets[a] > 0)
			{
				mProbabilityTriple[a] = static_cast<double>(mCumulativeRegrets[a]) / regretSum;
			}
			else
			{
				mProbabilityTriple[a] = 0.0;
			}
		}
	}
}

multimap<HANDOUTCOME, ACTION> RegretEngine::ConstructActionOutcomeRegretMap(const HANDOUTCOME outcome, const ACTION action)
{
	//Get map of all actions that is not the action chosen against the HANDoutcome this hand.
	//pass this to RegretEngine::UpdateRegrets() to modify the mCumulativeRegrets and mProbabilityTriple member variables respectively.
	multimap<HANDOUTCOME, ACTION> regretMap;
	for (auto a : mPossibleActionsToRegret)
	{
		if (a != action)
		{
			regretMap.insert(pair<HANDOUTCOME, ACTION>(outcome, a));
		}
	}
	return regretMap;
}

void RegretEngine::ModifyProbTripleByPlaystyle(const PLAYSTYLE playstyle)
{
	if (playstyle != PLAYSTYLE_NONE)
	{
		switch (playstyle)
		{
		case PLAYSTYLE_LOOSEAGGRESSIVE:
			mProbabilityTriple[ACTION_CALL] += kPlaystyleModifier;
			mProbabilityTriple[ACTION_RAISE] -= kPlaystyleModifier;
			break;
		case PLAYSTYLE_TIGHTAGGRESSIVE:
			mProbabilityTriple[ACTION_CALL] -= kPlaystyleModifier;
			mProbabilityTriple[ACTION_RAISE] += kPlaystyleModifier;
			break;
		case PLAYSTYLE_LOOSEPASSIVE:
			mProbabilityTriple[ACTION_CALL] += kPlaystyleModifier;
			mProbabilityTriple[ACTION_FOLD] -= kPlaystyleModifier;
			break;
		case PLAYSTYLE_TIGHTPASSIVE:
			mProbabilityTriple[ACTION_CALL] -= kPlaystyleModifier;
			mProbabilityTriple[ACTION_RAISE] += kPlaystyleModifier;
			break;
		default:
			cout << "ERROR: PLAYSTYLE SET INCORRECTLY FOR PROB% TRIPLE MODIFICATION" << endl;
		}
	}
}
