#include "FuzzyTermAND.h"


FuzzyTermAND::FuzzyTermAND(FuzzyTerm& operator1, FuzzyTerm& operator2)
{
	mTerms.push_back(operator1.Clone());
	mTerms.push_back(operator2.Clone());
}

FuzzyTermAND::FuzzyTermAND(const FuzzyTermAND& fa)
{
	vector<FuzzyTerm*>::const_iterator term;
	for (term = fa.mTerms.begin(); term != fa.mTerms.end(); ++term)
	{
		mTerms.push_back((*term)->Clone());
	}
}

//AND operator returns minimum DOM of the sets it's operating on
double FuzzyTermAND::GetDOM() const
{
	double smallest = (std::numeric_limits<double>::max)();

	vector<FuzzyTerm*>::const_iterator term;
	for (term = mTerms.begin(); term != mTerms.end(); ++term)
	{
		if ((*term)->GetDOM() < smallest)
		{
			smallest = (*term)->GetDOM();
		}
	}
	return smallest;
}

void FuzzyTermAND::ClearDOM()
{
	vector<FuzzyTerm*>::iterator term;
	for (term = mTerms.begin(); term != mTerms.end(); ++term)
	{
		(*term)->ClearDOM();
	}
}

void FuzzyTermAND::ORwithDOM(double val)
{
	vector<FuzzyTerm*>::iterator term;
	for (term = mTerms.begin(); term != mTerms.end(); ++term)
	{
		(*term)->ORwithDOM(val);
	}
}

FuzzyTermAND::~FuzzyTermAND()
{
	vector<FuzzyTerm*>::iterator term;
	for (term = mTerms.begin(); term != mTerms.end(); ++term)
	{
		delete *term;
	}
}
