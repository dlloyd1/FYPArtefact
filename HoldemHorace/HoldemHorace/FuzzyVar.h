#ifndef _FUZZYVAR_H
#define _FUZZYVAR_H

#include <map>
#include <string>
#include "FuzzySetProxy.h"
#include "BaseFuzzySet.h"
#include "RightShoulder.h"
#include "LeftShoulder.h"
#include "FuzzySingletonSet.h"

using namespace std;

class FuzzyVar
{
public:
	FuzzyVar();
	FuzzySetProxy AddRightShoulderSet(string name, double min, double peak, double max);
	FuzzySetProxy AddLeftShoulderSet(string name, double min, double peak, double max);
	FuzzySetProxy AddSingletonSet(string name, double min, double mid, double max);

	void Fuzzify(double val);

	double DeFuzzify() const;


private:
	map<string, BaseFuzzySet*> mMemberSets;

	//no copy or assignment ctor
	FuzzyVar(const FuzzyVar&);
	FuzzyVar operator=(const FuzzyVar&) {};

	double mMinRange;
	double mMaxRange;

	//called each time new set is added to adjust upper and lower ranges
	void AdjustRangeToFit(double min, double max);


	//prevents FuzzyVar reference gotten through CreateFuzzyVariable from being deleted
	~FuzzyVar();
	friend class FuzzyController;
};

#endif // !_FUZZYVAR_H

