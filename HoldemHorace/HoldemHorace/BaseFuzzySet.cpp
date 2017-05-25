#include "BaseFuzzySet.h"



BaseFuzzySet::BaseFuzzySet(double val)
{
	mRepresentativeVal = val;
	mDOM = 0.0;
}

BaseFuzzySet::~BaseFuzzySet()
{
}

void BaseFuzzySet::ORwithDOM(double val)
{
	if (val > mDOM)
	{
		mDOM = val;
	}
}

void BaseFuzzySet::SetDOM(double val)
{
	assert((val <= 1) && (val >= 0) && "<BaseFuzzySet>::SetDOM: invalid value");
	mDOM = val;
}
