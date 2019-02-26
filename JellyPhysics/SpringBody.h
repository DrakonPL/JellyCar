#ifndef _SPRING_BODY_H
#define _SPRING_BODY_H

#include "Body.h"
#include "InternalSpring.h"

namespace JellyPhysics 
{
	class SpringBody : public Body 
	{
	protected:

		bool			mShapeMatchingOn;
		float			mEdgeSpringK;
		float			mEdgeSpringDamp;
		float			mShapeSpringK;
		float			mShapeSpringDamp;
		
	public:
		
		SpringBody( World* w, const ClosedShape& shape, float massPerPoint, 
				   float edgeSpringK, float edgeSpringDamp, 
				   const Vector2& pos, float angleInRadians,
				   const Vector2& scale, bool kinematic );
		
		SpringBody(World* w, const ClosedShape& shape, float massPerPoint, 
				   float shapeSpringK, float shapeSpringDamp, 
				   float edgeSpringK, float edgeSpringDamp, 
				   const Vector2& pos, float angleinRadians, 
				   const Vector2& scale, bool kinematic);
		
		void addInternalSpring( int pointA, int pointB, float springK, float damping );
		
		void clearAllSprings();
		
		void _buildDefaultSprings();
		
		void setShapeMatching( bool onoff ) { mShapeMatchingOn = onoff; }
		void setShapeMatchingConstants( float springK, float damping ) { mShapeSpringK = springK; mShapeSpringDamp = damping; }
		
		void setEdgeSpringConstants( float edgeSpringK, float edgeSpringDamp );		
		void setSpringConstants( int springID, float springK, float springDamp );
		
		float getSpringK( int springID );
		float getSpringDamping( int springID );
		
		void accumulateInternalForces();
	};
}

#endif // _INTERNAL_SPRING_H