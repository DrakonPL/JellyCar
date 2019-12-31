#include "World.h"

namespace JellyPhysics 
{
	World::World()
	{
		mMaterialCount = 1;
		mMaterialPairs = new MaterialPair[1];
		mDefaultMatPair.Friction = 0.3f;
		mDefaultMatPair.Elasticity = 0.8f;
		mDefaultMatPair.Collide = true;
		
		mMaterialPairs[0] = mDefaultMatPair;
		
		setWorldLimits(Vector2(-20,-20), Vector2(20,20));
		
		mPenetrationThreshold = 0.3f;
		mPenetrationCount = 0;
	}
	
	World::~World()
	{
		
		delete[] mMaterialPairs;
	}
										 
	 void World::killing()
	 {
		 // clear up all "VoidMarker" elements in the list...
		 if (mBodies.size() > 0)
		 {
			 Body::BodyBoundary* bb = &mBodies[0]->mBoundStart;
			 
			 while (bb->prev)
				 bb = bb->prev;
			 
			 while (bb)
			 {
				 if (bb->type == Body::BodyBoundary::VoidMarker)
				 {
					 // remove this one!
					 _removeBoundary(bb);
					 Body::BodyBoundary* theNext = bb->next;
					 
					 delete bb;
					 
					 bb = theNext;
					 continue;
				 }
				 
				 bb = bb->next;
			 }
		 }
	 }
		
	void World::setWorldLimits(const Vector2& min, const Vector2& max)
	{
		mWorldLimits = AABB(min, max);
		mWorldSize = max - min;
		mWorldGridStep = mWorldSize / 32;
		
		// update bitmasks for all bodies.
		for (BodyList::iterator it = mBodies.begin(); it != mBodies.end(); it++)
			updateBodyBitmask((*it));
	}
	
	int World::addMaterial()
	{
		MaterialPair* old = new MaterialPair[mMaterialCount * mMaterialCount];
		for (int i = 0; i < mMaterialCount; i++)
		{
			for (int j = 0; j < mMaterialCount; j++)
			{
				old[(i*mMaterialCount)+j] = mMaterialPairs[(i*mMaterialCount)+j];
			}
		}
		
		mMaterialCount++;
		
		delete[] mMaterialPairs;
		mMaterialPairs = new MaterialPair[mMaterialCount * mMaterialCount];
		
		for (int i = 0; i < mMaterialCount; i++)
		{
			for (int j = 0; j < mMaterialCount; j++)
			{
				if ((i < (mMaterialCount-1)) && (j < (mMaterialCount-1)))
					mMaterialPairs[(i*mMaterialCount)+j] = old[(i*(mMaterialCount-1))+j];
				else
					mMaterialPairs[(i*mMaterialCount)+j] = mDefaultMatPair;
			}
		}
		
#ifdef _DEBUG
		//printf("addMaterial - final results...\n");
		_logMaterialCollide();
#endif

		delete[] old;
		
		return mMaterialCount - 1;
	}
	
	void World::setMaterialPairCollide(int a, int b, bool collide)
	{
#ifdef _DEBUG
		//printf("setMaterialPairCollide: %d vs %d %s\n", a, b, (collide ? "ON" : "OFF"));
#endif
		
		if ((a >= 0) && (a < mMaterialCount) && (b >= 0) && (b < mMaterialCount))
		{
			mMaterialPairs[(a*mMaterialCount)+b].Collide = collide;
			mMaterialPairs[(b*mMaterialCount)+a].Collide = collide;
		}

#ifdef _DEBUG
		_logMaterialCollide();
#endif
		
	}
	
