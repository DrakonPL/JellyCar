#include "KinematicMotor.h"

KinematicMotor::KinematicMotor(Body* body, float radPerSec) : KinematicControl(body)
{
	_speed = radPerSec;
}

void KinematicMotor::Update(float elapsed)
{
	_body->setKinematicAngle(_body->getDerivedAngle() + (_speed * elapsed));
}

float KinematicMotor::GetPeriod()
{
	return (TWO_PI / _speed);
}
