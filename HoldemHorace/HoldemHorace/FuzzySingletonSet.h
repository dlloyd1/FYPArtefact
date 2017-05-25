#ifndef _FUZZYSINGLETONSET_H
#define _FUZZYSINGLETONSET_H

#include "BaseFuzzySet.h"




class FuzzySingletonSet : public BaseFuzzySet
{
public:
	FuzzySingletonSet(double mid, double left, double right);
	~FuzzySingletonSet();

	double CalculateDOM(double val) const;

private:
	double mMidPoint;
	double mLeftOffset;
	double mRightOffset;
};

#endif // !_FUZZYSINGLETONSET_H
