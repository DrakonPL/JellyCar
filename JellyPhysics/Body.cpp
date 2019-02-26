#include "Body.h"
#include "World.h"
#include "VectorTools.h"

#include <stdio.h>

namespace JellyPhysics 
{
	
	void Body::BodyBoundary::log()
	{
		//printf("%s(%d)[%4.2f] |", ((type == Begin) ? "B" : ((type == End) ? "E" : "V")), body, value); 
	}
	
	//--------------------------------------------------------------------
	Body::Body( World* w )
	{
		mWorld = w;
		mScale = Vector2::One;
		mIsStatic = false;
		mKinematic = false;
		
		mVelDamping = 0.999f;
		mObjectTag = NULL;
		
		mMaterial = 0;
		
		mPointCount = 0;
		mInvPC = 0.0f;
		
		mBoundStart.body = this;
		mBoundEnd.body = this;
		mBoundEnd.type = BodyBoundary::End;
		
		mIgnoreMe = false;
		mDisable = false;
		
		w->addBody( this );
	}
	
	//--------------------------------------------------------------------
	Body::Body( World* w, const ClosedShape& shape, float massPerPoint, Vector2 position, float angleInRadians, Vector2 scale, bool kinematic)
	{
		mWorld = w;
		mDerivedPos = position;
		mDerivedAngle = angleInRadians;
		mLastAngle = mDerivedAngle;
		mScale = scale;
		mMaterial = 0;
		mPointCount = 0;
		mInvPC = 0.0f;
		mIsStatic = (massPerPoint == 0.0f);
		mKinematic = kinematic;
		
		mVelDamping = 0.999f;
		mObjectTag = NULL;
		
		mBoundStart.body = this;
		mBoundEnd.body = this;
		mBoundEnd.type = BodyBoundary::End;
		
		mIgnoreMe = false;
		mDisable = false;
		
		setShape(shape);
		for (int i = 0; i < mPointCount; i++)
			mPointMasses[i].Mass = massPerPoint;
		
		updateAABB(0.0f, true);
		updateEdgeInfo(true);
		updateBoundaryValues(true);
		
		w->addBody( this );
	}
	
	//--------------------------------------------------------------------
	Body::Body( World* w, const ClosedShape& shape, std::vector<float> pointMasses, Vector2 position, float angleInRadians, Vector2 scale, bool kinematic)
	{
		mWorld = w;
		mDerivedPos = position;
		mDerivedAngle = angleInRadians;
		mLastAngle = mDerivedAngle;
		mScale = scale;
		mMaterial = 0;
		mPointCount = 0;
		mInvPC = 0.0f;
		
		mIsStatic = false;
		mKinematic = kinematic;
		
		mVelDamping = 0.999f;
		mObjectTag = NULL;
		
		mBoundStart.body = this;
		mBoundEnd.body = this;
		mBoundEnd.type = BodyBoundary::End;
		
		mIgnoreMe = false;
		mDisable = false;
		
		setShape(shape);
		for (int i = 0; i < mPointCount; i++)
			mPointMasses[i].Mass = pointMasses[i];
		
		updateAABB(0.0f, true);
		updateEdgeInfo(true);
		updateBoundaryValues(true);
		
		w->addBody( this );
	}
	
	//--------------------------------------------------------------------
	Body::~Body()
	{
		mSprings.clear();
		mGlobalShape.clear();
		mPointMasses.clear();
		mEdgeInfo.clear();

		mWorld->removeBody(this);
	}
	
	//--------------------------------------------------------------------
	void Body::setShape(ClosedShape shape)
	{
		mBaseShape = shape;
		
		if (mBaseShape.getVertices().size() != mPointCount)
		{
			mPointMasses.clear();
			mGlobalShape.clear();
			mEdgeInfo.clear();
			
			for (unsigned int i = 0; i < shape.getVertices().size(); i++)
				mGlobalShape.push_back( Vector2::Zero );
			
			mBaseShape.transformVertices(mDerivedPos, mDerivedAngle, mScale, mGlobalShape);
			
			for (unsigned int i = 0; i < mBaseShape.getVertices().size(); i++)
				mPointMasses.push_back(PointMass(0.0f, mGlobalShape[i])); 
			
			EdgeInfo e;
			e.dir = Vector2::Zero;
			e.length = 0.0f;
			
			for (unsigned int i = 0; i < mBaseShape.getVertices().size(); i++)
				mEdgeInfo.push_back(e);
			
			mPointCount = mPointMasses.size();
			mInvPC = 1.0f / mPointCount;
		}
		
		updateAABB(0.0f, true);
		updateEdgeInfo(true);
		updateBoundaryValues(true);
	}
	
