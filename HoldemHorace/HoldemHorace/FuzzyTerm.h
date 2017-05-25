#ifndef _FUZZYTERM_H
#define _FUZZYTERM_H

//common interface for both fuzzy rule operands and operators
class FuzzyTerm
{
public:
	virtual FuzzyTerm* Clone() const = 0; //forced derived terms implementation of ctor

	virtual double GetDOM() const = 0;

	virtual void ClearDOM() = 0;

	//update the DOM of a result from a fuzzy rule
	virtual void ORwithDOM(double val) = 0;

	virtual ~FuzzyTerm() {}
};

#endif // !_FUZZYTERM_H
