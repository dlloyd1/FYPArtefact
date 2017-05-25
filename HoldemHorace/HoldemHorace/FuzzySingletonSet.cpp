#include "FuzzySingletonSet.h"



FuzzySingletonSet::FuzzySingletonSet(double mid, double left, double right) : BaseFuzzySet(mid)
{
	mMidPoint = mid;
	mLeftOffset = left;
	mRightOffset = right;
}

double FuzzySingletonSet::CalculateDOM(double val) const
{
	if ((val >= mMidPoint - mLeftOffset) &&
		(val <= mMidPoint + mRightOffset))
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}
