#ifndef CSG_CSGOPERAND_H
#define CSG_CSGOPERAND_H

#include "csgnode.h"

class CSGOperand : public CSGNode
{
public:
	explicit CSGOperand(CSGNode* lh, CSGNode* rh);
	~CSGOperand();
	virtual void rmvChild();
	virtual CSGNode* l() const
	{
		return mLHand;
	}
	virtual CSGNode* r() const
	{
		return mRHand;
	}
	virtual bool hasL() const
	{
		return mLHand != NULL;
	}
	virtual bool hasR() const
	{
		return mRHand != NULL;
	}

private:
	CSGNode *mLHand;
	CSGNode *mRHand;
};

#endif