	void World::setMaterialPairData(int a, int b, float friction, float elasticity)
	{
#ifdef _DEBUG
		//printf("setMaterialPairData: %d vs %d : f:%f e:%f\n", a, b, friction, elasticity);
#endif
		
		if ((a >= 0) && (a < mMaterialCount) && (b >= 0) && (b < mMaterialCount))
		{
			mMaterialPairs[(a*mMaterialCount)+b].Friction = friction;
			mMaterialPairs[(b*mMaterialCount)+a].Elasticity = elasticity;
			
			mMaterialPairs[(a*mMaterialCount)+b].Friction = friction;
			mMaterialPairs[(b*mMaterialCount)+a].Elasticity = elasticity;
		}
		
#ifdef _DEBUG
		_logMaterialCollide();
#endif
	}
	
	
	void World::setMaterialPairFilterCallback(int a, int b, CollisionCallback* c)
	{
		if ((a >= 0) && (a < mMaterialCount) && (b >= 0) && (b < mMaterialCount))
		{
			mMaterialPairs[(a*mMaterialCount)+b].Callback = c;
			mMaterialPairs[(b*mMaterialCount)+a].Callback = c;
		}
	}
	
	void World::addBody( Body* b )
	{
#ifdef _DEBUG
		//printf("addBody: %d\n", b);
#endif
		
		// check for already existing.
		bool exists = false;
		for (unsigned int i = 0; i < mBodies.size(); i++)
			if (mBodies[i] == b) { exists = true; break; }
		
		if (!exists)
		{
			mBodies.push_back( b );
			
			if (mBodies.size() > 1)
			{
				_addBoundaryAfter( &b->mBoundStart, &mBodies[0]->mBoundStart );				
			}
			
			_addBoundaryAfter( &b->mBoundEnd, &b->mBoundStart );
			
#ifdef _DEBUG
			_logBoundaries();
#endif
		}
	}
	
	void World::removeBody( Body* b )
	{
#ifdef _DEBUG
		//printf("removeBody: %d\n", b);
#endif
		
		std::vector<Body*>::iterator it = mBodies.begin();
		while (it != mBodies.end())
		{
			if ((*it) == b)
			{
				mBodies.erase( it );
				_removeBoundary(&b->mBoundStart);
				_removeBoundary(&b->mBoundEnd);
				
#ifdef _DEBUG
				_logBoundaries();
#endif
				
				break;
			}
			
			it++;
		}
	}

	void World::removeAllBodies()
	{
		mBodies.clear();
	}
	
	Body* World::getBody(unsigned int index )
	{
		if ((index >= 0) && (index < mBodies.size()))
			return mBodies[index];
		
		return 0;
	}
	
	
	void World::getClosestPointMass( const Vector2& pt, int& bodyID, int& pmID )
	{
		bodyID = -1;
		pmID = -1;
		
		float closestD = 1000.0f;
		for (unsigned int i = 0; i < mBodies.size(); i++)
		{
			float dist = 0.0f;
			int pm = mBodies[i]->getClosestPointMass(pt, dist);
			if (dist < closestD)
			{
				closestD = dist;
				bodyID = i;
				pmID = pm;
			}
		}
	}

	Vector2 World::getClosestBodyPointToBody(Body* body)
	{
		AABB bodyAABB = body->getAABB();
		Vector2 pt = bodyAABB.getCenter();
		Vector2 point;

		float closestD = 1000.0f;
		for (unsigned int i = 0; i < mBodies.size(); i++)
		{
			if (mBodies[i] != body && mBodies[i]->getIsStatic())
			{
				Vector2 norm, hitPt;
				int pointA, pointB;
				float edgeD;

				float dist = mBodies[i]->getClosestPoint(pt, hitPt, norm, pointA, pointB, edgeD);
				if (dist < closestD)
				{
					closestD = dist;
					point = hitPt;
				}
			}
		}

		return point;
	}
	
	Body* World::getBodyContaining( const Vector2& pt )
	{
		for (unsigned int i = 0; i < mBodies.size(); i++)
		{
			if (mBodies[i]->contains(pt))
				return mBodies[i];
		}
		
		return 0;
	}
	
