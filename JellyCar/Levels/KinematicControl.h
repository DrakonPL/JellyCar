#ifndef KinematicControl_H
#define KinematicControl_H

#include "JellyPhysics/JellyPhysics.h"
using namespace JellyPhysics;

class KinematicControl
{
protected:

	Body* _body;

public:

	KinematicControl(Body* body);
	virtual ~KinematicControl();

	virtual void Update(float elapsed);
	virtual float GetPeriod();

};


#endif