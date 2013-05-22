#ifndef CSG_CSGOPERATION_H
#define CSG_CSGOPERATION_H

#include "csgoperand.h"

class CSGOperation : public CSGOperand
{
public:
	explicit CSGOperation(CSGNode* lh, CSGNode* rh)
		: CSGOperand(lh, rh)
	{
	}
};

#endif