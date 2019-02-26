#ifndef Tire_H
#define Tire_H

#include <Andromeda/Graphics/RenderManager.h>
using namespace Andromeda::Graphics;

#include "JellyPhysics/JellyPhysics.h"
using namespace JellyPhysics;

class Tire : public PressureBody
{
private:

	VertexArrayObject* _vertexObject;
	VertexArrayObject* _texturedObject;
	glm::vec4 _color;
	bool _created;

	bool _useNearestGravity;

public:

	//private:
	bool	smallBig;

	float	p1;
	float	p2;

	float	ek1;
	float	ek2;

	float	ed1;
	float	ed2;

	float	torque1;
	float	torque2;

	float	torque;

	float	maxOmega1;
	float	maxOmega2;
	float	maxOmega;

	float	torqueForce;
	Vector2	shockForce;

	std::vector<Vector2>	mTetxure;
	std::vector<Vector2>	shape;

	float	drive;

	bool ballon;

	Tire(World *w, const ClosedShape& s,
		float d, float mpp, float pressure1, float pressure2,
		float shapeK, float shapeDamp, float edgeK1, float edgeDamp1,
		float edgeK2, float edgeDamp2, float t1, float t2,
		float maxO1, float maxO2, const Vector2& pos);

	~Tire();

	void SetTexturePoints();

	void SetTorque(float t);
	void SetShockForce(const Vector2& f);
	void AddShockForce(const Vector2& f);

	void InterpolatePressure(float x);

	void accumulateExternalForces();

	float GetDrive();
	float GetMaxOmega();

	void UseNearestGracity(bool state);

	void Draw(glm::mat4 &proj, Texture* texture);
};

#endif 