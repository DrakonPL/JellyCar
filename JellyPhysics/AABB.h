#ifndef _AABB_H
#define _AABB_H

#include "Vector2.h"

namespace JellyPhysics {
	
	class AABB
	{
	public:
		enum PointValidity { Invalid, Valid };
		
		Vector2				Min;
		Vector2				Max;
		PointValidity		Validity;
		
		AABB() : Min(Vector2::Zero), Max(Vector2::Zero), Validity(Invalid) { }
		AABB( const Vector2& minPt, const Vector2& maxPt );
		
		void clear();
		
		void expandToInclude( const Vector2& pt );
		void expandToInclude( const AABB& aabb );
		
		bool contains( const Vector2& pt ) const;
		bool intersects( const AABB& box ) const;
		
		Vector2 getSize() const { return Max - Min; }
		
		Vector2 getCenter() { return ((Max - Min) / 2) + Min; }
	};
}

#endif	// _AABB_H