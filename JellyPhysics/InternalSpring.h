#ifndef _INTERNAL_SPRING_H
#define _INTERNAL_SPRING_H

#include "Vector2.h"


namespace JellyPhysics 
{
	class InternalSpring
	{
	public:
		InternalSpring() : pointMassA(0), pointMassB(0), springD(0.0f), springK(0.0f), damping(0.0f) { }
		InternalSpring(int pmA, int pmB, float d, float k, float damp) : pointMassA(pmA), pointMassB(pmB), springD(d), springK(k), damping(damp) { }
		
		int		pointMassA;
		int		pointMassB;
		float	springD;
		float	springK;
		float	damping;
	};
}

#endif	// _INTERNAL_SPRING_H

