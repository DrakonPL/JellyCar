#ifndef KinematicMotor_H
#define KinematicMotor_H

#include "KinematicControl.h"

class KinematicMotor : public KinematicControl
{
protected:

	float _speed;

public:

	KinematicMotor(Body* body, float radPerSec);

	void Update(float elapsed);
	float GetPeriod();

};


#endif