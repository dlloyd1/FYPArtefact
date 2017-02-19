#ifndef _ENUMERATOR_H
#define _ENUMERATOR_H

#include "Structures.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>


class Enumerator
{
public:
	~Enumerator(); //compiler safe, stricly should be pvt dtor()
	static Enumerator* Instance()
	{
		static Enumerator instance;
		return &instance;
	}

	float GetRankOfHand(HoleHand ourHand);
	float GetHandStrength(vector<Card>* ourHand, vector<Card>* theirHand);

private:
	Enumerator();

	Enumerator(const Enumerator&); 
	Enumerator& operator=(const Enumerator &); 

	vector<HoleHand>* mDistinctHands;
	HoleHand ConvertHandToHoleHand(vector<Card>* theHand);
	RANK RankLookUp(char rank);
	vector<string> ReadInCSV();
	void PopulateDistinctHands(vector<string> inputFileAsStrings);
	void RemoveFromHoleHandVector(vector<HoleHand> &vectorToModify, HoleHand handToRemove);
};

#endif // !_ENUMERATOR_H