	void World::update( float elapsed )
	{
		mPenetrationCount = 0;
		
		// first, accumulate all forces acting on PointMasses.
		for (BodyList::iterator it = mBodies.begin(); it != mBodies.end(); it++)
		{
			if ((*it)->getIsStatic() || (*it)->getIgnoreMe()) { continue; }
			
			(*it)->derivePositionAndAngle(elapsed);
			(*it)->accumulateExternalForces();
			(*it)->accumulateInternalForces();
		}
		
		// now integrate.
		for (BodyList::iterator it = mBodies.begin(); it != mBodies.end(); it++)
		{
			//if ((*it)->getIsStatic()) { continue; }
			
			(*it)->integrate(elapsed);
		}
		
		// update all bounding boxes, and then bitmasks.
		for (BodyList::iterator it = mBodies.begin(); it != mBodies.end(); it++)
		{
			if ((*it)->getIsStatic() || (*it)->getIgnoreMe()) { continue; }
			
			(*it)->updateAABB(elapsed);
			updateBodyBitmask((*it));
			(*it)->updateEdgeInfo();
			(*it)->updateBoundaryValues();
		}
		
		// sort body boundaries for broadphase collision checks
		sortBodyBoundaries();
		
		// now check for collision.
		for (unsigned int i = 0; i < mBodies.size(); i++)
		{
			Body* bA = mBodies[i];
			if (bA->getIsStatic() || bA->getIgnoreMe())
				continue;
			
			/*
			 // OLD, BRUTE-FORCE COLLISION CHECKS USING BITMASKS ONLY FOR OPTIMIZATION.
			for (int j = i + 1; j < mBodies.size(); j++)
			{
				_goNarrowCheck( mBodies[i], mBodies[j] );
			}
			*/
			
			Body::BodyBoundary* bS = &bA->mBoundStart;
			Body::BodyBoundary* bE = &bA->mBoundEnd;
			Body::BodyBoundary* cur = bS->next;
			
			bool passedMyEnd = false;
			while (cur)
			{
				if (cur == bE)
				{
					passedMyEnd = true;
				}
				else if ((cur->type == Body::BodyBoundary::Begin) && (!passedMyEnd))
				{
					// overlapping, do narrow-phase check on this body pair.
					_goNarrowCheck(bA, cur->body);
				}
				else if (cur->type == Body::BodyBoundary::End)
				{
					// this is an end... the only situation in which we didn't already catch this body from its "begin",
					// is if the begin of this body starts before our own begin.
					if (cur->body->mBoundStart.value <= bS->value)
					{
						// overlapping, do narrow-phase check on this body pair.
						_goNarrowCheck(bA, cur->body);
					}
				}
				else if (cur->type == Body::BodyBoundary::VoidMarker)
				{
					break;
				}
				
				cur = cur->next;
			}
		}
		
		//printf("\n\n");
		
		// now handle all collisions found during the update at once.
		_handleCollisions();
		
		// now dampen velocities.
		for (BodyList::iterator it = mBodies.begin(); it != mBodies.end(); it++)
		{
			//if ((*it)->getIsStatic()) { continue; }
			(*it)->dampenVelocity();
		}
	}
	
	
	void World::_goNarrowCheck( Body* bI, Body* bJ )
	{
		//printf("goNarrow %d vs. %d\n", bI, bJ);
		
		// grid-based early out.
		if ( /*((bI->mBitMaskX.mask & bJ->mBitMaskX.mask) == 0) && */
			((bI->mBitMaskY.mask &bJ->mBitMaskY.mask) == 0))
		{
			//printf("update - no bitmask overlap.\n");
			return;
		}
		
		// early out - these bodies materials are set NOT to collide
		if (!mMaterialPairs[(bI->getMaterial() * mMaterialCount) + bJ->getMaterial()].Collide)
		{
			//printf("update - material early out: %d vs. %d\n", mBodies[i]->getMaterial(), mBodies[j]->getMaterial());
			return;
		}
		
		// broad-phase collision via AABB.
		const AABB& boxA = bI->getAABB();
		const AABB& boxB = bJ->getAABB();
		
		// early out
		if (!boxA.intersects(boxB))
		{
			//printf("update - no AABB overlap.\n");
			return;
		}
		
		// okay, the AABB's of these 2 are intersecting.  now check for collision of A against B.
		bodyCollide(bI, bJ, mCollisionList);
		
		// and the opposite case, B colliding with A
		bodyCollide(bJ, bI, mCollisionList);
	}
	
