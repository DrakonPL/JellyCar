#ifndef Car_H
#define Car_H

#include <Andromeda/Graphics/RenderManager.h>
using namespace Andromeda::Graphics;

#include "JellyPhysics/JellyPhysics.h"
using namespace JellyPhysics;

#include "Tire.h"
#include "Chassis.h"
#include "GameBallonBody.h"

class Car
{
private:

	Texture* smallChassisTexture;
	Texture* bigChassisTexture;
	Texture* smallTireTexture;
	Texture* bigTireTexture;

public:

	typedef std::vector<Tire*>		TireList;

	World*							mainWorld;
	TireList						mTires;
	Chassis*						mChassis;

	float							mShapeD;
	float							mShapeDelta;
	float							mTransformSpeed;

	ClosedShape						ballonShape;
	GameBallonBody*					ballonBody;
	bool							ballon;

public:

	enum TransformState { Normal, Transformed };
	TransformState		TransformStatus;

	Car(std::string xmlFile, World *mWorld, const Vector2& pos, int chassisMat, int tireMat);
	~Car();

	void clearForces();
	void setTorque(float t);
	bool Transform();
	void update(float elapsed);

	void UseNearestGracity(bool state);
	
	void UseBallon(bool state);
	void activateBallon();
	void deactivateBallon();

	Vector2 getPosition();
	Vector2 getVelocity();
	float getTireOmega(int index);

	Body* getChassisBody();

	int getTireCount();
	Tire* getTire(int index);

	void setChassisTorque(float t);

	void SetChassisTextures(Texture* small,Texture* big);
	void SetTireTextures(Texture* small, Texture* big);

	void Draw(glm::mat4 &proj);
};

#endif 