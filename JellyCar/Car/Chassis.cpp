#include "Chassis.h"
#include "../Utils/JellyHelper.h"

Chassis::Chassis(World *w, const ClosedShape& sA, const ClosedShape& sB,
	float mpp, float shapeK, float shapeDamp, float edgeK, float edgeDamp,
	const Vector2& pos, const Vector2& scale) :
	SpringBody(w, sA, mpp, shapeK, shapeDamp, edgeK, edgeDamp, pos, 0.0f, scale, false), mShapeA(sA), mShapeB(sB)
{
	torqueForce = 40.0f;
	torque = 0.0f;
	maxOmega = 5.0f;

	smallBig = false;
	ballon = false;

	_vertexObject = RenderManager::Instance()->CreateVertexArrayObject(Simple, DynamicDraw);
	_vertexObject->SetVertexPrimitive(Lines);

	_texturedObject = RenderManager::Instance()->CreateVertexArrayObject(Textured, DynamicDraw);

	_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	_created = false;
	_useNearestGravity = false;

	GenerateTextures();
	//GenerateVertices();
}

Chassis::~Chassis()
{
	mTexture1.clear();
	mTexture2.clear();
	mShocks.clear();
	delete[] mIndices;
	mPolys.clear();

	delete _vertexObject;
	delete _texturedObject;
}

void Chassis::AddShock(int PM, Tire* t, float d1, float d2, float k1, float k2, float damp1, float damp2)
{
	mShocks.push_back(SuspensionShock(PM, t, d1, d2, k1, k2, damp1, damp2));
}

void Chassis::accumulateExternalForces()
{
	SpringBody::accumulateExternalForces();

	Vector2 gravityForce(0, -12.0f);

	// gravity.
	if (ballon)
	{
		for (int i = 0; i < mPointCount; i++)
			mPointMasses[i].Force.Y += -12.0f * mPointMasses[i].Mass;

		//drag
		mPointMasses[5].Force.Y += 450;

		//shocks
		shockForce /= (float)mPointCount;
		for (int i = 0; i < mPointCount; i++)
		{
			mPointMasses[i].Force += shockForce;// / mPointCount);
		}
	}
	/*else if (_useNearestGravity)
	{
		Vector2 pointInWorld = mWorld->getClosestBodyPointToBody(this);
		Vector2 gravityVector = pointInWorld - mAABB.getCenter();
		gravityVector.normalise();
		gravityVector *= 15.0f;

		gravityForce = gravityVector;
	}*/
	else
	{
		for (int i = 0; i < mPointCount; i++)
			mPointMasses[i].Force += gravityForce * mPointMasses[i].Mass;
	}

	// shock forces!
	for (unsigned int i = 0; i < mShocks.size(); i++)
	{
		Vector2 pmp = mPointMasses[mShocks[i].mChassisPM].Position;
		Vector2 pmv = mPointMasses[mShocks[i].mChassisPM].Velocity;

		Tire *tire = mShocks[i].mTire;

		Vector2 force = VectorTools::calculateSpringForce(pmp, pmv, tire->getDerivedPosition(), tire->getDerivedVelocity(), mShocks[i].mD, mShocks[i].mK, mShocks[i].mDamp);

		//printf("chassis shocks: pmp[%f][%f] pmv[%f][%f] tp[%f][%f] tv[%f][%f]\n", pmp.X, pmp.Y, pmv.X, pmv.Y, tp.X, tp.Y, tv.X, tv.Y );
		//printf("chassis scocks: d[%f] k[%f] damp[%f]\n", mShocks[i].mD, mShocks[i].mK, mShocks[i].mDamp);
		//printf("chassis shocks: force:[%f][%f]\n", force.X, force.Y);

		mPointMasses[mShocks[i].mChassisPM].Force += force;

		force = -force;

		tire->AddShockForce(force);
	}


	// torque
	float omegaFactor = (absf(((mDerivedOmega > 0) ? maxOmega : -maxOmega) - mDerivedOmega) / maxOmega) * torqueForce * torque;
	for (int i = 0; i < mPointCount; i++)
	{
		Vector2 toPt = (mPointMasses[i].Position - mDerivedPos);
		toPt = VectorTools::rotateVector(toPt, NEG_PI_OVER_ONE_POINT_TWO);

		mPointMasses[i].Force += toPt * omegaFactor;
	}
}

void Chassis::InterpolateShape(float x)
{
	for (unsigned int i = 0; i < mBaseShape.getVertices().size(); i++)
		mBaseShape.getVertices()[i] = mShapeA.getVertices()[i].slerp(mShapeB.getVertices()[i], x);

	// now update internal springs.
	for (unsigned int i = 0; i < mSprings.size(); i++)
	{
		float newD = (mBaseShape.getVertices()[mSprings[i].pointMassA] - mBaseShape.getVertices()[mSprings[i].pointMassB]).length();
		mSprings[i].springD = newD;
	}

	// now update shocks.
	for (unsigned int i = 0; i < mShocks.size(); i++)
		mShocks[i].interpolateD(x);
}