	//--------------------------------------------------------------------
	void Body::setMassAll(float mass)
	{
		for (int i = 0; i < mPointCount; i++)
			mPointMasses[i].Mass = mass;
		
		if (mass == 0.0f) { mIsStatic = true; }
	}
	
	//--------------------------------------------------------------------
	void Body::setMassIndividual( int index, float mass )
	{
		if ((index >= 0) && (index < mPointCount))
			mPointMasses[index].Mass = mass;
	}
	
	//--------------------------------------------------------------------
	void Body::setMassFromList( std::vector<float> masses )
	{
		if (masses.size() == mPointCount)
		{
			for (int i = 0; i < mPointCount; i++)
				mPointMasses[i].Mass = masses[i];
		}
	}
	
	//--------------------------------------------------------------------
	void Body::setPositionAngle( const Vector2& pos, float angleInRadians, const Vector2& scale )
	{
		mBaseShape.transformVertices(pos, angleInRadians, scale, mGlobalShape);
		for (int i = 0; i < mPointCount; i++)
			mPointMasses[i].Position = mGlobalShape[i];
		
		mDerivedPos = pos;
		mDerivedAngle = angleInRadians;
	}
	
	//--------------------------------------------------------------------
	void Body::derivePositionAndAngle(float elapsed)
	{
		// no need it this is a static body, or kinematically controlled.
		if (mIsStatic || mKinematic)
			return;
	
		// if we are being ignored, be ignored!
		if (mIgnoreMe)
			return;
		
		// find the geometric center.
		Vector2 center = Vector2::Zero;
		Vector2 vel = Vector2::Zero;
		
		for (PointMassList::iterator it = mPointMasses.begin(); it != mPointMasses.end(); it++)
		{
			center += (*it).Position;
			vel += (*it).Velocity;
		}
		
		center *= mInvPC;
		vel *= mInvPC;
		
		mDerivedPos = center;
		mDerivedVel = vel;
		
		// find the average angle of all of the masses.
		float angle = 0;
		int originalSign = 1;
		float originalAngle = 0;
		for (int i = 0; i < mPointCount; i++)
		{
			Vector2 baseNorm = mBaseShape.getVertices()[i];
			baseNorm.normalise();
			
			Vector2 curNorm = mPointMasses[i].Position - mDerivedPos;
			curNorm.normalise();
			
			float dot = baseNorm.dotProduct(curNorm);
			if (dot > 1.0f) { dot = 1.0f; }
			if (dot < -1.0f) { dot = -1.0f; }
			
			float thisAngle = (float)acos(dot);
			if (!JellyPhysics::VectorTools::isCCW(baseNorm, curNorm)) { thisAngle = -thisAngle; }
			
			if (i == 0)
			{
				originalSign = (thisAngle >= 0.0f) ? 1 : -1;
				originalAngle = thisAngle;
			}
			else
			{
				float diff = (thisAngle - originalAngle);
				int thisSign = (thisAngle >= 0.0f) ? 1 : -1;
				
				if ((absf(diff) > PI) && (thisSign != originalSign))
				{
					thisAngle = (thisSign == -1) ? ((float)PI + ((float)PI + thisAngle)) : (((float)PI - thisAngle) - (float)PI);
				}
			}
			
			angle += thisAngle;
		}
		
		angle *= mInvPC;
		mDerivedAngle = angle;
		
		// now calculate the derived Omega, based on change in angle over time.
		float angleChange = (mDerivedAngle - mLastAngle);
		if (absf(angleChange) >= PI)
		{
			if (angleChange < 0.0f)
				angleChange = angleChange + TWO_PI;
			else
				angleChange = angleChange - TWO_PI;
		}
		
		mDerivedOmega = angleChange / elapsed;

		mLastAngle = mDerivedAngle;
		
	}
	
	//--------------------------------------------------------------------
	void Body::integrate( float elapsed )
	{
		if (mIsStatic || mIgnoreMe) { return; }
		
		for (PointMassList::iterator it = mPointMasses.begin(); it != mPointMasses.end(); it++)
			(*it).integrateForce(elapsed);
	}
	
	//--------------------------------------------------------------------
	void Body::dampenVelocity()
	{
		if (mIsStatic || mIgnoreMe) { return; }
		
		for (PointMassList::iterator it = mPointMasses.begin(); it != mPointMasses.end(); it++)
		{
			(*it).Velocity *= mVelDamping;
		}
	}
	
