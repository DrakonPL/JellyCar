#include "ClosedShape.h"
#include "VectorTools.h"

namespace JellyPhysics 
{
	
	void ClosedShape::begin()
	{
		mLocalVertices.clear();
	}
	
	int ClosedShape::addVertex( const Vector2& vec )
	{
		mLocalVertices.push_back( vec );
		return mLocalVertices.size();
	}
	
	void ClosedShape::finish(bool recenter)
	{
		if (recenter)
		{
			// find the average location of all of the vertices, this is our geometrical center.
			Vector2 center = Vector2::Zero;
			
			for (unsigned int i = 0; i < mLocalVertices.size(); i++)
				center += mLocalVertices[i];
			
			center /= (float)mLocalVertices.size();
			
			// now subtract this from each element, to get proper "local" coordinates.
			for (unsigned int i = 0; i < mLocalVertices.size(); i++)
				mLocalVertices[i] -= center;
		}
	}
	
	Vector2List ClosedShape::transformVertices(const Vector2& worldPos, float angleInRadians, const Vector2& scale)
	{
		Vector2List ret = mLocalVertices;
		
		Vector2 v;
		for (unsigned int i = 0; i < ret.size(); i++)
		{
			// rotate the point, and then translate.
			v = ret[i] * scale;

			v = JellyPhysics::VectorTools::rotateVector(v, angleInRadians);
			
			v += worldPos;

			ret[i] = v;
		}
		
		return ret;
	}
	
	
	
	void ClosedShape::transformVertices(const Vector2& worldPos, float angleInRadians, const Vector2& scale, Vector2List& outList)
	{
		float c = cos(angleInRadians);
		float s = sin(angleInRadians);
		
		Vector2List::iterator out = outList.begin();
		
		for (Vector2List::iterator it = mLocalVertices.begin(); it != mLocalVertices.end(); it++)
		{
			// rotate the point, and then translate.
			Vector2 v =(*it) * scale;

			v = JellyPhysics::VectorTools::rotateVector(v, c, s);
			v += worldPos;

			(*out) = v;
			
			out++;
		}
	}
	
	
}
