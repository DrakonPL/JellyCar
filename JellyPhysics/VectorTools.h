#ifndef _VECTOR_TOOLS_H
#define _VECTOR_TOOLS_H

#include "JellyPrerequisites.h"
#include "Vector2.h"

#include <math.h>
#include <vector>

namespace JellyPhysics 
{
	
	class VectorTools
	{
	public:		
		static inline Vector2 rotateVector( const Vector2& vecIn, float angleRadians )
		{
			Vector2 ret;
            float c = (float)cos(angleRadians);
            float s = (float)sin(angleRadians);
            ret.X = (c * vecIn.X) - (s * vecIn.Y);
            ret.Y = (c * vecIn.Y) + (s * vecIn.X);
			
			return ret;
		}
		
		static inline Vector2 rotateVector( const Vector2& vecIn, float cosAngle, float sinAngle )
		{
			Vector2 ret;
            ret.X = (cosAngle * vecIn.X) - (sinAngle * vecIn.Y);
            ret.Y = (cosAngle * vecIn.Y) + (sinAngle * vecIn.X);
			return ret;
		}
		
		static inline Vector2 reflectVector( const Vector2& V, const Vector2& N )
		{
			Vector2 ret = V - (N * (V.dotProduct(N) * 2.0f));
            return ret;
		}
		
		static inline void reflectVector(const Vector2& V, const Vector2& N, Vector2& vOut)
		{
			float dot = V.dotProduct(N);
            vOut = V - (N * (2.0f * dot));
		}
		
		static inline bool isCCW( const Vector2& A, const Vector2& B )
		{
			Vector2 perp = A.getPerpendicular();
			float dot = B.dotProduct(perp);
			return (dot >= 0.0f);
		}
		
		static bool lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
								  Vector2& hitPt, float& Ua, float& Ub );
		
		
		static bool lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
								  Vector2& hitPt );
		
		
		static Vector2 calculateSpringForce( const Vector2& posA, const Vector2& velA,
											const Vector2& posB, const Vector2& velB,
											float springD, float springK, float damping );
		
		static Vector2 calculateSpringForce( const Vector2& dir, float length, const Vector2& velA, const Vector2& velB,
											float springD, float springK, float damping );
		
		static inline float degToRad( float deg ) { return deg * (PI_OVER_ONE_EIGHTY); }
		static inline float radToDeg( float rad ) { return rad * (ONE_EIGHTY_OVER_PI); }
		
	};
}

#endif

