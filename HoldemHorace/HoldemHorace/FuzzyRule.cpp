#include "FuzzyRule.h"



FuzzyRule::FuzzyRule(FuzzyTerm& condition, FuzzyTerm& result)
{
	mCondition = condition.Clone();
	mResult = result.Clone();
}


FuzzyRule::~FuzzyRule()
{
	delete mCondition;
	delete mResult;
}

void FuzzyRule::Calculate()
{
	mResult->ORwithDOM(mCondition->GetDOM());
}
