#include "LeftShoulder.h"



LeftShoulder::LeftShoulder(double peak, double left, double right) : BaseFuzzySet(((peak - left) + peak) / 2)
{
	mPeakPoint = peak;
	mLeftOffset = left;
	mRightOffset = right;
}

LeftShoulder::~LeftShoulder()
{
}

double LeftShoulder::CalculateDOM(double val) const
{
	//test for either left or right offset at 0, prevents divide by zero errors
	if (
		//(isEqual(mRightOffset, 0.0) && isEqual(mPeakPoint, val))
		(mRightOffset == 0.0) && (mPeakPoint == val)
		||
		/*(isEqual(mLeftOffset, 0.0) && isEqual(mPeakPoint, val))
		*/(mLeftOffset == 0.0) && (mPeakPoint == val)
		)
	{
		return 1.0;
	}
	else if ((val >= mPeakPoint) && (val < (mPeakPoint + mRightOffset)))
	{
		double gradient = 1.0 / -mRightOffset;
		return gradient * (val - mPeakPoint) + 1.0;
	}
	else if ((val < mPeakPoint) && (val >= mPeakPoint - mLeftOffset))
	{
		return 1.0;
	}
	else
	{
		return 0;
	}
}
