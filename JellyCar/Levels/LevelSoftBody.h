#ifndef LevelSoftBody_H
#define LevelSoftBody_H

#include "JellyPhysics/JellyPhysics.h"
#include "SimpleStruct/BodyObject.h"


#include "ObjectInfo.h"
#include <glm/mat4x2.hpp>
#include "Andromeda/Graphics/Texture.h"

using namespace JellyPhysics;

#include "KinematicControl.h"

class LevelSoftBody
{
private:

	struct MassID
	{
		int _id;
		float _mass;
	};

	Body* mBody;

	typedef std::vector<KinematicControl*>	ControlList;

	ControlList				mControls;

	//---------------------------------------------------------------------
	//float*				mColorArray;
	JellyPhysics::AABB		mIgnoreAABB;


	//----------------------

	std::vector<int> mIndexList;
	int *mIndices;
	int mIndicesCount;

	float colorR, colorG, colorB;

	float massPerPoint;// = 0.0f;
	float edgeK;//= 100.0f;
	float edgeDamping;//= 1.0f;
	bool kinematic;//= false;
	bool shapeMatching;//= true;
	float shapeK;//= 100.0f;
	float shapeDamping;//= 10.0f;
	bool pressureized;//= false;
	float pressure;//= 0.0f;

	bool _visible;

	std::vector<MassID> massExceptions;
	ObjectInfo _bodyInfo;

public:

	LevelSoftBody(std::string fileName, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);
	LevelSoftBody(LevelSoftBody *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);
	LevelSoftBody(BodyObject *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);

	LevelSoftBody(std::string fileName, World *mWorld, ObjectInfo bodyInfo);
	LevelSoftBody(LevelSoftBody *exBody, World *mWorld, ObjectInfo bodyInfo);
	LevelSoftBody(BodyObject *exBody, World *mWorld, ObjectInfo bodyInfo);

	~LevelSoftBody();

	void AddKinematicControl(KinematicControl* kinematicControl);
	void Finalize();
	void Update(float elapsed);
	void SetIgnore(bool state);

	Body* GetBody();
	const AABB& GetIgnoreAABB();

	std::string GetName();
	glm::vec2 GetStartPosition();

	void SetVisible(bool visible);
	bool IsVisible();

	void AddTriangle(int a, int b, int c);
	void FinalizeTriangles();

	void SetTexture(Andromeda::Graphics::Texture* texture);
	void SetLineColor(glm::vec4 color);

	void Draw(glm::mat4 &proj);

};

#endif