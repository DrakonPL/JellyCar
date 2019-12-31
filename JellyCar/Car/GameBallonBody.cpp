#include "GameBallonBody.h"
#include "Chassis.h"

#include "../Utils/JellyHelper.h"

GameBallonBody::GameBallonBody(World* w, const ClosedShape& shape, float massPerPoint,
	float shapeSpringK, float shapeSpringDamp,
	float edgeSpringK, float edgeSpringDamp,
	const Vector2& pos, float angleinRadians, const Vector2& scale,
	bool kinematic) :
	SpringBody(w, shape, massPerPoint, shapeSpringK, shapeSpringDamp, edgeSpringK, edgeSpringDamp, pos, angleinRadians, scale, kinematic)
{
	ballonActive = false;

	_vertexObject = RenderManager::Instance()->CreateVertexArrayObject(Simple, DynamicDraw);
	_vertexObject->SetVertexPrimitive(Lines);

	_shapeObject = RenderManager::Instance()->CreateVertexArrayObject(Simple, DynamicDraw);

	_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	torque = 0.0f;

	_created = false;
}

GameBallonBody::~GameBallonBody()
{
	delete _vertexObject;
	delete _shapeObject;

	delete[] mIndices;
	mIndexList.clear();
}

void GameBallonBody::accumulateExternalForces()
{
	SpringBody::accumulateExternalForces();

	for (unsigned int i = 0; i < mPointMasses.size(); i++)
		mPointMasses[i].Force += Vector2(6.4f * torque, 9.8f * mPointMasses[i].Mass);
}

void GameBallonBody::SetBallonBody(Chassis* body)
{
	gameBallonBody = body;
}

void GameBallonBody::SetBallonState(bool state)
{
	ballonActive = state;
}

void GameBallonBody::SetTorque(float t)
{
	torque = t;
}

void GameBallonBody::SetBallonPosition(Vector2 pos, Vector2 velocity)
{
	ballonPosition = pos;
	ballonVelocity = velocity;

	//ballon 
	if (ballonActive)
	{
		//update to ballon
		Vector2 pmp = mPointMasses[11].Position;
		Vector2 pmv = mPointMasses[11].Velocity;
		Vector2 tp = ballonPosition;
		Vector2 tv = ballonVelocity;

		Vector2 force = VectorTools::calculateSpringForce(pmp, pmv, tp, tv, 0, 2000, 50);
		mPointMasses[11].Force += force;

		force = -force;

		gameBallonBody->AddShockForce(force);
	}
}

void GameBallonBody::AddTriangle(int a, int b, int c)
{
	mIndexList.push_back(a);
	mIndexList.push_back(b);
	mIndexList.push_back(c);
}

void GameBallonBody::FinalizeTriangles()
{
	mIndicesCount = mIndexList.size();
	mIndices = new int[mIndexList.size()];
	for (unsigned int i = 0; i < mIndexList.size(); i++)
		mIndices[i] = mIndexList[i];
}

void GameBallonBody::Draw(glm::mat4 &proj)
{
	if (!_created)
	{
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, true);
		JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, true);
		_created = true;
	}
	else if (!mIsStatic)
	{
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, false);
		JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, false);
	}

	_color = glm::vec4(1.0f, 0.65f, 0.0f, 0.7f);
	JellyHellper::Instance()->DrawShape(_shapeObject, proj, _color);

	_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	JellyHellper::Instance()->DrawLines(_vertexObject, proj, _color);
}