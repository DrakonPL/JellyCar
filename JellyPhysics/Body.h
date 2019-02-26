#ifndef _BODY_H
#define _BODY_H

#include "JellyPrerequisites.h"
#include "Vector2.h"
#include "ClosedShape.h"
#include "Bitmask.h"
#include "AABB.h"
#include "PointMass.h"
#include "InternalSpring.h"

#include <vector>
#include <string>

namespace JellyPhysics 
{
	class World;
	
	class Body
	{
	protected:

		std::string _name;

	public:
		typedef std::vector<PointMass> PointMassList;
		typedef std::vector<InternalSpring>	SpringList;
		
		struct EdgeInfo
		{
			Vector2		dir;	// normalized direction vector for this edge.
			float		length;	// length of the edge.
			float		slope;	// slope of the line described by this edge.
		};
		
		typedef std::vector<EdgeInfo> EdgeInfoList;
		
		World*					mWorld;
		ClosedShape				mBaseShape;
		Vector2List				mGlobalShape;
		PointMassList			mPointMasses;
		EdgeInfoList			mEdgeInfo;
		Vector2					mScale;
		Vector2					mDerivedPos;
		Vector2					mDerivedVel;
		SpringList				mSprings;
		float					mDerivedAngle;
		float					mDerivedOmega;
		float					mLastAngle;
		AABB					mAABB;
		int						mMaterial;
		bool					mIsStatic;
		bool					mKinematic;
		void*					mObjectTag;
		float					mVelDamping;
		
		int						mPointCount;
		float					mInvPC;
		
		bool					mIgnoreMe;
		bool					mDisable;
		
		int 					dragPoint;
		Vector2 				dragForce;
		
	public:
		struct BodyBoundary
		{
			enum BoundaryType { Begin, End, VoidMarker };
			
			float			value;
			BoundaryType	type;
			
			BodyBoundary*	next;
			BodyBoundary*	prev;
			
			Body*			body;
			
			BodyBoundary() : type(Begin), value(0.0f), next(0), prev(0), body(0) { }
			BodyBoundary( Body* b, BoundaryType t, float v ) : next(0), prev(0) { body = b; type = t; value = v; }
			
			void log(); 
		};
		
		//Bitmask					mBitMaskX;
		Bitmask					mBitMaskY;
		
		BodyBoundary			mBoundStart;
		BodyBoundary			mBoundEnd;
		
		Body( World* w );
		Body( World* w, const ClosedShape& shape, float massPerPoint, Vector2 position, float angleInRadians, Vector2 scale, bool kinematic);
		Body( World* w, const ClosedShape& shape, std::vector<float> pointMasses, Vector2 position, float angleInRadians, Vector2 scale, bool kinematic);
		virtual ~Body();
		
		void setShape(ClosedShape shape);
		
		void setMassAll(float mass);
		void setMassIndividual( int index, float mass );
		void setMassFromList( std::vector<float> masses );
		
		int getMaterial() { return mMaterial; }
		void setMaterial( int val ) { mMaterial = val; }
		
		void setPositionAngle( const Vector2& pos, float angleInRadians, const Vector2& scale );
		
		virtual void setKinematicPosition( const Vector2& pos ) { mDerivedPos = pos; }
		virtual void setKinematicAngle( float angleInRadians ) { mDerivedAngle = angleInRadians; }
		virtual void setKinematicScale( const Vector2& scale ) { mScale = scale; }
		
		void derivePositionAndAngle(float elapsed);
		
		void updateEdgeInfo(bool forceUpdate = false);
		
		virtual void updateBoundaryValues(bool forceUpdate = false);
		
		Vector2 getDerivedPosition() { return mDerivedPos; }
		float getDerivedAngle() { return mDerivedAngle; }
		Vector2 getDerivedVelocity() { return mDerivedVel; }
		float getDerivedOmega() { return mDerivedOmega; }
		
		Vector2 getScale() { return mScale; }
		
		virtual void accumulateInternalForces() {}
		virtual void accumulateExternalForces() {}
		
		void integrate( float elapsed );
		void dampenVelocity();
		
		void updateAABB(float elapsed, bool forceUpdate = false);
		const AABB& getAABB() { return mAABB; }
		
		bool contains( const Vector2& pt );
		
		float getClosestPoint( const Vector2& pt, Vector2& hitPt, Vector2& norm, int& pointA, int& pointB, float& edgeD );
		float getClosestPointOnEdge( const Vector2& pt, int edgeNum, Vector2& hitPt, Vector2& norm, float& edgeD );
		float getClosestPointOnEdgeSquared( const Vector2& pt, int edgeNum, Vector2& hitPt, Vector2& norm, float& edgeD );
		int getClosestPointMass( const Vector2& pos, float& dist );
		
		int getPointMassCount() { return mPointCount; }
		PointMass* getPointMass( int index ) { return &mPointMasses[index]; }
		
		void addGlobalForce( const Vector2& pt, const Vector2& force );
		
		void SetName(const std::string &name) { _name = name; }
		std::string GetName() { return _name; }

		void setVelocity(Vector2 velocity);
		
		bool getIsStatic() { return mIsStatic; }
		void setIsStatic( bool val ) { mIsStatic = val; }
		
		bool getIsKinematic() { return mKinematic; }
		void setIsKinematic( bool val ) { mKinematic = val; }
		
		float getVelocityDamping() { return mVelDamping; }
		void setVelocityDamping( float val ) { mVelDamping = val; }
		
		void* getObjectTag() { return mObjectTag; }
		void setObjectTag( void* obj ) { mObjectTag = obj; }
		
		bool getIgnoreMe() { return mIgnoreMe; }
		void setIgnoreMe( bool setting ) { mIgnoreMe = setting; }

		bool getDisable() { return mDisable; }
		void setDisable(bool disable) { mDisable = disable; }
		
		void setDragForce(Vector2 force, int pm)
		{
			dragForce = force * 2;
			dragPoint = pm;
		}
		
	};
}

#endif // _BODY_H
