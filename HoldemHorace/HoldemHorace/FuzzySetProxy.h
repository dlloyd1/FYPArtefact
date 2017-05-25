#ifndef _FUZZYSETPROXY_H
#define _FUZZYSETPROXY_H

#include "BaseFuzzySet.h"
#include "FuzzyTerm.h"

class FuzzySetProxy : public FuzzyTerm
{
public:
	FuzzySetProxy(BaseFuzzySet& fs) : mSet(fs) {}
	FuzzyTerm* Clone() const { return new FuzzySetProxy(*this); }
	double GetDOM() const { return mSet.GetDOM(); }
	void ClearDOM() { mSet.ClearDOM(); }
	void ORwithDOM(double val) { mSet.ORwithDOM(val); }
	                                                                                   

private:
	BaseFuzzySet& mSet;

};

#endif // !_FUZZYSETPROXY_H
