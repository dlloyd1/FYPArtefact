#ifndef _ENUMERATOR_H
#define _ENUMERATOR_H

#include "Structures.h"
#include <fstream>
#include <stdlib.h> //size_t / atof()?


class Enumerator
{
public:
	~Enumerator(); //compiler safe, stricly should be pvt dtor()
	static Enumerator* Instance()
	{
		static Enumerator instance;
		return &instance;
	}

	float GetWinPercentageOfHand(const vector<Card>* ourHand) const;

private:
	Enumerator();

	Enumerator(const Enumerator&); 
	Enumerator& operator=(const Enumerator&) {};

	HoleHand ConvertHandToHoleHand(const vector<Card>* theHand) const;
	RANK RankLookUp(const char rank) const;
	vector<string> ReadInCSV() const;
	void PopulateDistinctHands(const vector<string> inputFileAsStrings);

	vector<HoleHand>* mDistinctHands;
};

#endif // !_ENUMERATOR_H
