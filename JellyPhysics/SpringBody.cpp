#include "SpringBody.h"
#include "World.h"
#include "VectorTools.h"

namespace JellyPhysics 
{
	SpringBody::SpringBody( World* w, const ClosedShape& shape, float massPerPoint, 
			   float edgeSpringK, float edgeSpringDamp, 
			   const Vector2& pos, float angleinRadians,
						   const Vector2& scale, bool kinematic ) : 
	Body(w, shape, massPerPoint, pos, angleinRadians, scale, kinematic)
	{
		mShapeMatchingOn = false;
		
		setPositionAngle(pos, angleinRadians, scale);
		
		mEdgeSpringK = edgeSpringK;
		mEdgeSpringDamp = edgeSpringDamp;
		mShapeSpringK = 0.0f;
		mShapeSpringDamp = 0.0f;
		
		// build default springs.
		_buildDefaultSprings();
	}
	
	
	SpringBody::SpringBody(World* w, const ClosedShape& shape, float massPerPoint, 
			   float shapeSpringK, float shapeSpringDamp, 
			   float edgeSpringK, float edgeSpringDamp, 
			   const Vector2& pos, float angleinRadians, 
				const Vector2& scale, bool kinematic) :
	Body(w, shape, massPerPoint, pos, angleinRadians, scale, kinematic)
	{		
		setPositionAngle(pos, angleinRadians, scale);
		
		mShapeMatchingOn = true;
		mShapeSpringK = shapeSpringK;
		mShapeSpringDamp = shapeSpringDamp;
		mEdgeSpringK = edgeSpringK;
		mEdgeSpringDamp = edgeSpringDamp;
		
		// build default springs.
		_buildDefaultSprings();
	}	
	
	
	void SpringBody::addInternalSpring( int pointA, int pointB, float springK, float damping )
	{
		float dist = (mPointMasses[pointB].Position - mPointMasses[pointA].Position).length();
		InternalSpring s(pointA, pointB, dist, springK, damping);
		
		mSprings.push_back(s);
	}
	
	
	void SpringBody::clearAllSprings()
	{
		mSprings.clear();
		_buildDefaultSprings();
	}
	
	void SpringBody::_buildDefaultSprings()
	{
		for (int i = 0; i < mPointCount; i++)
		{
			if (i < (mPointCount - 1))
				addInternalSpring(i, i + 1, mEdgeSpringK, mEdgeSpringDamp);
			else
				addInternalSpring(i, 0, mEdgeSpringK, mEdgeSpringDamp);
		}
	}
	
	void SpringBody::setEdgeSpringConstants( float edgeSpringK, float edgeSpringDamp )
	{
		mEdgeSpringK = edgeSpringK;
		mEdgeSpringDamp = edgeSpringDamp;
		
		// we know that the first n springs in the list are the edge springs.
		for (int i = 0; i < mPointCount; i++)
		{
			mSprings[i].springK = edgeSpringK;
			mSprings[i].damping = edgeSpringDamp;
		}
	}
	
	
	void SpringBody::setSpringConstants( int springID, float springK, float springDamp )
	{
		// index is for all internal springs, AFTER the default internal springs.
		int index = mPointCount + springID;
		mSprings[index].springK = springK;
		mSprings[index].damping = springDamp;
	}
	
	
	
	float SpringBody::getSpringK( int springID )
	{
		int index = mPointCount + springID;
		return mSprings[index].springK;
	}
	
	
	float SpringBody::getSpringDamping( int springID )
	{
		int index = mPointCount + springID;
		return mSprings[index].damping;
	}
	
	void SpringBody::accumulateInternalForces()
	{		
		// internal spring forces.
		Vector2 force;
		int i = 0;
		for (SpringList::iterator it = mSprings.begin(); it != mSprings.end(); it++)
		{
			InternalSpring& s = (*it);
			PointMass& pmA = mPointMasses[s.pointMassA];
			PointMass& pmB = mPointMasses[s.pointMassB];
			
			if (i < mPointCount)
			{
				// spring forces for the edges of the shape can used the cached edge information to reduce calculations...
				force = VectorTools::calculateSpringForce(-mEdgeInfo[i].dir, mEdgeInfo[i].length, pmA.Velocity, pmB.Velocity,
														  s.springD, s.springK, s.damping );				
			}
			else
			{
				// these are other internal springs, they must be fully calculated each frame.
				force = VectorTools::calculateSpringForce(pmA.Position, pmA.Velocity,
														  pmB.Position, pmB.Velocity, 
														  s.springD, s.springK, s.damping);
			}
			
			pmA.Force += force;
			pmB.Force -= force;
			
			i++;
		}
		
		// shape matching forces.
		if (mShapeMatchingOn)
		{
			mBaseShape.transformVertices(mDerivedPos, mDerivedAngle, mScale, mGlobalShape);
			for (int i = 0; i < mPointCount; i++)
			{
				PointMass& pmA = mPointMasses[i];
				
				if (mShapeSpringK > 0)
				{
					if (!mKinematic)
					{
						force = VectorTools::calculateSpringForce(pmA.Position, pmA.Velocity,
														 mGlobalShape[i], pmA.Velocity, 0.0f, mShapeSpringK, mShapeSpringDamp);
					}
					else
					{
						force = VectorTools::calculateSpringForce(pmA.Position, pmA.Velocity,
														 mGlobalShape[i], Vector2::Zero, 0.0f, mShapeSpringK, mShapeSpringDamp);
					}
					
					pmA.Force += force;
				}
			}
		}
	}
}

