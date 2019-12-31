#include "Tire.h"
#include "../Utils/JellyHelper.h"

Tire::Tire(World *w, const ClosedShape& s,
	float d, float mpp, float pressure1, float pressure2,
	float shapeK, float shapeDamp, float edgeK1, float edgeDamp1,
	float edgeK2, float edgeDamp2, float t1, float t2,
	float maxO1, float maxO2, const Vector2& pos) :
	PressureBody(w, s, mpp, pressure1, shapeK, shapeDamp, edgeK1, edgeDamp1, pos, 0.0f, Vector2::One, false)
{
	p1 = pressure1;
	p2 = pressure2;
	ek1 = edgeK1;
	ek2 = edgeK2;
	ed1 = edgeDamp1;
	ed2 = edgeDamp2;
	torque1 = t1;
	torque2 = t2;
	torque = torque1;
	drive = d;
	maxOmega1 = maxO1;
	maxOmega2 = maxO2;
	maxOmega = maxOmega1;

	torqueForce = 0.0f;
	shockForce = Vector2::Zero;

	smallBig = false;
	ballon = false;

	_vertexObject = RenderManager::Instance()->CreateVertexArrayObject(Simple, DynamicDraw);
	_vertexObject->SetVertexPrimitive(Lines);

	_texturedObject = RenderManager::Instance()->CreateVertexArrayObject(Textured, DynamicDraw);
	_texturedObject->SetVertexPrimitive(TriangleFan);

	_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	_created = false;
	_useNearestGravity = false;

	SetTexturePoints();
	shape.resize(mPointMasses.size() + 2);
}

Tire::~Tire()
{
	delete _vertexObject;
	delete _texturedObject;

	mTetxure.clear();
	shape.clear();
}

void Tire::SetTexturePoints()
{
	//srodek
	mTetxure.push_back(Vector2(0.5f, 0.5f));

	//prawa strona
	mTetxure.push_back(Vector2(0.492f, 0.0f));
	mTetxure.push_back(Vector2(0.664f, 0.031f));
	mTetxure.push_back(Vector2(0.812f, 0.117f));
	mTetxure.push_back(Vector2(0.921f, 0.25f));
	mTetxure.push_back(Vector2(0.984f, 0.406f));

	mTetxure.push_back(Vector2(0.984f, 0.578f));
	mTetxure.push_back(Vector2(0.921f, 0.734f));
	mTetxure.push_back(Vector2(0.812f, 0.867f));
	mTetxure.push_back(Vector2(0.664f, 0.953f));
	mTetxure.push_back(Vector2(0.492f, 1.0f));

	//lewa strona
	mTetxure.push_back(Vector2(0.328f, 0.953f));
	mTetxure.push_back(Vector2(0.179f, 0.867f));
	mTetxure.push_back(Vector2(0.07f, 0.734f));
	mTetxure.push_back(Vector2(0.01f, 0.578f));

	mTetxure.push_back(Vector2(0.01f, 0.406f));
	mTetxure.push_back(Vector2(0.07f, 0.25f));
	mTetxure.push_back(Vector2(0.179f, 0.117f));
	mTetxure.push_back(Vector2(0.328f, 0.031f));

	//znowu 1
	mTetxure.push_back(Vector2(0.492f, 0.0f));

}

void Tire::SetTorque(float t) { torqueForce = t; }
void Tire::SetShockForce(const Vector2& f) { shockForce = f; }
void Tire::AddShockForce(const Vector2& f) { shockForce += f; }

void Tire::InterpolatePressure(float x)
{
	mGasAmount = p1 + ((p2 - p1) * x);

	// update edge springs.
	for (unsigned int i = 0; i < mSprings.size(); i++)
	{
		mSprings[i].springK = ek1 + ((ek2 - ek1) * x);
		mSprings[i].damping = ed1 + ((ed2 - ed1) * x);
	}

	torque = torque1 + ((torque2 - torque1) * x);
	maxOmega = maxOmega1 + ((maxOmega2 - maxOmega1) * x);
}

void Tire::UseNearestGracity(bool state)
{
	_useNearestGravity = state;
}

void Tire::accumulateExternalForces()
{
	PressureBody::accumulateExternalForces();

	Vector2 gravityForce(0, -12.0f);

	if (_useNearestGravity)
	{
		Vector2 pointInWorld = mWorld->getClosestBodyPointToBody(this);
		Vector2 gravityVector = pointInWorld - mAABB.getCenter();
		gravityVector.normalise();
		gravityVector *= 15.0f;

		gravityForce = gravityVector;
	}

	for (int i = 0; i < mPointCount; i++)
		mPointMasses[i].Force += gravityForce * mPointMasses[i].Mass;

	// torque
	float omegaFactor = (absf(((mDerivedOmega > 0) ? maxOmega : -maxOmega) - mDerivedOmega) / maxOmega) * torqueForce * torque;
	for (int i = 0; i < mPointCount; i++)
	{
		Vector2 toPt = (mPointMasses[i].Position - mDerivedPos);
		toPt = VectorTools::rotateVector(toPt, NEG_PI_OVER_ONE_POINT_TWO);

		mPointMasses[i].Force += toPt * omegaFactor;
	}

	// shock forces.
	shockForce /= mPointCount;
	for (int i = 0; i < mPointCount; i++)
	{
		mPointMasses[i].Force += (shockForce);// / mPointCount);
	}
}

float Tire::GetDrive() { return drive; }
float Tire::GetMaxOmega() { return maxOmega; }

void Tire::Draw(glm::mat4 &proj, Texture* texture)
{
	//update blob shape
	{
		float srodekX = ((mAABB.Max.X - mAABB.Min.X) / 2) + mAABB.Min.X;
		float srodekY = ((mAABB.Max.Y - mAABB.Min.Y) / 2) + mAABB.Min.Y;

		shape[0].X = srodekX;
		shape[0].Y = srodekY;

		for (unsigned int i = 0; i < mPointMasses.size(); i++)
		{
			shape[i + 1] = ((PointMass)mPointMasses.at(i)).Position;
		}

		shape[mPointMasses.size() + 1] = mPointMasses[0].Position;
	}

	if (!_created)
	{
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, true);
		JellyHellper::Instance()->UpdateTexturedBlob(_texturedObject, shape, shape.size(), mTetxure, true);
		_created = true;
	}
	else
	{
		JellyHellper::Instance()->UpdateTexturedBlob(_texturedObject, shape, shape.size(), mTetxure, false);
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, false);
	}

	_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	JellyHellper::Instance()->DrawTextured(_texturedObject, proj, texture, _color);

	_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	JellyHellper::Instance()->DrawLines(_vertexObject, proj, _color);
}