void Chassis::SetTorque(float t) { torque = t; }
float Chassis::GetTorque() { return torque; }



void Chassis::SetShockForce(const Vector2& f)
{
	shockForce = f;
}
void Chassis::AddShockForce(const Vector2& f)
{
	shockForce += f;
}

void Chassis::SetBallonState(bool state)
{
	ballon = state;
}

void Chassis::UseNearestGracity(bool state)
{
	_useNearestGravity = state;
}

void Chassis::StartPolys()
{
	mPolys.clear();
}

void Chassis::AddPoly(int a, int b, int c)
{
	mPolys.push_back(a);
	mPolys.push_back(b);
	mPolys.push_back(c);
}

void Chassis::EndPolys()
{
	mIndices = new int[mPolys.size()];
	for (unsigned int i = 0; i < mPolys.size(); i++)
		mIndices[i] = (int)mPolys[i];

	mIndexCount = mPolys.size();
	mPolys.clear();

	//GenerateVertices();
}

void Chassis::GenerateTextures()
{
	//for small shape 14 points
	//h - 0.687500000
	mTexture1.push_back(Vector2(0.031f, 0.964f));
	mTexture1.push_back(Vector2(0.0f, 0.589f));

	mTexture1.push_back(Vector2(0.19f, 0.513f));

	mTexture1.push_back(Vector2(0.303f, 0.039f));
	mTexture1.push_back(Vector2(0.48f, 0.034f));
	mTexture1.push_back(Vector2(0.613f, 0.029f));

	mTexture1.push_back(Vector2(0.718f, 0.44f));
	mTexture1.push_back(Vector2(0.996f, 0.45f));

	//down
	mTexture1.push_back(Vector2(0.972f, 0.936f));
	mTexture1.push_back(Vector2(0.718f, 0.944f));
	mTexture1.push_back(Vector2(0.613f, 0.948f));
	mTexture1.push_back(Vector2(0.48f, 0.952f));
	mTexture1.push_back(Vector2(0.35f, 0.956f));
	mTexture1.push_back(Vector2(0.19f, 0.96f));

	//for big shape 14 points
	//w - 0.632812500

	mTexture2.push_back(Vector2(0.01f, 0.975f));
	mTexture2.push_back(Vector2(0.01f, 0.35f));

	mTexture2.push_back(Vector2(0.172f, 0.030f));
	mTexture2.push_back(Vector2(0.3f, 0.023f));
	mTexture2.push_back(Vector2(0.449f, 0.020f));
	mTexture2.push_back(Vector2(0.601f, 0.017f));
	mTexture2.push_back(Vector2(0.75f, 0.429f));
	mTexture2.push_back(Vector2(0.99f, 0.449f));

	//down
	mTexture2.push_back(Vector2(0.96f, 0.985f));
	mTexture2.push_back(Vector2(0.804f, 0.985f));
	mTexture2.push_back(Vector2(0.644f, 0.985f));
	mTexture2.push_back(Vector2(0.484f, 0.985f));
	mTexture2.push_back(Vector2(0.324f, 0.985f));
	mTexture2.push_back(Vector2(0.164f, 0.985f));
}

