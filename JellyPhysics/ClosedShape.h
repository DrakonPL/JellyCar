#ifndef _CLOSED_SHAPE_H
#define _CLOSED_SHAPE_H

#include "JellyPrerequisites.h"
#include "Vector2.h"

#include <vector>

namespace JellyPhysics {
	
	class ClosedShape
	{
	private:
		Vector2List	mLocalVertices;
		
	public:
		ClosedShape() {}
		ClosedShape( Vector2List input ):mLocalVertices(input) {finish(); }
		
		void begin();
		int addVertex( const Vector2& vec );
		void finish( bool recenter = true );
		
		Vector2List& getVertices() { return mLocalVertices; }
		
		Vector2List transformVertices(const Vector2& worldPos, float angleInRadians, const Vector2& scale);
		void transformVertices(const Vector2& worldPos, float angleInRadians, const Vector2& scale, Vector2List& outList);
		
		
	};
	
}	// end namespace JellyPhysics

#endif // _CLOSED_SHAPE_H