	void World::updateBodyBitmask( Body* body )
	{
		AABB box = body->getAABB();
		
		/*int minX = (int)floor((box.Min.X - mWorldLimits.Min.X) / mWorldGridStep.X);
		int maxX = (int)floor((box.Max.X - mWorldLimits.Min.X) / mWorldGridStep.X);
		
		if (minX < 0) { minX = 0; } else if (minX > 31) { minX = 31; }
		if (maxX < 0) { maxX = 0; } else if (maxX > 31) { maxX = 31; }
		*/
		
		int minY = (int)floor((box.Min.Y - mWorldLimits.Min.Y) / mWorldGridStep.Y);
		int maxY = (int)floor((box.Max.Y - mWorldLimits.Min.Y) / mWorldGridStep.Y);
		
		if (minY < 0) { minY = 0; } else if (minY > 31) { minY = 31; }
		if (maxY < 0) { maxY = 0; } else if (maxY > 31) { maxY = 31; }
		
		/*
		body->mBitMaskX.clear();
		for (int i = minX; i <= maxX; i++)
			body->mBitMaskX.setOn(i);
		*/
		
		body->mBitMaskY.clear();
		for (int i = minY; i <= maxY; i++)
			body->mBitMaskY.setOn(i);
		
		//Console.WriteLine("Body bitmask: minX{0} maxX{1} minY{2} maxY{3}", minX, maxX, minY, minY, maxY);
	}
	
	
	void World::sortBodyBoundaries()
	{
		//--------------------------------------------------------
		// for every body in the list, update it's START end END.
		for (BodyList::iterator it = mBodies.begin(); it != mBodies.end(); it++)
		{
			// start with START boundary.
			_checkAndMoveBoundary( &(*it)->mBoundStart );
			
			// and then END boundary.
			_checkAndMoveBoundary( &(*it)->mBoundEnd );
		}
		
		//--------------------------------------------------------
		// now go through and add / remove the "VOID" identifiers.
		if (mBodies.size() > 0)
		{
			Body::BodyBoundary* bb = &mBodies[0]->mBoundStart;
			
			while (bb->prev)
				bb = bb->prev;
			
			int stackCount = 0;
			while (bb)
			{
				//printf("bb: ");
				//bb->log();
				
				
				if (bb->type == Body::BodyBoundary::Begin)
				{
					stackCount++;
					//printf(" begin, stack inced to %d.", stackCount);
				}
				else if (bb->type == Body::BodyBoundary::End)
				{
					stackCount--;
					//printf(" end, stack deced to %d.", stackCount);
				}
				else if (bb->type == Body::BodyBoundary::VoidMarker)
				{
					if (stackCount != 0)
					{
						// this void marker should not be here.
						Body::BodyBoundary* v = bb;
						bb = bb->next;
						_removeBoundary(v);
						delete v;
						
						//printf(" VOID but stack != 0. deleted.\n");
						continue;
					}
					else
					{
						// OK as-is
						//printf(" VOID but stack is 0. continuing.\n");
						bb = bb->next;
						continue;
					}
				}
				
				//printf("  |  ");
				
				if (stackCount == 0)
				{
					if (bb->next)
					{
						if (bb->next->type == Body::BodyBoundary::VoidMarker)
						{
							bb = bb->next->next;
							//printf("next is VOID, so OK. skipping VOID...\n");
							continue;
						}
					}
					else
					{
						// no next, end of the array!
						//printf("end of the array!\n");
						break;
					}
					
					// add a new void marker here!
					Body::BodyBoundary* v = new Body::BodyBoundary( 0, Body::BodyBoundary::VoidMarker, bb->value + 0.000001f );
					_addBoundaryAfter(v, bb);
					
					bb = v->next;
					//printf("stack is 0, adding VOID!\n");
					continue;
				}
				
				//printf("\n");
				
				bb = bb->next;
			}
		}
		
		//printf("sortBodyBoundaries...\n");
		//_logBoundaries();
	}
	
