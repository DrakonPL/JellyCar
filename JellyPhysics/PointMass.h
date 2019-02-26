#ifndef _POINT_MASS_H
#define _POINT_MASS_H

#include "Vector2.h"

namespace JellyPhysics 
{
	
	class PointMass
	{
	public:
		float		Mass;
		Vector2		Position;
		Vector2		Velocity;
		Vector2		Force;
		
		PointMass() : Mass(0), Position(Vector2::Zero), Velocity(Vector2::Zero), Force(Vector2::Zero), lastElapsed(0), lastElapMass(1) { }
		PointMass( float mass, const Vector2& pos);
		
		void integrateForce( float elapsed );
		
	private:
		float		lastElapsed;
		float		lastElapMass;
	};
}

#endif // _POINT_MASS_H
