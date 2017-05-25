#ifndef _FUZZYTERMAND_H
#define _FUZZYTERMAND_H

#include "FuzzyTerm.h"
#include <vector>
//#include "Constants.h"

using namespace std;

class FuzzyTermAND : public FuzzyTerm
{
public:
	FuzzyTermAND(FuzzyTerm& operator1, FuzzyTerm& operator2);
	FuzzyTermAND(const FuzzyTermAND& fa);

	FuzzyTerm* Clone() const { return new FuzzyTermAND(*this); }

	double GetDOM() const;
	void ClearDOM();
	void ORwithDOM(double val);

	~FuzzyTermAND();

private:
	vector<FuzzyTerm*> mTerms;

	//no instantiation by assignment
	FuzzyTermAND& operator=(const FuzzyTermAND&);
};
#endif // !_FUZZYTERMAND_H
