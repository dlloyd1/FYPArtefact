#include "RightShoulder.h"



RightShoulder::RightShoulder(double peak, double left, double right) : BaseFuzzySet(((peak + right) + peak) / 2)
{
	mPeakPoint = peak;
	mLeftOffset = left;
	mRightOffset = right;
}

RightShoulder::~RightShoulder()
{
}

double RightShoulder::CalculateDOM(double val) const
{
	//test for either left or right offset at 0, prevents divide by zero errors
	if ( 
		/*(isEqual(mRightOffset, 0.0) && isEqual(mPeakPoint, val) )*/
		(mRightOffset == 0.0) && (mPeakPoint == val)
		||
		/*(isEqual(mLeftOffset, 0.0) && isEqual(mPeakPoint, val))*/
		(mLeftOffset == 0.0) && (mPeakPoint == val)
		)
	{
		return 1.0;
	}
	else if ((val <= mPeakPoint) && (val > (mPeakPoint - mLeftOffset)))
	{
		double gradient = 1.0 / mLeftOffset;
		return gradient * (val - (mPeakPoint - mLeftOffset));
	}
	else if ((val > mPeakPoint) && (val <= mPeakPoint + mRightOffset))
	{
		return 1.0;
	}
	else
	{
		return 0;
	}
}