	void World::_checkAndMoveBoundary( Body::BodyBoundary* bb )
	{
		//printf("checkAndMoveBoundary:");
		//bb->log();
		//printf("\n");
		
		// need to move backwards?
		if (bb->prev)
		{
			if (bb->value < bb->prev->value)
			{
				Body::BodyBoundary* beforeThis = bb->prev;
				while (true)
				{					
					if (!beforeThis->prev) 
					{ 
						break; 
					}
					else
					{
						if (bb->value < beforeThis->prev->value)
							beforeThis = beforeThis->prev;
						else
							break;
					}
				}
				
				//printf("moving left: ");
				//bb->log();
				//printf(" to before:");
				//beforeThis->log();
				//printf("\n");
				
				_removeBoundary(bb);
				_addBoundaryBefore(bb, beforeThis);
				// moved, so we're done!
				return;
			}
		}
		
		if (bb->next)
		{
			if (bb->value > bb->next->value)
			{
				Body::BodyBoundary* afterThis = bb->next;
				while (true)
				{
					if (!afterThis->next)
					{
						break;
					}
					else
					{
						if (bb->value > afterThis->next->value)
							afterThis = afterThis->next;
						else
							break;
					}
				}
				
				//printf("moving right: ");
				//bb->log();
				//printf(" to after:");
				//afterThis->log();
				//printf("\n");
				
				_removeBoundary(bb);
				_addBoundaryAfter(bb, afterThis);
				
				return;
			}
		}
	}
				
				
	
	void World::bodyCollide( Body* bA, Body* bB, std::vector<BodyCollisionInfo>& infoList )
	{
		int bApmCount = bA->getPointMassCount();
		int bBpmCount = bB->getPointMassCount();
		
		AABB boxB = bB->getAABB();
		
		// check all PointMasses on bodyA for collision against bodyB.  if there is a collision, return detailed info.
		BodyCollisionInfo infoAway;
		BodyCollisionInfo infoSame;
		for (int i = 0; i < bApmCount; i++)
		{
			Vector2 pt = bA->getPointMass(i)->Position;
			
			// early out - if this point is outside the bounding box for bodyB, skip it!
			if (!boxB.contains(pt))
			{
				//printf("bodyCollide - bodyB AABB does not contain pt\n");
				continue;
			}
			
			// early out - if this point is not inside bodyB, skip it!
			if (!bB->contains(pt))
			{
				//printf("bodyCollide - bodyB does not contain pt\n");
				continue;
			}
			
			int prevPt = (i>0) ? i-1 : bApmCount-1;
			int nextPt = (i < bApmCount - 1) ? i + 1 : 0;
			
			Vector2 prev = bA->getPointMass(prevPt)->Position;
			Vector2 next = bA->getPointMass(nextPt)->Position;
			
			// now get the normal for this point. (NOT A UNIT VECTOR)
			Vector2 fromPrev = pt - prev;
			Vector2 toNext = next - pt;
			
			Vector2 ptNorm = fromPrev + toNext;
			ptNorm.makePerpendicular();
			
			// this point is inside the other body.  now check if the edges on either side intersect with and edges on bodyB.          
			float closestAway = 100000.0f;
			float closestSame = 100000.0f;
			
			infoAway.Clear();
			infoAway.bodyA = bA;
			infoAway.bodyApm = i;
			infoAway.bodyB = bB;
			
			infoSame.Clear();
			infoSame.bodyA = bA;
			infoSame.bodyApm = i;
			infoSame.bodyB = bB;
			
			bool found = false;
			
			int b1 = 0;
			int b2 = 1;
			for (int j = 0; j < bBpmCount; j++)
			{
				Vector2 hitPt;
				Vector2 norm;
				float edgeD;
				
				b1 = j;
				
				if (j < bBpmCount - 1)
					b2 = j + 1;
				else
					b2 = 0;
				
				//Vector2 pt1 = bB->getPointMass(b1)->Position;
				//Vector2 pt2 = bB->getPointMass(b2)->Position;
				
				// quick test of distance to each point on the edge, if both are greater than current mins, we can skip!
				//float distToA = (pt1 - pt).lengthSquared();
				//float distToB = (pt2 - pt).lengthSquared();				
				
				/*if ((distToA > closestAway) && (distToA > closestSame) && (distToB > closestAway) && (distToB > closestSame))
				{
					//printf("bodyCollide - not close enough\n");
					continue;
				}*/
				
				// test against this edge.
				float dist = bB->getClosestPointOnEdgeSquared(pt, j, hitPt, norm, edgeD);
				//printf("bodyCollide - dist:%f\n", dist);
				
				
				// only perform the check if the normal for this edge is facing AWAY from the point normal.
				float dot = ptNorm.dotProduct(norm);
				if (dot <= 0.0f)
				{
					if (dist < closestAway)
					{
						closestAway = dist;
						infoAway.bodyBpmA = b1;
						infoAway.bodyBpmB = b2;
						infoAway.edgeD = edgeD;
						infoAway.hitPt = hitPt;
						infoAway.norm = norm;
						infoAway.penetration = dist;
						found = true;
						
						//printf("bodyCollide - set away.\n");
						infoAway.Log();
					}
				}
				else
				{
					if (dist < closestSame)
					{
						closestSame = dist;
						infoSame.bodyBpmA = b1;
						infoSame.bodyBpmB = b2;
						infoSame.edgeD = edgeD;
						infoSame.hitPt = hitPt;
						infoSame.norm = norm;
						infoSame.penetration = dist;
						
						//printf("bodyCollide - set same\n");
						infoSame.Log();
					}
				}
			}
			
			// we've checked all edges on BodyB.  add the collision info to the stack.
			if ((found) && (closestAway > mPenetrationThreshold) && (closestSame < closestAway))
			{
				infoSame.penetration = (float)sqrt(infoSame.penetration);
				infoList.push_back(infoSame);
				//printf("bodyCollide - added same: penetration:%f\n", infoSame.penetration);
			}
			else
			{
				infoAway.penetration = (float)sqrt(infoAway.penetration);
				infoList.push_back(infoAway);
				//printf("bodyCollide - added away: penetration:%f\n", infoAway.penetration);
			}
		}
		
	}
	
