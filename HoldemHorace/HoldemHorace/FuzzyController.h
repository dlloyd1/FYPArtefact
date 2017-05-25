
#ifndef _FUZZYCONTROLLER_H
#define _FUZZYCONTROLLER_H

#include <string>
#include <map>
#include <vector>
#include "FuzzyVar.h"
#include "FuzzyRule.h"
#include <cassert>

using namespace std;

class FuzzyController
{
public:
	~FuzzyController();
	static FuzzyController* Instance()
	{
		static FuzzyController instance;
		return &instance;
	}

	void Fuzzify(const string& fuzzyVarName, double val);

	double DeFuzzify(const string& fuzzyVarName);

	FuzzyVar& CreateFuzzyVariable(const string& name);

	void CreateFuzzyRule(FuzzyTerm& condition, FuzzyTerm& result);

private:
	FuzzyController();
	FuzzyController(const FuzzyController&);
	FuzzyController& operator=(const FuzzyController &) {}; //disable assignment and copy ctors().

	map<string, FuzzyVar*> mFuzzyVarMap;
	vector<FuzzyRule*> mFuzzyRules;

	void SetConfidenceOfResultsToZero();
};

#endif // !_FUZZYCONTROLLER_H
