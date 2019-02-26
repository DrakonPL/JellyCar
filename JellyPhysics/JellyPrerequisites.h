#ifndef _JELLY_PREREQ_H
#define _JELLY_PREREQ_H

#include "Vector2.h"
#include <vector>

namespace JellyPhysics 
{
	
	typedef std::vector<Vector2> Vector2List;
	
	static const float PI = 3.14159265f;
	static const float TWO_PI = (3.14159265f * 2.0f);
	static const float HALF_PI = (3.14159265f * 0.5f);
	static const float PI_OVER_ONE_EIGHTY = (3.14159265f / 180.0f);
	static const float ONE_EIGHTY_OVER_PI = (180.0f / 3.14159265f);
	static const float NEG_PI_OVER_ONE_POINT_TWO = -2.617994f;
	
	static inline float absf( float v ) { return (v >= 0.0f) ? v : -v; }
}

#endif	// _JELLY_PREREQ_H