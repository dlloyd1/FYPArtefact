#include "UtilityHelper.h"

UtilityHelper::UtilityHelper()
{
	mPayoffMatrix = map<HANDOUTCOME, map<ACTION, int>>();
	ConstructPayoffMatrix();
}

UtilityHelper::~UtilityHelper()
{
	mPayoffMatrix.clear();
}

void UtilityHelper::ConstructPayoffMatrix()
{
	if (!mPayoffMatrix.empty())
	{
		mPayoffMatrix.clear();
		mPayoffMatrix = map<HANDOUTCOME, map<ACTION, int>>();
	}
	
	mPayoffMatrix[OUTCOME_WIN][ACTION_FOLD] = -1;
	mPayoffMatrix[OUTCOME_WIN][ACTION_CALL] = 0;
	mPayoffMatrix[OUTCOME_WIN][ACTION_RAISE] = 1;
	mPayoffMatrix[OUTCOME_LOSE][ACTION_FOLD] = 1;
	mPayoffMatrix[OUTCOME_LOSE][ACTION_CALL] = 0;
	mPayoffMatrix[OUTCOME_LOSE][ACTION_RAISE] = -1;
	mPayoffMatrix[OUTCOME_TIE][ACTION_FOLD] = -1;
	mPayoffMatrix[OUTCOME_TIE][ACTION_CALL] = 0;
}

int UtilityHelper::GetUtilityOfAction(const HANDOUTCOME outcome, const ACTION action)
{
	assert(mPayoffMatrix[outcome][action] == -1 ||
		mPayoffMatrix[outcome][action] == 0 ||
		mPayoffMatrix[outcome][action] == 1);

	return mPayoffMatrix[outcome][action];
}
