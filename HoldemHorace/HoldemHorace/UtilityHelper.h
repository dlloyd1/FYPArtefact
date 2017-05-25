#ifndef _UTILITYHELPER_H
#define _UTILITYHELPER_H
#include "Structures.h"
#include "Structures.h"

class UtilityHelper
{
public:
	~UtilityHelper();
	static UtilityHelper* Instance()
	{
		static UtilityHelper instance;
		return &instance;
	}

	void ConstructPayoffMatrix();
	int GetUtilityOfAction(const HANDOUTCOME outcome, const ACTION action);

private:
	UtilityHelper(const UtilityHelper&);
	UtilityHelper& operator=(const UtilityHelper&) {};//no copy or assignment
	UtilityHelper();

	map<HANDOUTCOME, map<ACTION, int>> mPayoffMatrix;

};
#endif // !_UTILITYHELPER_H
