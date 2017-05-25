#include "FuzzyLogicFacade.h"



FuzzyLogicFacade::FuzzyLogicFacade()
{
	mHumanPlaystyle = PLAYSTYLE_NONE;
}


FuzzyLogicFacade::~FuzzyLogicFacade()
{
}

void FuzzyLogicFacade::SetupFuzzyLogicEngine() const
{
	//The Two Fuzzy Graphs
	FuzzyVar& tightness = FuzzyController::Instance()->CreateFuzzyVariable("tightness");
	//The actual sets for the tightness and aggressiveness fuzzy graphs
	FuzzySetProxy& tightnessTight = tightness.AddRightShoulderSet("tightnessTight", 0.0, 0.9, 1.0);
	FuzzySetProxy& tightnessLoose = tightness.AddLeftShoulderSet("tightnessLoose", 0.0, 0.1, 1.0);

	FuzzyVar& aggressiveness = FuzzyController::Instance()->CreateFuzzyVariable("aggressiveness");
	FuzzySetProxy& aggressivenessAggressive = aggressiveness.AddRightShoulderSet("aggressivenessAggressive", 0.0, 1.5, 2.0);
	FuzzySetProxy& aggressivenessPassive = aggressiveness.AddLeftShoulderSet("aggressivenessPassive", 0.0, 0.5, 2.0);
}

map<string, double> FuzzyLogicFacade::ConstructDefuzzResultsMap() const
{
	map<string, double> defuzzResults = map<string, double>();
	defuzzResults.clear();

	double defuzzTight = FuzzyController::Instance()->DeFuzzify("tightness");
	double defuzzAgg = FuzzyController::Instance()->DeFuzzify("aggressiveness");

	defuzzResults["defuzzTight"] = defuzzTight;
	defuzzResults["defuzzAgg"] = defuzzAgg;


	return defuzzResults;
}

void FuzzyLogicFacade::CategoriseHumanPlaystyle()
{
	if (OpponentModeller::Instance()->GetTotalHands() > 0)
	{
		FuzzyController::Instance()->Fuzzify("tightness", OpponentModeller::Instance()->CalculateHandFoldPercentage());
		FuzzyController::Instance()->Fuzzify("aggressiveness", OpponentModeller::Instance()->CalculateRaiseToCallRatio());

		map<string, double> defuzzResults = ConstructDefuzzResultsMap();
		
		if (defuzzResults["defuzzTight"] > 0.5 && defuzzResults["defuzzAgg"] > 1.0)
		{
			mHumanPlaystyle = PLAYSTYLE_TIGHTAGGRESSIVE;
		}
		else if (defuzzResults["defuzzTight"] <= 0.5 && defuzzResults["defuzzAgg"] > 1.0)
		{
			mHumanPlaystyle = PLAYSTYLE_LOOSEAGGRESSIVE;
		}
		else if (defuzzResults["defuzzTight"] > 0.5 && defuzzResults["defuzzAgg"] <= 1.0)
		{
			mHumanPlaystyle = PLAYSTYLE_TIGHTPASSIVE;
		}
		else if (defuzzResults["defuzzTight"] <= 0.5 && defuzzResults["defuzzAgg"] <= 1.0)
		{
			mHumanPlaystyle = PLAYSTYLE_LOOSEPASSIVE;
		}
	}
}

string FuzzyLogicFacade::ConvertPlaystyleToString() const
{
	switch (mHumanPlaystyle)
	{
	case PLAYSTYLE_TIGHTPASSIVE:
		return "TightPassive";
	case PLAYSTYLE_LOOSEPASSIVE:
		return "LoosePassive";
	case PLAYSTYLE_TIGHTAGGRESSIVE:
		return "TightAggressive";
	case PLAYSTYLE_LOOSEAGGRESSIVE:
		return "LooseAggressive";
	case PLAYSTYLE_NONE:
		return "Uncategorised Playstyle";
	default:
		return "ERROR: Unable to convert playstyle to string successfully";
	}
}

