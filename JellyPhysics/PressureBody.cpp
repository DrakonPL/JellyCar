#include "PressureBody.h"

namespace JellyPhysics 
{
	PressureBody::~PressureBody()
	{
		delete[] mNormalList;
		//delete[] mEdgeLengthList;
	}
	
	void PressureBody::accumulateInternalForces()
	{

		SpringBody::accumulateInternalForces();
		
		// internal forces based on pressure equations.  we need 2 loops to do this.  one to find the overall volume of the
		// body, and 1 to apply forces.  we will need the normals for the edges in both loops, so we will cache them and remember them.
		mVolume = 0.0f;
		
		for (int i = 0; i < mPointCount; i++)
		{
			int prev = (i > 0) ? i-1 : mPointCount-1;
			int next = (i < mPointCount - 1) ? i + 1 : 0;
			
			PointMass& pmP = mPointMasses[prev];
			PointMass& pmI = mPointMasses[i];
			PointMass& pmN = mPointMasses[next];
			
			// currently we are talking about the edge from i --> j.
			// first calculate the volume of the body, and cache normals as we go.
			Vector2 edge1N = pmI.Position - pmP.Position;
			edge1N.makePerpendicular();
			
			Vector2 edge2N = pmN.Position - pmI.Position;
			edge2N.makePerpendicular();
			
			Vector2 norm = edge1N + edge2N;
			
			norm.normalise();			
			float edgeL = mEdgeInfo[i].length; //edge2N.length();
			
			// cache normal and edge length
			mNormalList[i] = norm;
			//mEdgeLengthList[i] = edgeL;
			
			float xdist = absf(pmI.Position.X - pmN.Position.X);
			float normX = absf(norm.X);
			
			float volumeProduct = xdist * normX * edgeL;
			
			// add to volume
			mVolume += 0.5f * volumeProduct;
		}
		
		// now loop through, adding forces!
		float invVolume = 1.0f / mVolume;
		
		for (int i = 0; i < mPointCount; i++)
		{
			int j = (i < mPointCount - 1) ? i + 1 : 0;
			
			float pressureV = (invVolume * mEdgeInfo[i].length * mGasAmount);
			mPointMasses[i].Force += mNormalList[i] * pressureV;
			mPointMasses[j].Force += mNormalList[j] * pressureV;
		}
	}
}
