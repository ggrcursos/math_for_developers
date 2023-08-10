#ifndef AABB_H  
#define AABB_H
#include "vector.h"

class AABB
{
public:
	AABB operator+(const Point& p) const
	{
		AABB result = (*this);
		result.vecMin = p + vecMin;
		result.vecMax = p + vecMax;
		return result;
	}

public:
	Vector vecMin;
	Vector vecMax;
};
#endif