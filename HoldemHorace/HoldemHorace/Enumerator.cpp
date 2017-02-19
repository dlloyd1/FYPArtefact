#include "Enumerator.h"

Enumerator::Enumerator()
{
	//all 169 distinct preflop hands
	mDistinctHands = new vector<HoleHand>();
	vector<string> csvLines = ReadInCSV();
	PopulateDistinctHands(csvLines);
}

Enumerator::~Enumerator()
{
	delete mDistinctHands;
}

HoleHand Enumerator::ConvertHandToHoleHand(vector<Card>* theHand)
{
	//go through each mDistinctHand
	//find the HoleHand where card1 == theHand[0].rank && card2 == theHand[0].rank
	for(int i = 0; i < mDistinctHands->size(); ++i)
	{
		if (mDistinctHands->at(i).card1 == theHand->at(0).rank &&
			mDistinctHands->at(i).card2 == theHand->at(1).rank)
		{
			return mDistinctHands->at(i);
		}
	}
}

RANK Enumerator::RankLookUp(char rank)
{
	switch (rank)
	{
	case 'A':
		return RANK_ACE;
		break;
	case 'K':
		return RANK_KING;
		break;
	case 'Q':
		return RANK_QUEEN;
		break;
	case 'J':
		return RANK_JACK;
		break;
	case 'T':
		return RANK_TEN;
		break;
	case '9':
		return RANK_NINE;
		break;
	case '8':
		return RANK_EIGHT;
		break;
	case '7':
		return RANK_SEVEN;
		break;
	case '6':
		return RANK_SIX;
		break;
	case '5':
		return RANK_FIVE;
		break;
	case '4':
		return RANK_FOUR;
		break;
	case '3':
		return RANK_THREE;
		break;
	case '2':
		return RANK_TWO;
		break;

	}
}

//each index of the container is a line from the csv
vector<string> Enumerator::ReadInCSV()
{
	//format 1,AA
	ifstream inputFile;
	inputFile.open("HandRankingsTrim.csv");
	if (!inputFile)
	{
		cerr << "Error: Hand Rankings file could not be opened" << endl;
		exit(EXIT_FAILURE);
	}

	vector<string> fileAsLineStrings;
	while (inputFile)
	{
		string tmpString;
		if (!getline(inputFile, tmpString))
		{
			break;
		}
		fileAsLineStrings.push_back(tmpString);
	}
	return fileAsLineStrings;
}

//populate table of distinct preflop hands by rank, given output from the csv read
void Enumerator::PopulateDistinctHands(vector<string> inputFileAsStrings)
{
	for each (string s in inputFileAsStrings)
	{
		bool paired = false;
		bool suited = false;

		int commaPos = s.find(',');
		std::string rank = s.substr(0, commaPos);
		int rankAsInt = atoi(rank.c_str());
		std::string strCardInfo = s.substr(commaPos + 1); //everything after the comma
		char card1 = strCardInfo[0];
		char card2 = strCardInfo[1];

		if (strCardInfo.size() == 2)
		{
			paired = true;
			suited = false;
		}
		else
		{
			if (strCardInfo[strCardInfo.size() - 1] == 's')
			{
				suited = true;
			}
			else
			{
				suited = false;
			}
		}
		mDistinctHands->push_back(HoleHand(RankLookUp(card1), RankLookUp(card2), rankAsInt, paired, suited));	
	}
}

void Enumerator::RemoveFromHoleHandVector(vector<HoleHand> &vectorToModify, HoleHand handToRemove)
{

	int indexToDel = 0;
	for (int i = 0; i < vectorToModify.size(); ++i)
	{
		if (vectorToModify.at(i).rank == handToRemove.rank &&
			vectorToModify.at(i).card1 == handToRemove.card1 &&
			vectorToModify.at(i).card2 == handToRemove.card2)
		{
			indexToDel = i;
			break;
		}
	}
	vectorToModify.erase(vectorToModify.begin() + indexToDel);
}

//returns float percentage of hands that are stronger than 'ourHand'
float Enumerator::GetRankOfHand(HoleHand ourHand)
{
	//copy to new vector
	vector<HoleHand> eachOtherHand = *mDistinctHands;
	RemoveFromHoleHandVector(eachOtherHand, ourHand);
	//168 remaining distinct hands that is not the hand we are ranking
	//eachOtherHand.erase(remove(eachOtherHand.begin(), eachOtherHand.end(), ourHand), eachOtherHand.end());

	float total = 0.0f;
	for each (HoleHand hand in eachOtherHand)
	{
		if (hand.rank > ourHand.rank)
		{
			total++;
		}
		else if (hand.rank == ourHand.rank)
		{
			total += 0.5f;
		}
	}
	//percentage of hands that defeat our hand, as decimal
	return total / 168.0f;
}

//@TODO: update to add weighted enumerations instead of flat 1/0.5f 
float Enumerator::GetHandStrength(vector<Card>* ourHand, vector<Card>* theirHand)
{
	HoleHand ourHoleHand = ConvertHandToHoleHand(ourHand);
	HoleHand theirHoldHand = ConvertHandToHoleHand(theirHand);

	vector<HoleHand> possibleOppDistinctHands = *mDistinctHands;
	RemoveFromHoleHandVector(possibleOppDistinctHands, theirHoldHand);
//possibleOppDistinctHands.erase(remove(possibleOppDistinctHands.begin(), possibleOppDistinctHands.end(), theirHand), possibleOppDistinctHands.end());

	float ourRank = GetRankOfHand(ourHoleHand);
	float ahead, tied, behind, handStrength;
	ahead = tied = behind = handStrength = 0;

	for each(HoleHand Hh in possibleOppDistinctHands)
	{
		float oppRank = GetRankOfHand(Hh);
		if (ourRank > oppRank)
		{
			ahead++;
		}
		else if (ourRank == oppRank)
		{
			tied++;
		}
		else
		{
			behind++;
		}
	}

	handStrength = (ahead + tied / 2) / (ahead + tied + behind);
	return handStrength;
}
