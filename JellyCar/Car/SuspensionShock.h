#ifndef SuspensionShock_H
#define SuspensionShock_H

#include <Andromeda/Graphics/RenderManager.h>
using namespace Andromeda::Graphics;

#include "JellyPhysics/JellyPhysics.h"

class Tire;

class SuspensionShock
{
public:

	int		mChassisPM;
	Tire*	mTire;
	float	mD;
	float	mK;
	float	mDamp;

	float	mK1;
	float	mK2;

	float	mDamp1;
	float	mDamp2;

	float	mD1;
	float	mD2;

	SuspensionShock(int chassisPM, Tire* goalTire, float goalD1, float goalD2, float K1, float K2, float damp1, float damp2);

	void interpolateD(float x);
};

#endif 