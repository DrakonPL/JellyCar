#ifndef KinematicPlatform_H
#define KinematicPlatform_H

#include "KinematicControl.h"

class KinematicPlatform : public KinematicControl
{
protected:

	Vector2		_start;
	Vector2		_end;
	float		_factor;
	float		_i;

public:

	KinematicPlatform(Body* body, const Vector2& start, const Vector2& end, float secondsPerLoop, float startOffset);

	void Update(float elapsed);
	float GetPeriod();

};


#endif