#include "FuzzyController.h"



FuzzyController::FuzzyController()
{
	mFuzzyVarMap = map<string, FuzzyVar*>();
	mFuzzyRules = vector<FuzzyRule*>();
}

FuzzyController::~FuzzyController()
{
	map<string, FuzzyVar*>::iterator var = mFuzzyVarMap.begin();
	for (var; var != mFuzzyVarMap.end(); ++var)
	{
		delete var->second;
	}

	vector<FuzzyRule*>::iterator rule = mFuzzyRules.begin();
	for (rule; rule != mFuzzyRules.end(); ++rule)
	{
		delete *rule;
	}
}

void FuzzyController::Fuzzify(const string& fuzzyVarName, double val)
{
	//check it exists
	assert((mFuzzyVarMap.find(fuzzyVarName) != mFuzzyVarMap.end()) &&
		"<FuzzyController::Fuzzify>:key not found");

	mFuzzyVarMap[fuzzyVarName]->Fuzzify(val);
}

double FuzzyController::DeFuzzify(const string& fuzzyVarName)
{
	assert((mFuzzyVarMap.find(fuzzyVarName) != mFuzzyVarMap.end()) &&
		"<FuzzyController::DeFuzzify>:key not found");

	SetConfidenceOfResultsToZero();

	vector<FuzzyRule*>::iterator rule = mFuzzyRules.begin();
	for (rule; rule != mFuzzyRules.end(); ++rule)
	{
		(*rule)->Calculate();
	}

	return mFuzzyVarMap[fuzzyVarName]->DeFuzzify();
}

FuzzyVar& FuzzyController::CreateFuzzyVariable(const string& name)
{
	mFuzzyVarMap[name] = new FuzzyVar();

	return *mFuzzyVarMap[name];
}

void FuzzyController::CreateFuzzyRule(FuzzyTerm& condition, FuzzyTerm& result)
{
	mFuzzyRules.push_back(new FuzzyRule(condition, result));
}

void FuzzyController::SetConfidenceOfResultsToZero()
{
	vector<FuzzyRule*>::iterator rule = mFuzzyRules.begin();
	for (rule; rule != mFuzzyRules.end(); ++rule)
	{
		(*rule)->SetConfidenceOfResultToZero();
	}
}
