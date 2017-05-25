#ifndef _BASEFUZZYSET_H
#define _BASEFUZZYSET_H

#include <cassert>

class BaseFuzzySet
{
public:
	BaseFuzzySet(double val);
	~BaseFuzzySet();

	virtual double CalculateDOM(double val) const = 0;

	//for deciding which in a Fuzzy Associative Matrix
	void ORwithDOM(double val);

	double GetRepresentativeVal()const { return mRepresentativeVal; }
	void ClearDOM() { mDOM = 0.0; }
	double GetDOM()const { return mDOM;}
	void SetDOM(double val);

protected:
	double mDOM;
	double mRepresentativeVal;
private:

};

#endif // !_BASEFUZZYSET_H

