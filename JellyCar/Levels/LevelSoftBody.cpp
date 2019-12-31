#include "LevelSoftBody.h"
#include "GameSpringBody.h"
#include "GamePressureBody.h"

#include "tinyxml.h"
#include <string>

#include <Andromeda/FileSystem/FileManager.h>
using namespace Andromeda;

LevelSoftBody::LevelSoftBody(std::string fileName, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material)
{
	massPerPoint = 0.0f;
	edgeK = 100.0f;
	edgeDamping = 1.0f;
	kinematic = false;
	shapeMatching = false;
	shapeK = 100.0f;
	shapeDamping = 10.0f;
	pressureized = false;
	pressure = 0.0f;
	_visible = true;

	JellyPhysics::ClosedShape shape;

	//loac main level file
	FileSystem::BaseFile* file = FileSystem::FileManager::Instance()->GetFile(fileName);

	if (file == 0)
		return;

	file->Open(FileSystem::Read, FileSystem::Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem)
	{
		//Error - Can't find root :/
		return;
	}


	colorR = colorG = colorB = 0.0f;
	const char* sKinematic, *sShapeMatching;
	//bool kinematic,shapeMatching;

	//basic info
	massPerPoint = std::stof(pElem->Attribute("massPerPoint"));
	edgeK = std::stof(pElem->Attribute("edgeK"));
	edgeDamping = std::stof(pElem->Attribute("edgeDamping"));

	//load color of the body
	if (pElem->Attribute("colorR") != NULL)
	{
		colorR = std::stof(pElem->Attribute("colorR"));
		colorG = std::stof(pElem->Attribute("colorG"));
		colorB = std::stof(pElem->Attribute("colorB"));
	}

	//is body kinematic
	if (pElem->Attribute("kinematic") != NULL)
	{
		sKinematic = pElem->Attribute("kinematic");
		if (strcmp(sKinematic, "True") == 0)
			kinematic = true;
		else
			kinematic = false;
	}

	//shape matching
	if (pElem->Attribute("shapeMatching") != NULL)
	{
		sShapeMatching = pElem->Attribute("shapeMatching");
		if (strcmp(sShapeMatching, "True") == 0)
		{
			shapeMatching = true;
			shapeK = std::stof(pElem->Attribute("shapeK"));
			shapeDamping = std::stof(pElem->Attribute("shapeDamping"));
		}
	}

	//end of first section
	hRoot = TiXmlHandle(pElem);

	//pressure
	pElem = hRoot.FirstChild("Pressure").Element();
	if (pElem)
	{
		pressureized = true;
		pressure = std::stof(pElem->Attribute("amount"));
	}

	//SHAPE

	//Points
	shape.begin();

	int _id = 0;
	TiXmlElement* PointNode = hRoot.FirstChild("Points").FirstChild().Element();
	for (PointNode; PointNode; PointNode = PointNode->NextSiblingElement())
	{
		float x = 0.0f, y = 0.0f;
		x = std::stof(PointNode->Attribute("x"));
		y = std::stof(PointNode->Attribute("y"));
		shape.addVertex(Vector2(x, y));

		if (PointNode->Attribute("mass") != NULL)
		{
			MassID m;
			m._id = _id;
			m._mass = std::stof(PointNode->Attribute("mass"));

			massExceptions.push_back(m);
		}

		_id++;
	}

	//shape->finish(false);
	shape.finish(((massPerPoint != 0.0f) && (!kinematic)));

	if (!pressureized)
	{
		mBody = new GameSpringBody(mWorld, shape, massPerPoint, edgeK, edgeDamping, pos, angle, scale, kinematic);
	}
	else
	{
		mBody = new GamePressureBody(mWorld, shape, massPerPoint, pressure, shapeK, shapeDamping, edgeK, edgeDamping, pos, angle, scale, kinematic);
	}

	mBody->setMaterial(material);
	mBody->setVelocityDamping(0.993f);

	if (shapeMatching && (!pressureized))
	{
		static_cast<GameSpringBody*>(mBody)->setShapeMatching(true);
		static_cast<GameSpringBody*>(mBody)->setShapeMatchingConstants(shapeK, shapeDamping);
	}

	// were there any mass exceptions?
	for (unsigned int i = 0; i < massExceptions.size(); i++)
	{
		mBody->setMassIndividual(massExceptions[i]._id, massExceptions[i]._mass);
	}

	//Springs
	int springCount = 0;
	TiXmlElement* SpingNode = hRoot.FirstChild("Springs").FirstChild().Element();
	for (SpingNode; SpingNode; SpingNode = SpingNode->NextSiblingElement())
	{
		int pt1, pt2;
		float k, damp;
		pt1 = atoi(SpingNode->Attribute("pt1"));
		pt2 = atoi(SpingNode->Attribute("pt2"));
		k = std::stof(SpingNode->Attribute("k"));
		damp = std::stof(SpingNode->Attribute("damp"));

		if (!pressureized)
		{
			static_cast<GameSpringBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}
		else
		{
			static_cast<GamePressureBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}
	}

	//Polygons
	TiXmlElement* PolygonNode = hRoot.FirstChild("Polygons").FirstChild().Element();
	for (PolygonNode; PolygonNode; PolygonNode = PolygonNode->NextSiblingElement())
	{
		int pt0 = 0, pt1 = 0, pt2 = 0;
		pt0 = atoi(PolygonNode->Attribute("pt0"));
		pt1 = atoi(PolygonNode->Attribute("pt1"));
		pt2 = atoi(PolygonNode->Attribute("pt2"));

		// polygons!
		AddTriangle(pt0, pt1, pt2);
	}

	FinalizeTriangles();
}

