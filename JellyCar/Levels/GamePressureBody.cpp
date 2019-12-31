#include "GamePressureBody.h"
#include "../Utils/JellyHelper.h"

GamePressureBody::GamePressureBody(World* w, const ClosedShape& s, float mpp, float gasPressure, float shapeK, float shapeD,
	float edgeK, float edgeD, const Vector2& pos, float angleInRadians, const Vector2& scale, bool kinematic) :
	PressureBody(w, s, mpp, gasPressure, shapeK, shapeD, edgeK, edgeD, pos, angleInRadians, scale, kinematic)
{
	dragPoint = -1;

	//lines
	_vertexObject = RenderManager::Instance()->CreateVertexArrayObject(Simple, DynamicDraw);
	_vertexObject->SetVertexPrimitive(Lines);

	_shapeObject = 0;

	_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	_lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	_created = false;

	shape.resize(mPointMasses.size() + 2);

	_texture = 0;
}

GamePressureBody::~GamePressureBody()
{
	delete _vertexObject;
	delete _shapeObject;
}

void GamePressureBody::accumulateInternalForces()
{
	if (!mIsStatic)
	{
		PressureBody::accumulateInternalForces();
	}
}

void GamePressureBody::accumulateExternalForces()
{
	if (!mIsStatic)
	{
		PressureBody::accumulateExternalForces();

		for (unsigned int i = 0; i < mPointMasses.size(); i++)
			mPointMasses[i].Force += Vector2(0.0f, -9.8f * mPointMasses[i].Mass);

		if (dragPoint != -1)
			mPointMasses[dragPoint].Force += dragForce;

		dragPoint = -1;
	}
}

void GamePressureBody::SetTexture(Texture* texture)
{
	_texture = texture;
}

void GamePressureBody::SetLineColor(glm::vec4 color)
{
	_lineColor = color;
}

void GamePressureBody::Draw(glm::mat4 &proj, int *mIndices, int mIndicesCount, float  R, float  G, float B)
{
	if (!_created)
	{
		if (_texture != 0)
		{
			//generate texture positions
			_textPositions = JellyHellper::Instance()->GetTexturePositions(getAABB(), mPointMasses);

			//texture fill
			_shapeObject = RenderManager::Instance()->CreateVertexArrayObject(Simple, DynamicDraw);
		}
		else
		{
			//color fill
			_shapeObject = RenderManager::Instance()->CreateVertexArrayObject(Simple, DynamicDraw);
		}


		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, true);
		JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, true);
		
		_created = true;
	}
	else if (!mIsStatic)
	{
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, false);
		JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, false);
	}

	_color = glm::vec4(R, G, B, 0.7f);
	JellyHellper::Instance()->DrawShape(_shapeObject, proj, _color);

	//draw lines
	JellyHellper::Instance()->DrawLines(_vertexObject, proj, _lineColor);
}