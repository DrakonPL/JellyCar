#include "KinematicPlatform.h"

KinematicPlatform::KinematicPlatform(Body* body, const Vector2& start, const Vector2& end, float secondsPerLoop, float startOffset) : KinematicControl(body)
{
	_start = start;
	_end = end;
	_factor = TWO_PI / secondsPerLoop;
	_i = (HALF_PI)+(TWO_PI * startOffset);
}

void KinematicPlatform::Update(float elapsed)
{
	_i += elapsed * _factor;
	if (_i > (TWO_PI)) { _i -= (TWO_PI); }

	Vector2 newPos = _start.lerp(_end, 0.5f + (sinf(_i)*0.5f));
	_body->setKinematicPosition(newPos);
}

float KinematicPlatform::GetPeriod()
{ 
	return (TWO_PI / _factor);
}