void Chassis::GenerateVertices()
{
	/*_texturedObject = RenderManager::Instance()->CreateVertexArrayObject(Textured, StaticDraw);

	//create vertices
	_texturedObject->CreateVertices(mPointMasses.size());

	//get vertices
	TextureVertex* _simpleData = static_cast<TextureVertex*>(_texturedObject->GetVertices());

	for (size_t i = 0; i < mPointMasses.size(); i++)
	{
		_simpleData[i].x = mPointMasses[i].Position.X;
		_simpleData[i].y = mPointMasses[i].Position.Y;
		_simpleData[i].z = 0.0f;

		_simpleData[i].u = mTexture1[i].X;
		_simpleData[i].v = mTexture1[i].Y;
	}

	//create indices
	_texturedObject->CreateIndices(mPolys.size());

	//get indices
	unsigned short* _indices = static_cast<unsigned short*>(_texturedObject->GetIndices());

	for (size_t i = 0; i < mPolys.size(); i++)
	{
		_indices[i] = mPolys[i];
	}

	//generate buffer object
	_texturedObject->Generate();*/

	//test
	{
		_vertexObject = RenderManager::Instance()->CreateVertexArrayObject(Textured, StaticDraw);

		//create vertices
		_vertexObject->CreateVertices(14);

		//get vertices
		TextureVertex* _simpleData = static_cast<TextureVertex*>(_vertexObject->GetVertices());

		_simpleData[0].x = -3.8f;		_simpleData[0].y = -1.6f;		_simpleData[0].z = 0.0f;
		_simpleData[1].x = -3.8f;		_simpleData[1].y = 0.4f;		_simpleData[1].z = 0.0f;
		_simpleData[2].x = -2.6f;		_simpleData[2].y = 1.6f;		_simpleData[2].z = 0.0f;
		_simpleData[3].x = -1.4f;		_simpleData[3].y = 1.6f;		_simpleData[3].z = 0.0f;
		_simpleData[4].x = 0.0f;		_simpleData[4].y = 1.6f;		_simpleData[4].z = 0.0f;
		_simpleData[5].x = 1.4f;		_simpleData[5].y = 1.6f;		_simpleData[5].z = 0.0f;
		_simpleData[6].x = 2.2f;		_simpleData[6].y = 0.4f;		_simpleData[6].z = 0.0f;
		_simpleData[7].x = 4.2f;		_simpleData[7].y = 0.0f;		_simpleData[7].z = 0.0f;
		_simpleData[8].x = 3.8f;		_simpleData[8].y = -1.6f;		_simpleData[8].z = 0.0f;
		_simpleData[9].x = 2.6f;		_simpleData[9].y = -1.6f;		_simpleData[9].z = 0.0f;
		_simpleData[10].x = 1.4f;		_simpleData[10].y = -1.6f;		_simpleData[10].z = 0.0f;
		_simpleData[11].x = 0.0f;		_simpleData[11].y = -1.6f;		_simpleData[11].z = 0.0f;
		_simpleData[12].x = -1.4f;		_simpleData[12].y = -1.6f;		_simpleData[12].z = 0.0f;
		_simpleData[13].x = -2.6f;		_simpleData[13].y = -1.6f;		_simpleData[13].z = 0.0f;

		_simpleData[0].u = 0.01f;	_simpleData[0].v = 0.975f;
		_simpleData[1].u = 0.01f;	_simpleData[1].v = 0.35f;
		_simpleData[2].u = 0.172f;  _simpleData[2].v = 0.030f;
		_simpleData[3].u = 0.3f;	_simpleData[3].v = 0.023f;
		_simpleData[4].u = 0.449f;  _simpleData[4].v = 0.020f;
		_simpleData[5].u = 0.601f;  _simpleData[5].v = 0.017f;
		_simpleData[6].u = 0.75f;	_simpleData[6].v = 0.429f;
		_simpleData[7].u = 0.99f;	_simpleData[7].v = 0.449f;
		_simpleData[8].u = 0.96f;	_simpleData[8].v = 0.985f;
		_simpleData[9].u = 0.804f;  _simpleData[9].v = 0.985f;
		_simpleData[10].u = 0.644f;  _simpleData[10].v = 0.985f;
		_simpleData[11].u = 0.484f;  _simpleData[11].v = 0.985f;
		_simpleData[12].u = 0.324f;  _simpleData[12].v = 0.985f;
		_simpleData[13].u = 0.164f;  _simpleData[13].v = 0.985f;

		//create indices
		_vertexObject->CreateIndices(36);

		//get indices
		unsigned short* _indices = static_cast<unsigned short*>(_vertexObject->GetIndices());

		_indices[0] = 0;	_indices[1] = 1;	_indices[2] = 13;
		_indices[3] = 1;	_indices[4] = 2;	_indices[5] = 13;
		_indices[6] = 2;	_indices[7] = 12;	_indices[8] = 13;
		_indices[9] = 2;	_indices[10] = 3;	_indices[11] = 12;
		_indices[12] = 3;	_indices[13] = 11;	_indices[14] = 12;
		_indices[15] = 3;	_indices[16] = 4;	_indices[17] = 11;
		_indices[18] = 4;	_indices[19] = 10;	_indices[20] = 11;
		_indices[21] = 4;	_indices[22] = 5;	_indices[23] = 10;
		_indices[24] = 5;	_indices[25] = 9;	_indices[26] = 10;
		_indices[27] = 5;	_indices[28] = 6;	_indices[29] = 9;
		_indices[30] = 6;	_indices[31] = 8;	_indices[32] = 9;
		_indices[33] = 6;	_indices[34] = 7;	_indices[35] = 8;

		//generate buffer object
		_vertexObject->Generate();
	}

}

void Chassis::Draw(glm::mat4 &proj, Texture* texture)
{
	if (!_created)
	{
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, true);
		JellyHellper::Instance()->UpdateTextured(_texturedObject, mPointMasses, mTexture1, mIndices, mIndexCount, true);
		_created = true;
	}
	else
	{
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, false);		

		if (smallBig)
		{
			JellyHellper::Instance()->UpdateTextured(_texturedObject, mPointMasses, mTexture2, mIndices, mIndexCount, false);
		}
		else
		{
			JellyHellper::Instance()->UpdateTextured(_texturedObject, mPointMasses, mTexture1, mIndices, mIndexCount, false);
		}
		
	}

	_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	JellyHellper::Instance()->DrawTextured(_texturedObject, proj, texture, _color);

	_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	JellyHellper::Instance()->DrawLines(_vertexObject, proj, _color);
}

