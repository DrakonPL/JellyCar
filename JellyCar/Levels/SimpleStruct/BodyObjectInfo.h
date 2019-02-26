#ifndef BodyObjectInfo_H
#define BodyObjectInfo_H

struct BodyObjectInfo
{
	char name[64];

	float colorR;
	float colorG;
	float colorB;

	float massPerPoint;
	float edgeK;
	float edgeDamping;

	bool isKinematic;

	bool shapeMatching;//= true;
	float shapeK;//= 100.0f;
	float shapeDamping;//= 10.0f;
	bool pressureized;//= false;
	float pressure;//= 0.0f;
};

#endif