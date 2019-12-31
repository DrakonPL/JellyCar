#include "SuspensionShock.h"
#include "Tire.h"

SuspensionShock::SuspensionShock(int chassisPM, Tire* goalTire, float goalD1, float goalD2, float K1, float K2, float damp1, float damp2)
{
	mChassisPM = chassisPM;
	mTire = goalTire;
	mD = goalD1;
	mK = K1;
	mDamp = damp1;

	mD1 = goalD1;
	mD2 = goalD2;

	mK1 = K1;
	mK2 = K2;

	mDamp1 = damp1;
	mDamp2 = damp2;
}

void SuspensionShock::interpolateD(float x)
{
	mD = mD1 + ((mD2 - mD1) * x);
	mK = mK1 + ((mK2 - mK1) * x);
	mDamp = mDamp1 + ((mDamp2 - mDamp1) * x);
}