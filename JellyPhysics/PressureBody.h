#ifndef _PRESSURE_BODY_H
#define _PRESSURE_BODY_H

#include "SpringBody.h"

namespace JellyPhysics 
{
	class PressureBody : public SpringBody
	{
	protected:
		float		mVolume;
		float		mGasAmount;
		Vector2*	mNormalList;
		
	public:
		PressureBody(World* w, const ClosedShape& s, float mpp,
					 float gasPressure, float shapeK, float shapeD,
					 float edgeK, float edgeD,
					 const Vector2& pos, float angleInRadians, const Vector2& scale,
					 bool kinematic) :
			SpringBody(w, s, mpp, shapeK, shapeD, edgeK, edgeD, pos, angleInRadians, scale, kinematic)
		{
			mGasAmount = gasPressure;
			mNormalList = new Vector2[mPointCount];
			mVolume = 0.0f;
		}
		
		~PressureBody();
		
		void setGasPressure( float val ) { mGasAmount = val; }
		float getGasPressure() { return mGasAmount; }
		
		float getVolume() { return mVolume; }
		
		void accumulateInternalForces();
		
	};
}


#endif	// _PRESSURE_BODY_H