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

HoleHand Enumerator::ConvertHandToHoleHand(const vector<Card>* theHand) const
{
	//make a copy, to preserve the integrity of the actual hand card order
	vector<Card> copyOfHand;
	copyOfHand.push_back(theHand->at(0));
	copyOfHand.push_back(theHand->at(1));


	//Format the hand correctly, larger rank on the left, to determine which Distinct Hand we have
	//only applicable to non pair hands
	if (copyOfHand[0].rank != copyOfHand[1].rank)
	{
		if (copyOfHand[0].rank < copyOfHand[1].rank)
		{
			iter_swap(copyOfHand.begin(), copyOfHand.begin() + 1);
		}
	}

	//go through each mDistinctHand and find a match
	for(int i = 0; i < mDistinctHands->size(); ++i)
	{
		if (mDistinctHands->at(i).card1 == copyOfHand[0].rank &&
			mDistinctHands->at(i).card2 == copyOfHand[1].rank)
		{
			return mDistinctHands->at(i);
		}
	}
}

RANK Enumerator::RankLookUp(const char rank) const
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
vector<string> Enumerator::ReadInCSV() const
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
void Enumerator::PopulateDistinctHands(const vector<string> inputFileAsStrings)
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

//returns float percentage of hands our hand is stronger than
float Enumerator::GetWinPercentageOfHand(const vector<Card>* ourHand) const
{
	HoleHand theHand = ConvertHandToHoleHand(ourHand);

	//copy to new vector
	vector<HoleHand> hands = *mDistinctHands;

	float total = 0.0f;
	for each (HoleHand hand in hands)
	{
		if (hand.rank > theHand.rank)
		{
			total++;
		}
		else if (hand.rank == theHand.rank)
		{
			total += 0.5f;
		}
	}
	return (float)(total / mDistinctHands->size());
}
