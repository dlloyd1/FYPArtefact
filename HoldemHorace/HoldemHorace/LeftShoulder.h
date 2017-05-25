#ifndef _LEFTSHOULDER_H
#define _LEFTSHOULDER_H

#include "BaseFuzzySet.h"

class LeftShoulder : public BaseFuzzySet
{
public:
	LeftShoulder(double peak, double left, double right);
	~LeftShoulder();

	double CalculateDOM(double val)const;
private:
	double mPeakPoint;
	double mLeftOffset;
	double mRightOffset;
};

#endif // !_LEFTSHOULDER_H