LevelSoftBody::LevelSoftBody(LevelSoftBody *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material)
{
	massPerPoint = 0.0f;
	edgeK = 100.0f;
	edgeDamping = 1.0f;
	kinematic = false;
	shapeMatching = false;
	shapeK = 100.0f;
	shapeDamping = 10.0f;
	pressureized = false;
	pressure = 0.0f;

	_visible = true;

	JellyPhysics::ClosedShape shape;

	colorR = exBody->colorR;
	colorG = exBody->colorG;
	colorB = exBody->colorB;

	//basic info
	massPerPoint = exBody->massPerPoint;
	edgeK = exBody->edgeK;
	edgeDamping = exBody->edgeDamping;

	//is body kinematic
	kinematic = exBody->kinematic;

	//shape matching
	shapeMatching = exBody->shapeMatching;
	shapeK = exBody->shapeK;
	shapeDamping = exBody->shapeDamping;

	//pressure
	pressureized = exBody->pressureized;
	pressure = exBody->pressure;

	//SHAPE
	//Points
	shape.begin();

	for (unsigned int i = 0; i < exBody->mBody->mBaseShape.getVertices().size(); i++)
	{
		shape.addVertex(exBody->mBody->mBaseShape.getVertices()[i]);
	}

	//shape->finish(false);
	shape.finish(((massPerPoint != 0.0f) && (!kinematic)));

	if (!pressureized)
	{
		mBody = new GameSpringBody(mWorld, shape, massPerPoint, edgeK, edgeDamping, pos, angle, scale, kinematic);
	}
	else
	{
		mBody = new GamePressureBody(mWorld, shape, massPerPoint, pressure, shapeK, shapeDamping, edgeK, edgeDamping, pos, angle, scale, kinematic);
	}

	mBody->setMaterial(exBody->mBody->getMaterial());
	mBody->setVelocityDamping(0.993f);

	if (shapeMatching && (!pressureized))
	{
		((GameSpringBody*)mBody)->setShapeMatching(true);
		((GameSpringBody*)mBody)->setShapeMatchingConstants(shapeK, shapeDamping);
	}

	// were there any mass exceptions?
	for (unsigned int i = 0; i < exBody->massExceptions.size(); i++)
	{
		mBody->setMassIndividual(exBody->massExceptions[i]._id, exBody->massExceptions[i]._mass);
	}

	//Springs
	if (!pressureized)
	{
		for (unsigned int i = 0; i < exBody->mBody->mSprings.size(); i++)
		{
			((GameSpringBody*)mBody)->addInternalSpring(exBody->mBody->mSprings[i].pointMassA, exBody->mBody->mSprings[i].pointMassB, exBody->mBody->mSprings[i].springK, exBody->mBody->mSprings[i].damping);
		}
	}
	else
	{
		for (unsigned int i = 0; i < exBody->mBody->mSprings.size(); i++)
		{
			((GamePressureBody*)mBody)->addInternalSpring(exBody->mBody->mSprings[i].pointMassA, exBody->mBody->mSprings[i].pointMassB, exBody->mBody->mSprings[i].springK, exBody->mBody->mSprings[i].damping);
		}
	}

	//Polygons
	for (unsigned int i = 0; i < exBody->mIndexList.size(); i++)
	{
		mIndexList.push_back(exBody->mIndexList[i]);
	}
	
	FinalizeTriangles();
}

