#ifndef _WORLD_H
#define _WORLD_H

#include "JellyPrerequisites.h"
#include "Vector2.h"
#include "Body.h"


namespace JellyPhysics 
{
	class CollisionCallback
	{
	public:
		virtual bool collisionFilter( Body* bA, int bodyApm, Body* bodyB, int bodyBpm1, int bodyBpm2, Vector2 hitPt, float normalVel ) { return true; }
	};
	
	class World 
	{
	public:
		
		struct MaterialPair
		{
			MaterialPair() : Collide(true), Elasticity(0.7f), Friction(0.3f), Callback(0) { }
			bool				Collide;
			float				Elasticity;
			float				Friction;
			CollisionCallback*	Callback;
		};
		
	private:
		typedef std::vector<Body*>	BodyList;
		
		BodyList		mBodies;
		AABB			mWorldLimits;
		Vector2			mWorldSize;
		Vector2			mWorldGridStep;
		
		float			mPenetrationThreshold;
		int				mPenetrationCount;
		
		MaterialPair*	mMaterialPairs;
		MaterialPair	mDefaultMatPair;
		int				mMaterialCount;
		
		struct BodyCollisionInfo 
		{
			void Clear() { bodyA = bodyB = 0; bodyApm = bodyBpmA = bodyBpmB = -1; hitPt = norm = Vector2::Zero; edgeD = penetration = 0.0f; }
			void Log() 
			{ 
				//printf("BCI bodyA:%d bodyB:%d bApm:%d bBpmA:%d, bBpmB:%d\n", bodyA, bodyB, bodyApm, bodyBpmA, bodyBpmB);
				//printf("	hitPt[%f][%f] edgeD:%f norm[%f][%f] penetration:%f\n",
				//	   hitPt.X, hitPt.Y, edgeD, norm.X, norm.Y, penetration);
			}
			
			Body*	bodyA;
			Body*	bodyB;
			
			int		bodyApm;
			int		bodyBpmA;
			int		bodyBpmB;
			
			Vector2	hitPt;
			float	edgeD;
			Vector2	norm;
			float	penetration;
		};
		
		std::vector<BodyCollisionInfo>		mCollisionList;
		
	public:
		
		World();
		~World();
		
		void killing();
		
		void setWorldLimits(const Vector2& min, const Vector2& max);
		
		int addMaterial();
		
		void setMaterialPairCollide(int a, int b, bool collide);
		void setMaterialPairData(int a, int b, float friction, float elasticity);
		void setMaterialPairFilterCallback(int a, int b, CollisionCallback* c);
		
		void addBody( Body* b );
		void removeBody( Body* b );
		void removeAllBodies();
		Body* getBody( unsigned int index );
		
		void getClosestPointMass( const Vector2& pt, int& bodyID, int& pmID );
		Vector2 getClosestBodyPointToBody(Body* body);
		Body* getBodyContaining( const Vector2& pt );
		
		void update( float elapsed );
		
	private:
		void updateBodyBitmask( Body* b );
		void sortBodyBoundaries();
		
		void _goNarrowCheck( Body* bI, Body* bJ );
		void bodyCollide( Body* bA, Body* bB, std::vector<BodyCollisionInfo>& infoList );
		void _handleCollisions();

		void _checkAndMoveBoundary( Body::BodyBoundary* bb );
		void _removeBoundary( Body::BodyBoundary* me );
		void _addBoundaryAfter( Body::BodyBoundary* me, Body::BodyBoundary* toAfterMe );
		void _addBoundaryBefore( Body::BodyBoundary* me, Body::BodyBoundary* toBeforeMe );
		
		void _logMaterialCollide();
		void _logBoundaries();
		
	public:			
		int getMaterialCount() { return mMaterialCount; }
		
		float getPenetrationThreshold() { return mPenetrationThreshold; }
		void setPenetrationThreshold( float val ) { mPenetrationThreshold = val; }
		
		int getPenetrationCount() { return mPenetrationCount; }
	};
}

#endif	// _WORLD_H