	void World::_handleCollisions()
	{
		//printf("handleCollisions - count %d\n", mCollisionList.size());
		
		// handle all collisions!
		for (unsigned int i = 0; i < mCollisionList.size(); i++)
		{
			BodyCollisionInfo info = mCollisionList[i];
			
			PointMass* A = info.bodyA->getPointMass(info.bodyApm);
			PointMass* B1 = info.bodyB->getPointMass(info.bodyBpmA);
			PointMass* B2 = info.bodyB->getPointMass(info.bodyBpmB);
			
			// velocity changes as a result of collision.
			Vector2 bVel = (B1->Velocity + B2->Velocity) * 0.5f;
			Vector2 relVel = A->Velocity - bVel;
			
			float relDot = relVel.dotProduct(info.norm);
			
			//printf("handleCollisions - relVel:[x:%f][y:%f] relDot:%f\n",
			//	   relVel.X, relVel.Y, relDot);
			
			// collision filter!
			//if (!mMaterialPairs[info.bodyA.Material, info.bodyB.Material].CollisionFilter(info.bodyA, info.bodyApm, info.bodyB, info.bodyBpmA, info.bodyBpmB, info.hitPt, relDot))
			//	continue;
			CollisionCallback* cf = mMaterialPairs[(info.bodyA->getMaterial() * mMaterialCount) + info.bodyB->getMaterial()].Callback;
			if (cf)
			{
				if (!cf->collisionFilter(info.bodyA, info.bodyApm, info.bodyB, info.bodyBpmA, info.bodyBpmB, info.hitPt, relDot))
					continue;
			}
			
			if (info.penetration > mPenetrationThreshold)
			{
				//Console.WriteLine("penetration above Penetration Threshold!!  penetration={0}  threshold={1} difference={2}",
				//    info.penetration, mPenetrationThreshold, info.penetration-mPenetrationThreshold);
				//printf("handleCollisions - penetration above threshold! threshold:%f penetration:%f diff:%f\n",
				//	   mPenetrationThreshold, info.penetration, info.penetration - mPenetrationThreshold);
				
				mPenetrationCount++;
				continue;
			}
			
			float b1inf = 1.0f - info.edgeD;
			float b2inf = info.edgeD;
			
			float b2MassSum = ((B1->Mass == 0.0f) || (B2->Mass == 0.0f)) ? 0.0f : (B1->Mass + B2->Mass);
			
			float massSum = A->Mass + b2MassSum;
			
			float Amove;
			float Bmove;
			if (A->Mass == 0.0f)
			{
				Amove = 0.0f;
				Bmove = (info.penetration) + 0.001f;
			}
			else if (b2MassSum == 0.0f)
			{
				Amove = (info.penetration) + 0.001f;
				Bmove = 0.0f;
			}
			else
			{
				Amove = (info.penetration * (b2MassSum / massSum));
				Bmove = (info.penetration * (A->Mass / massSum));
			}
			
			float B1move = Bmove * b1inf;
			float B2move = Bmove * b2inf;
			
			//printf("handleCollisions - Amove:%f B1move:%f B2move:%f\n",
			//	   Amove, B1move, B2move)
			if (A->Mass != 0.0f)
			{
				A->Position += info.norm * Amove;
			}
			
			if (B1->Mass != 0.0f)
			{
				B1->Position -= info.norm * B1move;
			}
			
			if (B2->Mass != 0.0f)
			{
				B2->Position -= info.norm * B2move;
			}
			
			float AinvMass = (A->Mass == 0.0f) ? 0.0f : 1.0f / A->Mass;
			float BinvMass = (b2MassSum == 0.0f) ? 0.0f : 1.0f / b2MassSum;
			
			float jDenom = AinvMass + BinvMass;
			float elas = 1.0f + mMaterialPairs[(info.bodyA->getMaterial() * mMaterialCount) + info.bodyB->getMaterial()].Elasticity;
			Vector2 numV = relVel * elas;
			
			float jNumerator = numV.dotProduct(info.norm);
			jNumerator = -jNumerator;
			
			float j = jNumerator / jDenom;
			
			
			Vector2 tangent = info.norm.getPerpendicular();
			float friction = mMaterialPairs[(info.bodyA->getMaterial() * mMaterialCount) + info.bodyB->getMaterial()].Friction;
			float fNumerator = relVel.dotProduct(tangent);
			fNumerator *= friction;
			float f = fNumerator / jDenom;
			
			// adjust velocity if relative velocity is moving toward each other.
			if (relDot <= 0.0001f)
			{
				if (A->Mass != 0.0f)
				{
					A->Velocity += (info.norm * (j / A->Mass)) - (tangent * (f / A->Mass));
				}
				
				if (b2MassSum != 0.0f)
				{
					B1->Velocity -= (info.norm * (j / b2MassSum) * b1inf) - (tangent * (f / b2MassSum) * b1inf);
				}
				
				if (b2MassSum != 0.0f)
				{
					B2->Velocity -= (info.norm * (j / b2MassSum) * b2inf) - (tangent * (f / b2MassSum) * b2inf);
				}
			}
		}
		
		mCollisionList.clear();
	}

	
	void World::_removeBoundary( Body::BodyBoundary* me )
	{		
		if (me->prev)
			me->prev->next = me->next;
		
		if (me->next)
			me->next->prev = me->prev;
	}
	
	void World::_addBoundaryAfter( Body::BodyBoundary* me, Body::BodyBoundary* toAfterMe )
	{
		me->next = toAfterMe->next;
		toAfterMe->next = me;
		
		if (me->next)
			me->next->prev = me;
		me->prev = toAfterMe;
	}
	
	void World::_addBoundaryBefore( Body::BodyBoundary* me, Body::BodyBoundary* toBeforeMe )
	{
		me->prev = toBeforeMe->prev;
		toBeforeMe->prev = me;
		
		if (me->prev)
			me->prev->next = me;
		me->next = toBeforeMe;
	}
	
	
	void World::_logBoundaries()
	{
		// first, find the "first" boundary in the list...
		if (mBodies.size() == 0)
			return;
		
		Body::BodyBoundary* bb = &mBodies[0]->mBoundStart;
		while (bb->prev)
			bb = bb->prev;
		
		while (bb)
		{
			bb->log();
			bb = bb->next;
		}
		
		//printf("\n\n");
	}
	
	void World::_logMaterialCollide()
	{
		
	}
	
}

