#include "PointMass.h"
#include <stdio.h>

namespace  JellyPhysics 
{

	PointMass::PointMass( float mass, const Vector2& pos )
	{
		Mass = mass;
		Position = pos;
		Velocity = Force = Vector2::Zero;
		lastElapsed = 0;
		lastElapMass = 1;
	}
	
	void PointMass::integrateForce( float elapsed )
	{
		if (Mass != 0.0f)
		{
			if (lastElapsed != elapsed)
			{
				lastElapsed = elapsed;
				lastElapMass = elapsed / Mass;
			}
			
			Velocity += (Force * lastElapMass);
			
			Position += (Velocity * elapsed);
		}
		
		Force = Vector2::Zero;
	}
	
}

