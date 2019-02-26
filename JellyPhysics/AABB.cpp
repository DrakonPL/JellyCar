#include "AABB.h"

namespace JellyPhysics 
{
	
	AABB::AABB( const Vector2& minPt, const Vector2& maxPt )
	{
		Min = minPt;
		Max = maxPt;
		Validity = Valid;
	}
	
	void AABB::clear()
	{
		Min = Max = Vector2::Zero;
		Validity = Invalid;
	}
	
	void AABB::expandToInclude( const Vector2&  pt )
	{
		if (Validity == Valid)
		{
			if (pt.X < Min.X) { Min.X = pt.X; }
			else if (pt.X > Max.X) { Max.X = pt.X; }
			
			if (pt.Y < Min.Y) { Min.Y = pt.Y; }
			else if (pt.Y > Max.Y) { Max.Y = pt.Y; }
		}
		else
		{
			Min = Max = pt;
			Validity = Valid;
		}
	}
	
	void AABB::expandToInclude( const AABB& aabb )
	{
		expandToInclude( aabb.Min );
		expandToInclude( aabb.Max );
	}
	
	bool AABB::contains( const Vector2& pt ) const
	{
		if (Validity == Invalid) { return false; }
		
		return ((pt.X >= Min.X) && (pt.X <= Max.X) && (pt.Y >= Min.Y) && (pt.Y <= Max.Y));
	}
	
	bool AABB::intersects( const AABB& box ) const
	{
		bool overlapX = ((Min.X <= box.Max.X) && (Max.X >= box.Min.X));
		bool overlapY = ((Min.Y <= box.Max.Y) && (Max.Y >= box.Min.Y));
		
		return (overlapX && overlapY);
	}
}