	//--------------------------------------------------------------------
	void Body::updateAABB(float elapsed, bool forceUpdate)
	{
		if (((!mIsStatic) && (!mIgnoreMe)) || (forceUpdate))
		{
			mAABB.clear();
			for (PointMassList::iterator it = mPointMasses.begin(); it != mPointMasses.end(); it++)
			{
				Vector2 p = (*it).Position;
				mAABB.expandToInclude(p);
				
				// expanding for velocity only makes sense for dynamic objects.
				if (!mIsStatic)
				{
					p += (*it).Velocity * elapsed;
					mAABB.expandToInclude(p);
				}
			}
			
			//printf("Body: %d AABB: min[%f][%f] max[%f][%f]\n", this, mAABB.Min.X, mAABB.Min.Y, mAABB.Max.X, mAABB.Max.Y);
		}
	}
	
	//--------------------------------------------------------------------
	void Body::updateBoundaryValues(bool forceUpdate)
	{
		if (((!mIsStatic) && (!mIgnoreMe)) || (forceUpdate))
		{
			mBoundStart.value = mAABB.Min.X;
			mBoundEnd.value = mAABB.Max.X;
		}
	}
	
	//--------------------------------------------------------------------
	void Body::updateEdgeInfo(bool forceUpdate)
	{
		if (((!mIsStatic) && (!mIgnoreMe)) || (forceUpdate))
		{
			for (int i = 0; i < mPointCount; i++)
			{
				int j = (i < (mPointCount-1)) ? i+1 : 0;
				
				Vector2 e = mPointMasses[j].Position - mPointMasses[i].Position;
				mEdgeInfo[i].length = e.normalise();
				mEdgeInfo[i].dir = e;
				mEdgeInfo[i].slope = (absf(e.Y) < 1.0e-08) ? 0.0f : (e.X / e.Y);
			}
		}
	}
	
	//--------------------------------------------------------------------
	bool Body::contains( const Vector2& pt )
	{
		// basic idea: draw a line from the point to a point known to be outside the body.  count the number of
		// lines in the polygon it intersects.  if that number is odd, we are inside.  if it's even, we are outside.
		// in this implementation we will always use a line that moves off in the positive X direction from the point
		// to simplify things.
		Vector2 endPt = Vector2(mAABB.Max.X + 0.1f, pt.Y);
		
		// line we are testing against goes from pt -> endPt.
		bool inside = false;
		Vector2 edgeSt = mPointMasses[0].Position;
		Vector2 edgeEnd;
		int c = mPointCount;
		for (int i = 0; i < c; i++)
		{
			// the current edge is defined as the line from edgeSt -> edgeEnd.
			if (i < (c - 1))
				edgeEnd = mPointMasses[i + 1].Position;
			else
				edgeEnd = mPointMasses[0].Position;
			
			// perform check now...
			if (((edgeSt.Y <= pt.Y) && (edgeEnd.Y > pt.Y)) || ((edgeSt.Y > pt.Y) && (edgeEnd.Y <= pt.Y)))
			{
				// this line crosses the test line at some point... does it do so within our test range?
				float slope = mEdgeInfo[i].slope; //(edgeEnd.X - edgeSt.X) / (edgeEnd.Y - edgeSt.Y);
				float hitX = edgeSt.X + ((pt.Y - edgeSt.Y) * slope);
				
				if ((hitX >= pt.X) && (hitX <= endPt.X))
					inside = !inside;
			}
			edgeSt = edgeEnd;
		}
		
		return inside;
	}
	
	//--------------------------------------------------------------------
	float Body::getClosestPoint( const Vector2& pt, Vector2& hitPt, Vector2& norm, int& pointA, int& pointB, float& edgeD )
	{
		hitPt = Vector2::Zero;
		pointA = -1;
		pointB = -1;
		edgeD = 0.0f;
		norm = Vector2::Zero;
		
		float closestD = 1000.0f;
		
		for (int i = 0; i < mPointCount; i++)
		{
			Vector2 tempHit;
			Vector2 tempNorm;
			float tempEdgeD;
			
			float dist = getClosestPointOnEdge(pt, i, tempHit, tempNorm, tempEdgeD);
			if (dist < closestD)
			{
				closestD = dist;
				pointA = i;
				if (i < (mPointCount - 1))
					pointB = i + 1;
				else
					pointB = 0;
				edgeD = tempEdgeD;
				norm = tempNorm;
				hitPt = tempHit;
			}
		}
		
		
		// return.
		return closestD;
	}
	
