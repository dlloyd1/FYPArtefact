#ifndef _RIGHTSHOULDER_H
#define _RIGHTSHOULDER_H

#include "BaseFuzzySet.h"


class RightShoulder : public BaseFuzzySet
{
public:
	RightShoulder(double peak, double left, double right);
	~RightShoulder();

	double CalculateDOM(double val)const;
private:
	double mPeakPoint;
	double mLeftOffset;
	double mRightOffset;
};

#endif // !_RIGHTSHOULDER_H