LevelSoftBody::LevelSoftBody(BodyObject *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material)
{
	massPerPoint = 0.0f;
	edgeK = 100.0f;
	edgeDamping = 1.0f;
	kinematic = false;
	shapeMatching = false;
	shapeK = 100.0f;
	shapeDamping = 10.0f;
	pressureized = false;
	pressure = 0.0f;

	_visible = true;

	JellyPhysics::ClosedShape shape;

	colorR = exBody->info.colorR;
	colorG = exBody->info.colorG;
	colorB = exBody->info.colorB;

	//basic info
	massPerPoint = exBody->info.massPerPoint;
	edgeK = exBody->info.edgeK;
	edgeDamping = exBody->info.edgeDamping;

	//is body kinematic
	kinematic = exBody->info.isKinematic;

	//shape matching
	shapeMatching = exBody->info.shapeMatching;
	shapeK = exBody->info.shapeK;
	shapeDamping = exBody->info.shapeDamping;

	//pressure
	pressureized = exBody->info.pressureized;
	pressure = exBody->info.pressure;

	
	//Points
	shape.begin();
	int _id = 0;

	for (int i = 0; i < exBody->points; i++)
	{
		shape.addVertex(Vector2(exBody->bodyPoints[i].x, exBody->bodyPoints[i].y));
		if (exBody->bodyPoints[i].mass != -1)
		{
			MassID m;
			m._id = _id;
			m._mass = exBody->bodyPoints[i].mass;

			massExceptions.push_back(m);
		}

		_id++;
	}

	//shape->finish(false);
	shape.finish(((massPerPoint != 0.0f) && (!kinematic)));

	if (!pressureized)
	{
		mBody = new GameSpringBody(mWorld, shape, massPerPoint, edgeK, edgeDamping, pos, angle, scale, kinematic);
	}
	else
	{
		mBody = new GamePressureBody(mWorld, shape, massPerPoint, pressure, shapeK, shapeDamping, edgeK, edgeDamping, pos, angle, scale, kinematic);
	}

	mBody->setMaterial(material);
	mBody->setVelocityDamping(0.993f);

	if (shapeMatching && (!pressureized))
	{
		static_cast<GameSpringBody*>(mBody)->setShapeMatching(true);
		static_cast<GameSpringBody*>(mBody)->setShapeMatchingConstants(shapeK, shapeDamping);
	}

	// were there any mass exceptions?
	for (unsigned int i = 0; i < massExceptions.size(); i++)
	{
		mBody->setMassIndividual(massExceptions[i]._id, massExceptions[i]._mass);
	}

	//Springs
	for (int i = 0; i < exBody->springs; i++)
	{
		int pt1, pt2;
		float k, damp;

		pt1 = exBody->bodySprings[i].pt1;
		pt2 = exBody->bodySprings[i].pt2;
		k = exBody->bodySprings[i].k;
		damp = exBody->bodySprings[i].damp;

		if (!pressureized)
		{
			static_cast<GameSpringBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}
		else
		{
			static_cast<GamePressureBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}
	}

	//polygons
	for (int i = 0; i <  exBody->polygons; i++)
	{
		int pt0 = 0, pt1 = 0, pt2 = 0;

		pt0 = exBody->bodyPolygons[i].x;
		pt1 = exBody->bodyPolygons[i].y;
		pt2 = exBody->bodyPolygons[i].z;

		AddTriangle(pt0, pt1, pt2);
	}

	FinalizeTriangles();
}

LevelSoftBody::LevelSoftBody(std::string fileName, World *mWorld, ObjectInfo bodyInfo) : LevelSoftBody(fileName, mWorld, Vector2(bodyInfo.posX, bodyInfo.posY), VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY), bodyInfo.material)
{
	_bodyInfo = bodyInfo;
	mBody->SetName(_bodyInfo.name);
}

LevelSoftBody::LevelSoftBody(LevelSoftBody *exBody, World *mWorld, ObjectInfo bodyInfo) : LevelSoftBody(exBody, mWorld, Vector2(bodyInfo.posX, bodyInfo.posY), VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY), bodyInfo.material)
{
	_bodyInfo = bodyInfo;
	mBody->SetName(_bodyInfo.name);
}

LevelSoftBody::LevelSoftBody(BodyObject *exBody, World *mWorld, ObjectInfo bodyInfo) : LevelSoftBody(exBody, mWorld, Vector2(bodyInfo.posX, bodyInfo.posY), VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY), bodyInfo.material)
{
	_bodyInfo = bodyInfo;
	mBody->SetName(_bodyInfo.name);
}