	//--------------------------------------------------------------------
	float Body::getClosestPointOnEdge( const Vector2& pt, int edgeNum, Vector2& hitPt, Vector2& norm, float& edgeD )
	{
		hitPt = Vector2::Zero;
		norm = Vector2::Zero;
		
		edgeD = 0.0f;
		float dist = 0.0f;
		
		Vector2 ptA = mPointMasses[edgeNum].Position;
		Vector2 ptB;
		
		if (edgeNum < (mPointCount - 1))
			ptB = mPointMasses[edgeNum + 1].Position;
		else
			ptB = mPointMasses[0].Position;
		
		Vector2 toP = pt - ptA;		
		Vector2 E = mEdgeInfo[edgeNum].dir;

		// get the length of the edge, and use that to normalize the vector.
		float edgeLength = mEdgeInfo[edgeNum].length;
		
		// normal
		Vector2 n = E.getPerpendicular();
		
		// calculate the distance!
		float x = toP.dotProduct(E);
		
		if (x <= 0.0f)
		{
			// x is outside the line segment, distance is from pt to ptA.
			dist = (pt - ptA).length();
			hitPt = ptA;
			edgeD = 0.0f;
			norm = n;
		}
		else if (x >= edgeLength)
		{
			// x is outside of the line segment, distance is from pt to ptB.
			dist = (pt - ptB).length();
			hitPt = ptB;
			edgeD = 1.0f;
			norm = n;
		}
		else
		{
			// point lies somewhere on the line segment.			
			dist = absf(toP.crossProduct(E));
			hitPt = ptA + (E * x);
			edgeD = x / edgeLength;
			norm = n;
		}
		
		return dist;
	}
	
	//--------------------------------------------------------------------
	float Body::getClosestPointOnEdgeSquared( const Vector2& pt, int edgeNum, Vector2& hitPt, Vector2& norm, float& edgeD )
	{
		hitPt = Vector2::Zero;
		norm = Vector2::Zero;
		
		edgeD = 0.0f;
		float dist = 0.0f;
		
		Vector2 ptA = mPointMasses[edgeNum].Position;
		Vector2 ptB;
		
		if (edgeNum < (mPointCount - 1))
			ptB = mPointMasses[edgeNum + 1].Position;
		else
			ptB = mPointMasses[0].Position;
		
		Vector2 toP = pt - ptA;
		Vector2 E = mEdgeInfo[edgeNum].dir;
		
		// get the length of the edge, and use that to normalize the vector.
		float edgeLength = mEdgeInfo[edgeNum].length;
		
		// normal
		Vector2 n = E.getPerpendicular();
		
		// calculate the distance!
		float x = toP.dotProduct(E);
		if (x <= 0.0f)
		{
			// x is outside the line segment, distance is from pt to ptA.
			dist = (pt - ptA).lengthSquared();
			//Vector2.DistanceSquared(ref pt, ref ptA, out dist);
			hitPt = ptA;
			edgeD = 0.0f;
			norm = n;
			
			//printf("getClosestPointonEdgeSquared - closest is ptA: %f\n", dist);
		}
		else if (x >= edgeLength)
		{
			// x is outside of the line segment, distance is from pt to ptB.
			dist = (pt - ptB).lengthSquared();
			//Vector2.DistanceSquared(ref pt, ref ptB, out dist);
			hitPt = ptB;
			edgeD = 1.0f;
			norm = n;
			
			//printf("getClosestPointonEdgeSquared - closest is ptB: %f\n", dist);
		}
		else
		{
			// point lies somewhere on the line segment.			
			dist = toP.crossProduct(E);
			//Vector3.Cross(ref toP3, ref E3, out E3);
			dist = (dist * dist);
			hitPt = ptA + (E * x);
			edgeD = x / edgeLength;
			norm = n;
			
			//printf("getClosestPointonEdgeSquared - closest is at %f: %f\n", edgeD, dist);
		}
		
		return dist;
	}
	
	//--------------------------------------------------------------------
	int Body::getClosestPointMass( const Vector2& pos, float& dist )
	{
		float closestSQD = 100000.0f;
		int closest = -1;
		
		for (int i = 0; i < mPointCount; i++)
		{
			float thisD = (pos - mPointMasses[i].Position).lengthSquared();
			if (thisD < closestSQD)
			{
				closestSQD = thisD;
				closest = i;
			}
		}
		
		dist = (float)sqrt(closestSQD);
		return closest;
	}

	void Body::setVelocity(Vector2 velocity)
	{
		for (PointMassList::iterator it = mPointMasses.begin(); it != mPointMasses.end(); it++)
		{
			(*it).Velocity = velocity;
		}
	}
	
	//--------------------------------------------------------------------
	void Body::addGlobalForce( const Vector2& pt, const Vector2& force )
	{
		Vector2 R = (mDerivedPos - pt);
		
		float torqueF = R.crossProduct(force);
		
		for (PointMassList::iterator it = mPointMasses.begin(); it != mPointMasses.end(); it++)
		{
			Vector2 toPt = ((*it).Position - mDerivedPos);
			Vector2 torque = JellyPhysics::VectorTools::rotateVector(toPt, -HALF_PI);
			
			(*it).Force += torque * torqueF;
			(*it).Force += force;
		}
	}


	
}

