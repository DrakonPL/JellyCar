#include "VectorTools.h"

namespace JellyPhysics 
{
	bool VectorTools::lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
							  Vector2& hitPt, float& Ua, float& Ub )
	{
		hitPt = Vector2::Zero;
		Ua = 0.0f;
		Ub = 0.0f;
		
		float denom = ((ptD.Y - ptC.Y) * (ptB.X - ptA.X)) - ((ptD.X - ptC.X) * (ptB.Y - ptA.Y));
		
		// if denom == 0, lines are parallel - being a bit generous on this one..
		if (absf(denom) < 0.000001f)
			return false;
		
		float UaTop = ((ptD.X - ptC.X) * (ptA.Y - ptC.Y)) - ((ptD.Y - ptC.Y) * (ptA.X - ptC.X));
		float UbTop = ((ptB.X - ptA.X) * (ptA.Y - ptC.Y)) - ((ptB.Y - ptA.Y) * (ptA.X - ptC.X));
		
		Ua = UaTop / denom;
		Ub = UbTop / denom;
		
		if ((Ua >= 0.0f) && (Ua <= 1.0f) && (Ub >= 0.0f) && (Ub <= 1.0f))
		{
			// these lines intersect!
			hitPt = ptA + ((ptB - ptA) * Ua);
			return true;
		}
		
		return false;
	}
	
	
	bool VectorTools::lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
							  Vector2& hitPt )
	{
		float Ua, Ub;
		return lineIntersect( ptA, ptB, ptC, ptD, hitPt, Ua, Ub );
	}
	
	Vector2 VectorTools::calculateSpringForce( const Vector2& posA, const Vector2& velA,
										const Vector2& posB, const Vector2& velB,
										float springD, float springK, float damping)
	{
		Vector2 BtoA = (posA - posB);
		float dist = BtoA.length();
		if (dist > 0.0001f)
			BtoA /= dist;
		else
			BtoA = Vector2::Zero;
		
		dist = springD - dist;
		
		Vector2 relVel = velA - velB;
		float totalRelVel = relVel.dotProduct(BtoA);
		
		//printf("calculateSpringForce: dist[%f] k[%f]  totalRelVel[%f] damp[%f]\n", dist, springK, totalRelVel, damping);
		
		return BtoA * ((dist * springK) - (totalRelVel * damping));  
	}
	
	Vector2 VectorTools::calculateSpringForce( const Vector2& dir, float length, const Vector2& velA, const Vector2& velB,
											  float springD, float springK, float damping )
	{
		float dist = springD - length;
		
		Vector2 relVel = velA - velB;
		float totalRelVel = relVel.dotProduct(dir);
		
		//printf("calculateSpringForce: dist[%f] k[%f]  totalRelVel[%f] damp[%f]\n", dist, springK, totalRelVel, damping);
		
		return dir * ((dist * springK) - (totalRelVel * damping)); 
	}
}
