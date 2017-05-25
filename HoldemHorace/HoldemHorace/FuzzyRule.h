#ifndef _FUZZYRULE_H
#define _FUZZYRULE_H

#include "FuzzyTermAND.h"

class FuzzyRule
{
public:
	//condition = antecedent
	//result = consequence
	FuzzyRule(FuzzyTerm& condition, FuzzyTerm& result);
	~FuzzyRule(/*delete the member vars in here*/);

	void SetConfidenceOfResultToZero() { mResult->ClearDOM(); }

	//updates the dom of the result with the dom of the condition
	void Calculate();
private:

	//condition (can be a composite of several fuzzy sets and operators)
	const FuzzyTerm* mCondition;
	//result (can be a single fuzzy set, or several ANDed together)
	FuzzyTerm* mResult;

	//prevent copying of rules with assign and copy ctors
	FuzzyRule(const FuzzyRule&);
	FuzzyRule& operator =(const FuzzyRule&) {}
};

#endif // !_FUZZYRULE_H
