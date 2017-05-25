#include "FuzzyVar.h"



FuzzyVar::FuzzyVar()
{
	mMinRange = 0.0;
	mMaxRange = 0.0;
}

FuzzyVar::~FuzzyVar()
{
	map<string, BaseFuzzySet*>::iterator it;
	for (it = mMemberSets.begin(); it != mMemberSets.end(); ++it)
	{
		delete it->second;
	}
}

void FuzzyVar::AdjustRangeToFit(double min, double max)
{
	if (min < mMinRange)
	{
		mMinRange = min;
	}
	if (max > mMaxRange)
	{
		mMaxRange = max;
	}
}

void FuzzyVar::Fuzzify(double val)
{
	assert((val >= mMinRange) && (val <= mMaxRange) &&
		"<FuzzyVar::Fuzzify>: value out of range)");

	map<string, BaseFuzzySet*>::const_iterator set;
	for (set = mMemberSets.begin(); set != mMemberSets.end(); ++set)
	{
		set->second->SetDOM(set->second->CalculateDOM(val));
	}
}

double FuzzyVar::DeFuzzify() const
{
	double bottom = 0.0;
	double top = 0.0;

	map<string, BaseFuzzySet*>::const_iterator set;
	for (set = mMemberSets.begin(); set != mMemberSets.end(); ++set)
	{
		bottom += set->second->GetDOM();
		top += set->second->GetRepresentativeVal() * set->second->GetDOM();
	}
	//if (isEqual(0, bottom))
	if(bottom == 0.0)
	{
		return 0.0;
	}
	return top / bottom;
}

FuzzySetProxy FuzzyVar::AddRightShoulderSet(string name, double min, double peak, double max)
{
	mMemberSets[name] = new RightShoulder(peak, peak - min, max - peak);
	AdjustRangeToFit(min, max);

	return FuzzySetProxy(*mMemberSets[name]);
}

FuzzySetProxy FuzzyVar::AddLeftShoulderSet(string name, double min, double peak, double max)
{

	mMemberSets[name] = new LeftShoulder(peak, peak - min, max - peak);
	AdjustRangeToFit(min, max);
	return FuzzySetProxy(*mMemberSets[name]);
}

FuzzySetProxy FuzzyVar::AddSingletonSet(string name, double min, double mid, double max)
{
	mMemberSets[name] = new FuzzySingletonSet(mid, mid - min, max - mid);
	AdjustRangeToFit(min, max);
	return FuzzySetProxy(*mMemberSets[name]);
}