LevelSoftBody::~LevelSoftBody()
{
	massExceptions.clear();

	for (unsigned int i = 0; i < mControls.size(); i++)
		delete mControls[i];

	mControls.clear();

	delete[] mIndices;
	mIndexList.clear();

	delete mBody;
}

void LevelSoftBody::AddKinematicControl(KinematicControl* kinematicControl)
{
	mControls.push_back(kinematicControl);
}

void LevelSoftBody::Finalize()
{
	// generate the "ignoreMe" AABB.
	if (mBody->getIsKinematic())
	{
		Vector2 originalPos = mBody->getDerivedPosition();
		float originalAngle = mBody->getDerivedAngle();

		mIgnoreAABB.clear();

		float maxTime = 1.0f;
		for (ControlList::iterator it = mControls.begin(); it != mControls.end(); it++)
		{
			if ((*it)->GetPeriod() > maxTime) { maxTime = (*it)->GetPeriod(); }
		}

		float step = maxTime / 10.0f;

		mBody->updateAABB(0.0f, true);
		mIgnoreAABB.expandToInclude(mBody->getAABB());

		for (int i = 0; i < 10; i++)
		{
			for (ControlList::iterator it = mControls.begin(); it != mControls.end(); it++)
				(*it)->Update(step);

			mBody->setPositionAngle(mBody->getDerivedPosition(), mBody->getDerivedAngle(), mBody->getScale());
			mBody->updateAABB(0.0f, true);
			mIgnoreAABB.expandToInclude(mBody->getAABB());
		}

		mBody->setPositionAngle(originalPos, originalAngle, mBody->getScale());
		mBody->updateAABB(0.0f, true);
	}
}

void LevelSoftBody::Update(float elapsed)
{
	for (std::vector<KinematicControl*>::iterator it = mControls.begin(); it != mControls.end(); it++)
		(*it)->Update(elapsed);
}

void LevelSoftBody::SetIgnore(bool state)
{
	bool prevIg = mBody->getIgnoreMe();

	mBody->setIgnoreMe(state);

	if ((prevIg) && (!state) && (mBody->getIsKinematic()))
	{
		mBody->setPositionAngle(mBody->getDerivedPosition(), mBody->getDerivedAngle(), mBody->getScale());
	}
}

Body* LevelSoftBody::GetBody()
{
	return mBody;
}

const AABB& LevelSoftBody::GetIgnoreAABB()
{
	if (mBody->getIsKinematic())
		return mIgnoreAABB;
	else
		return mBody->getAABB();
}

std::string LevelSoftBody::GetName()
{
	return _bodyInfo.name;
}

glm::vec2 LevelSoftBody::GetStartPosition()
{
	return glm::vec2(_bodyInfo.posX, _bodyInfo.posY);
}

void LevelSoftBody::SetVisible(bool visible)
{
	_visible = visible;
}

bool LevelSoftBody::IsVisible()
{
	return _visible;
}

void LevelSoftBody::AddTriangle(int a, int b, int c)
{
	mIndexList.push_back(a);
	mIndexList.push_back(b);
	mIndexList.push_back(c);
}

void LevelSoftBody::FinalizeTriangles()
{
	mIndicesCount = mIndexList.size();
	mIndices = new int[mIndexList.size()];
	for (unsigned int i = 0; i < mIndexList.size(); i++)
		mIndices[i] = mIndexList[i];
}

void LevelSoftBody::SetTexture(Andromeda::Graphics::Texture* texture)
{
	if (pressureized == true)
	{
		static_cast<GamePressureBody*>(mBody)->SetTexture(texture);
	}
	else
	{
		static_cast<GameSpringBody*>(mBody)->SetTexture(texture);
	}
}

void LevelSoftBody::SetLineColor(glm::vec4 color)
{
	if (pressureized == true)
	{
		static_cast<GamePressureBody*>(mBody)->SetLineColor(color);
	}
	else
	{
		static_cast<GameSpringBody*>(mBody)->SetLineColor(color);
	}
}

void LevelSoftBody::Draw(glm::mat4 &proj)
{
	if (_visible)
	{
		if (pressureized == true)
		{
			static_cast<GamePressureBody*>(mBody)->Draw(proj, mIndices, mIndicesCount, colorR, colorG, colorB);
		}
		else
		{
			static_cast<GameSpringBody*>(mBody)->Draw(proj, mIndices, mIndicesCount, colorR, colorG, colorB);
		}
	}
}