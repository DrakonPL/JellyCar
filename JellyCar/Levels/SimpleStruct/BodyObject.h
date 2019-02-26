#ifndef BodyObject_H
#define BodyObject_H

#include "BodyPoint.h"
#include "BodySpring.h"
#include "BodyPolygon.h"
#include "BodyObjectInfo.h"

struct BodyObject
{
	BodyObjectInfo info;

	int points;
	BodyPoint* bodyPoints;

	int springs;
	BodySpring* bodySprings;

	int polygons;
	BodyPolygon* bodyPolygons;
};

#endif
