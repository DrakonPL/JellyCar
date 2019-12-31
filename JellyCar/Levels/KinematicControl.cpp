#include "KinematicControl.h"

KinematicControl::KinematicControl(Body* body)
{
	_body = body;
}

KinematicControl::~KinematicControl()
{

}

void KinematicControl::Update(float elapsed)
{

}


float KinematicControl::GetPeriod()
{
	return 1.0f;
}