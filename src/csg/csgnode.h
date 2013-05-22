#ifndef CSG_CSGNODE_H
#define CSG_CSGNODE_H

#include "geometry/intersection.h"
#include "interfaces/ishape.h"
	
class Ray;

struct CSGNode : IShape
{
	virtual void rmvChild() = 0;
	virtual CSGNode* l() const = 0;
	virtual CSGNode* r() const = 0;
	virtual bool hasL() const = 0;
	virtual bool hasR() const = 0;
};
#endif