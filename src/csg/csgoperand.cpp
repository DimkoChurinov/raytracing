//-------------------------------------------------------------------
// File: csgoperand.cpp
// 
// Constructive solid geometry evaluation tree operand
// Operand can either be an operation or the object container, if it's a leaf node
// So this is the super class for value and operation. 
// Also it stays abstract, because all methods, inherited from IShape are still not implemented.
//
//  
//-------------------------------------------------------------------

#include "csgoperand.h"

CSGOperand::CSGOperand(CSGNode* lh, CSGNode* rh)
	: mLHand(lh),
		mRHand(rh)
{
}

CSGOperand::~CSGOperand()
{
	rmvChild();
}

void CSGOperand::rmvChild()
{

}