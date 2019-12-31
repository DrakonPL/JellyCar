#include "LevelManager.h"
#include "ObjectInfo.h"
#include "KinematicMotor.h"
#include "KinematicPlatform.h"

#include "SimpleStruct/GameObject.h"

#include "SimpleStruct/BodyPoint.h"
#include "SimpleStruct/BodySpring.h"
#include "SimpleStruct/BodyPolygon.h"

#include "tinyxml.h"

#include <Andromeda/FileSystem/FileManager.h>
using namespace Andromeda;

#include <Andromeda/Graphics/TextureManager.h>
using namespace Andromeda::Graphics;

std::string LevelManager::GetPathName(std::string& fileName)
{
	char sep = '/';

	size_t i = fileName.rfind(sep, fileName.length());
	if (i != std::string::npos)
	{
		return(fileName.substr(0, i));
	}

	return("");
}

void LevelManager::SetAssetsLocation(std::string location)
{
	_assetLocation = location;
}

LevelManager::~LevelManager()
{
	_levels.clear();
	_skins.clear();

	_images.clear();
}

void LevelManager::LoadAllScenes(std::string fileName)
{
	fileName = _assetLocation + fileName;

	//loac main level file
	FileSystem::BaseFile* file = FileSystem::FileManager::Instance()->GetFile(fileName);

	if (file == 0)
		return;

	file->Open(FileSystem::Read, FileSystem::Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Scenes").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		LevelInfo info;

		info.name =  ObjectNode->Attribute("name");
		info.file = ObjectNode->Attribute("file");
		info.thumb = ObjectNode->Attribute("thumb");
		info.time = 999.0f;
		info.jump = 0.0f;

		_levels.push_back(info);
	}
}

void LevelManager::LoadCarSkins(std::string fileName)
{
	fileName = _assetLocation + fileName;

	//loac main level file
	FileSystem::BaseFile* file = FileSystem::FileManager::Instance()->GetFile(fileName);

	if (file == 0)
		return;

	file->Open(FileSystem::Read, FileSystem::Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Skins").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		SkinInfo skinInfo;
		skinInfo.name =  ObjectNode->Attribute("name");
		skinInfo.tireSmall = ObjectNode->Attribute("tireSmall");
		skinInfo.tireBig = ObjectNode->Attribute("tireBig");
		skinInfo.chassisSmall = ObjectNode->Attribute("chassisSmall");
		skinInfo.chassisBig = ObjectNode->Attribute("chassisBig");

		_skins.push_back(skinInfo);
	}
}

void LevelManager::InitPhysic(World *world)
{
	//add materials
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();

	// car tires don't collide with the chassis.
	world->setMaterialPairCollide(2, 3, false);

	//??
	world->setMaterialPairCollide(3, 3, false);

	//
	world->setMaterialPairCollide(0, 2, true);
	world->setMaterialPairCollide(0, 3, true);

	// car tires have a lot of friction against the ground.
	world->setMaterialPairData(0, 3, 1.0f, 0.0f);
	world->setMaterialPairData(1, 3, 1.0f, 0.0f);

	// ground cannot collide with itself, ever.
	world->setMaterialPairCollide(0, 0, false);

	// default ground vs, dynamic object settings.
	world->setMaterialPairData(0, 1, 0.4f, 1.0f);

	// default car tire vs ice.
	world->setMaterialPairData(3, 4, 0.0f, 0.0f);

	// default car chassis vs. ice
	world->setMaterialPairData(2, 4, 0.0f, 0.0f);

	// np collision between ice and other grounb objects.
	world->setMaterialPairCollide(0, 4, false);

	// no collision between ice and ice.
	world->setMaterialPairCollide(4, 4, false);

	// default friction for objects and ice.
	world->setMaterialPairData(1, 4, 0.0f, 0.0f);

	// no collision between items and other objects
	world->setMaterialPairCollide(0, 5, false);
	world->setMaterialPairCollide(1, 5, false);
	world->setMaterialPairCollide(2, 5, true);
	world->setMaterialPairCollide(3, 5, true);
	world->setMaterialPairCollide(4, 5, false);
	world->setMaterialPairCollide(5, 5, false);
	world->setMaterialPairCollide(6, 5, false);

	world->setMaterialPairCollide(0, 7, true);
	world->setMaterialPairCollide(7, 7, true);

	world->setMaterialPairCollide(2, 7, false);
	world->setMaterialPairCollide(3, 7, false);

	//no collision between ballon and car
	world->setMaterialPairCollide(2, 6, false);
	world->setMaterialPairCollide(3, 6, false);
}

void LevelManager::LoadCarImage(std::string& imageName)
{
	std::string imageLocation = _assetLocation + "Car_Skins/" + imageName;

	//load
	Texture* image = TextureManager::Instance()->LoadFromFile(imageLocation);

	if (image != 0)
	{
		//add to collection
		_images.insert(std::pair<std::string, Texture*>(imageName, image));
	}
}

void LevelManager::LoadLevelImage(std::string& imageName)
{
	std::string imageLocation = _assetLocation + "Texture/" + imageName + ".png";

	//load thumb
	Texture* image = TextureManager::Instance()->LoadFromFile(imageLocation);

	if (image != 0)
	{
		//add to collection
		_images.insert(std::pair<std::string, Texture*>(imageName, image));
	}
}

Texture* LevelManager::GetThumb(std::string levelName)
{
	std::string thumb = "";

	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			thumb = _levels[i].thumb;
			break;
		}
	}

	if (_images.find(thumb) == _images.end())
	{
		LoadLevelImage(thumb);
	}

	return _images[thumb];
}

Texture* LevelManager::GetCarImage(std::string carImage)
{
	if (_images.find(carImage) == _images.end())
	{
		LoadCarImage(carImage);
	}

	return _images[carImage];
}

std::vector<std::string> LevelManager::GetScenes()
{
	std::vector<std::string> files;

	for (size_t i = 0; i < _levels.size(); i++)
	{
		files.push_back(_levels[i].name);
	}

	return files;
}

std::vector<SkinInfo> LevelManager::GetCarSkins()
{
	return _skins;
}

bool LevelManager::LoadCompiledLevel(World *world, std::string levelName, std::string carFileName)
{
	worldLimits.clear();
	bool found = false;

	std::string levelFile = "";

	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			levelFile = _levels[i].file;
			found = true;
		}
	}

	if (!found)
	{
		levelFile = levelName;
	}

	levelFile = _assetLocation + "Scenes/" + levelFile + "c";

	FileSystem::BaseFile* loadFile = FileSystem::FileManager::Instance()->GetFile(levelFile);

	if (loadFile == 0)
		return false;

	loadFile->Open(FileSystem::Read, FileSystem::Binary);

	//object container
	std::vector<BodyObject> bodyObjects;

	//load body objects and info
	int number = 0;
	loadFile->Read(&number, sizeof(int), 1);

	//save info of each body
	for (int i = 0; i < number; i++)
	{
		BodyObject bodyObject;
		memset(&bodyObject.info, 0, sizeof(BodyObjectInfo));

		//load info struct
		loadFile->Read(&bodyObject.info, sizeof(BodyObjectInfo), 1);

		//save points
		loadFile->Read(&bodyObject.points, sizeof(int), 1);
		bodyObject.bodyPoints = new BodyPoint[bodyObject.points];
		loadFile->Read(bodyObject.bodyPoints, sizeof(BodyPoint), bodyObject.points);

		//save springs
		loadFile->Read(&bodyObject.springs, sizeof(int), 1);
		bodyObject.bodySprings = new BodySpring[bodyObject.springs];
		loadFile->Read(bodyObject.bodySprings, sizeof(BodySpring), bodyObject.springs);

		//save polygons
		loadFile->Read(&bodyObject.polygons, sizeof(int), 1);
		bodyObject.bodyPolygons = new BodyPolygon[bodyObject.polygons];
		loadFile->Read(bodyObject.bodyPolygons, sizeof(BodyPolygon), bodyObject.polygons);

		bodyObjects.push_back(bodyObject);
	}

	//object count
	int objectsCount = 0;
	loadFile->Read(&objectsCount, sizeof(int), 1);

	//objects
	GameObject* objectsArray = new GameObject[objectsCount];
	loadFile->Read(objectsArray, sizeof(GameObject), objectsCount);

	//create game level bodies
	for (int i = 0; i < objectsCount; i++)
	{
		ObjectInfo bodyInfo;

		//name
		bodyInfo.name = objectsArray[i].name;

		//position
		bodyInfo.posX = objectsArray[i].posX;
		bodyInfo.posY = objectsArray[i].posY;

		//angle
		bodyInfo.angle = objectsArray[i].angle;

		//scale
		bodyInfo.scaleX = objectsArray[i].scaleX;
		bodyInfo.scaleY = objectsArray[i].scaleY;

		//material
		bodyInfo.material = objectsArray[i].material;

		//get body info
		int bodyNumber = -1;
		for (size_t i = 0; i < bodyObjects.size(); i++)
		{
			if (bodyInfo.name.compare(bodyObjects[i].info.name) == 0)
			{
				bodyNumber = i;
				break;
			}
		}

		//create body
		LevelSoftBody * gameBody = new LevelSoftBody(&bodyObjects[bodyNumber], world, bodyInfo);

		//ballon and tire item
		if (gameBody->GetName() == "itemballoon" || gameBody->GetName() == "itemstick")
		{
			gameBody->SetVisible(false);
		}

		gameBodies.push_back(gameBody);

		Body* body = gameBody->GetBody();

		if (objectsArray[i].isPlatform)
		{
			Vector2 end(bodyInfo.posX, bodyInfo.posY);

			end.X += objectsArray[i].platformOffsetX;
			end.Y += objectsArray[i].platformOffsetY;

			float seconds = objectsArray[i].platformSecondsPerLoop;
			float offset = objectsArray[i].platformStartOffset;

			gameBody->AddKinematicControl(new KinematicPlatform(body, Vector2(bodyInfo.posX, bodyInfo.posY), end, seconds, offset));

			if (offset != 0.0f)
			{
				Vector2 newPos = Vector2(bodyInfo.posX, bodyInfo.posY).lerp(end, 0.5f + (sinf((PI / 2.0f) + (PI*2.0*offset))*0.5f));
				body->setPositionAngle(newPos, VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY));
			}
		}

		if (objectsArray[i].isMotor)
		{
			float rps = objectsArray[i].motorRadiansPerSecond;
			gameBody->AddKinematicControl(new KinematicMotor(body, rps));
		}

		body->updateAABB(0.0f, true);

		worldLimits.expandToInclude(body->getAABB().Min);
		worldLimits.expandToInclude(body->getAABB().Max);

		// finalize this one!
		gameBody->Finalize();
	}

    //remove loaded data
    delete[] objectsArray;

    //delete body objects
    for (size_t i = 0; i < bodyObjects.size(); i++)
    {
        delete[] bodyObjects[i].bodyPoints;
        delete[] bodyObjects[i].bodyPolygons;
        delete[] bodyObjects[i].bodySprings;
    }

	//car info
	char carName[64];
	memset(carName, 0, 64);

	loadFile->Read(carName, sizeof(char), 64);

	//car position
	loadFile->Read(&_carPos.X, sizeof(float), 1);
	loadFile->Read(&_carPos.Y, sizeof(float), 1);

	if (!carFileName.empty())
		_car = new Car(carFileName, world, _carPos, 2, 3);

	//Settings
	loadFile->Read(&finishX, sizeof(float), 1);
	loadFile->Read(&finishY, sizeof(float), 1);
	loadFile->Read(&fallLine, sizeof(float), 1);

	//very important to set this at the end...
	world->setWorldLimits(worldLimits.Min, worldLimits.Max);

	loadFile->Close();
	delete loadFile;

	return true;
}


bool LevelManager::LoadLevel(World *world, std::string levelName, std::string carFileName)
{
	worldLimits.clear();
	bool found = false;

	std::string levelFile = "";

	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			levelFile = _levels[i].file;
			found = true;
		}
	}

	if (!found)
	{
		levelFile = levelName;
	}

	levelFile = _assetLocation +"Scenes/" + levelFile;

	//loac main level file
	FileSystem::BaseFile* file = FileSystem::FileManager::Instance()->GetFile(levelFile);

	if (file == 0)
		return false;

	file->Open(FileSystem::Read, FileSystem::Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	int bodyNumber = 0;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return false;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem)
	{
		//Error can't find root
		return false;
	}

	hRoot = TiXmlHandle(pElem);

	//look for some objects ;)
	TiXmlElement* ObjectNode = hRoot.FirstChild("Objects").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		ObjectInfo bodyInfo;

		//name
		bodyInfo.name = ObjectNode->Attribute("name");

		//position
		bodyInfo.posX = (float)std::stof(ObjectNode->Attribute("posX"));
		bodyInfo.posY = (float)std::stof(ObjectNode->Attribute("posY"));

		//angle
		bodyInfo.angle = (float)std::stof(ObjectNode->Attribute("angle"));

		//scale
		bodyInfo.scaleX = (float)std::stof(ObjectNode->Attribute("scaleX"));
		bodyInfo.scaleY = (float)std::stof(ObjectNode->Attribute("scaleY"));

		//material
		bodyInfo.material = 0.0f;
		if (ObjectNode->Attribute("material") != NULL)
		{
			bodyInfo.material = atoi(ObjectNode->Attribute("material"));
		}

		//main path to scene file
		std::string scenePath = GetPathName(levelFile);

		//Add this object
		std::string name = scenePath + "/" + bodyInfo.name + ".softbody";

		bool znalazlem = false;
		int numerek = 0;

		for (unsigned int i = 0; i < gameBodiesNames.size(); i++)
		{
			if (name.compare(gameBodiesNames[i]) == 0)
			{
				znalazlem = true;
				numerek = i;
			}
		}

		if (znalazlem == false)
		{
			gameBodiesNames.push_back(name);
			gameBodiesNumbers.push_back(bodyNumber);
		}

		LevelSoftBody * gameBody;

		if (znalazlem == false)
		{
			gameBody = new LevelSoftBody(name, world, bodyInfo);
		}
		else
		{
			gameBody = new LevelSoftBody(gameBodies[gameBodiesNumbers[numerek]], world, bodyInfo);
		}

		//ballon and tire item
		if (gameBody->GetName() == "itemballoon" || gameBody->GetName() == "itemstick")
		{
			gameBody->SetVisible(false);
		}

		gameBodies.push_back(gameBody);
		bodyNumber++;

		Body* body = gameBody->GetBody();

		//Add kinematic controls to the body
		TiXmlElement* KinematicElements = ObjectNode->FirstChildElement();
		for (KinematicElements; KinematicElements; KinematicElements = KinematicElements->NextSiblingElement())
		{

			TiXmlElement* Element = KinematicElements->FirstChild()->ToElement();
			while (Element != NULL)
			{
				const char* sKinematic;
				if (Element->Value() != NULL)
				{
					sKinematic = Element->Value();

					if (strcmp(sKinematic, "PlatformMotion") == 0)
					{
						Vector2 end(bodyInfo.posX, bodyInfo.posY);

						end.X += std::stof(Element->Attribute("offsetX"));
						end.Y += std::stof(Element->Attribute("offsetY"));


						float seconds = std::stof(Element->Attribute("secondsPerLoop"));

						float offset = 0.0f;

						if (Element->Attribute("startOffset") != NULL)
						{
							offset = std::stof(Element->Attribute("startOffset"));
						}

						gameBody->AddKinematicControl(new KinematicPlatform(body, Vector2(bodyInfo.posX, bodyInfo.posY), end, seconds, offset));

						if (offset != 0.0f)
						{
							Vector2 newPos = Vector2(bodyInfo.posX, bodyInfo.posY).lerp(end, 0.5f + (sinf((PI / 2.0f) + (PI*2.0*offset))*0.5f));
							//Vector2 newPos = Vector2::Lerp(Vector2(bodyInfo.posX,bodyInfo.posY),end,0.5f + (Vector2::sinf((Vector2::PI/2.0f)+(Vector2::PI*2*offset))*0.5f));
							body->setPositionAngle(newPos, VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY));
						}

					}
					if (strcmp(sKinematic, "Motor") == 0)
					{
						float rps = std::stof(Element->Attribute("radiansPerSecond"));
						gameBody->AddKinematicControl(new KinematicMotor(body, rps));
					}
				}
				Element = Element->NextSiblingElement();
			}
		}

		body->updateAABB(0.0f, true);

		worldLimits.expandToInclude(body->getAABB().Min);
		worldLimits.expandToInclude(body->getAABB().Max);

		// finalize this one!
		gameBody->Finalize();
	}

	//load car
	TiXmlElement* CarNode = hRoot.FirstChild("Car").Element();
	const char * carName = CarNode->Attribute("name");

	_carPos.X = std::stof(CarNode->Attribute("posX"));
	_carPos.Y = std::stof(CarNode->Attribute("posY"));

	if(!carFileName.empty())
		_car = new Car(carFileName,world, _carPos,2,3);

	//Settings
	TiXmlElement* SettingsNode = hRoot.FirstChild("Settings").Element();
	finishX = std::stof(SettingsNode->Attribute("finishX"));
	finishY = std::stof(SettingsNode->Attribute("finishY"));
	fallLine = std::stof(SettingsNode->Attribute("fallLine"));

	//very important to set this at the end...
	world->setWorldLimits(worldLimits.Min, worldLimits.Max);

	return true;
}

bool LevelManager::ClearLevel(World *world)
{
	world->killing();

	for (unsigned int i = 0; i < gameBodies.size(); i++)
	{
		world->removeBody(gameBodies[i]->GetBody());
	}

	for (unsigned int i = 0; i < gameBodies.size(); i++)
	{
		delete gameBodies[i];
	}

	world->removeAllBodies();

	gameBodies.clear();
	gameBodiesNames.clear();
	gameBodiesNumbers.clear();

	delete _car;

	return true;
}

std::string LevelManager::GetLevelFile(std::string levelName)
{
	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			return _levels[i].file;
		}
	}

	return "";
}

void LevelManager::LoadScores(std::string fileName)
{
	//loac main level file
	FileSystem::BaseFile* file = FileSystem::FileManager::Instance()->GetFile(fileName,true);

	if (file == 0)
		return;

	file->Open(FileSystem::Read, FileSystem::Binary);

	if (!file->Exist())
	{
		delete file;
		return;
	}
		

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	int bodyNumber = 0;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Levels").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		std::string levelName = ObjectNode->Attribute("name");

		for (size_t i = 0; i < _levels.size(); i++)
		{
			if (_levels[i].name == levelName)
			{
				_levels[i].time = std::stof(ObjectNode->Attribute("score"));
				_levels[i].jump = std::stof(ObjectNode->Attribute("jump"));
			}
		}
	}
}

void LevelManager::SaveScores(std::string fileName)
{
	std::string saveFile = FileSystem::FileManager::Instance()->GetSaveDirPath() + fileName;

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Levels");
	doc.LinkEndChild(root);

	for (size_t i = 0; i < _levels.size(); i++)
	{
		TiXmlElement * cxn = new TiXmlElement("Level");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", _levels[i].name.c_str());
		cxn->SetDoubleAttribute("score", _levels[i].time);
		cxn->SetDoubleAttribute("jump", _levels[i].jump);
	}

	doc.SaveFile(saveFile.c_str());
}

float LevelManager::GetTime(std::string levelName)
{
	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			return _levels[i].time;
		}
	}

	return 999.0f;
}

float LevelManager::GetJump(std::string levelName)
{
	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			return _levels[i].jump;
		}
	}

	return 0.0f;
}

void LevelManager::SetTime(std::string levelName, float time)
{
	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			_levels[i].time = time;
			return;
		}
	}
}

void LevelManager::SetJump(std::string levelName, float jump)
{
	for (size_t i = 0; i < _levels.size(); i++)
	{
		if (_levels[i].name == levelName)
		{
			_levels[i].jump = jump;
			return;
		}
	}
}

Car* LevelManager::GetCar()
{
	return _car;
}

Vector2 LevelManager::GetCarStartPos()
{
	return _carPos;
}

std::vector<LevelSoftBody*> LevelManager::GetLevelBodies()
{
	return gameBodies;
}

AABB LevelManager::GetWorldLimits()
{
	return worldLimits;
}

Vector2 LevelManager::GetWorldCenter()
{
	float x = (((worldLimits.Max.X - worldLimits.Min.X) / 2) + worldLimits.Min.X);
	float y = (((worldLimits.Max.Y - worldLimits.Min.Y) / 2) + worldLimits.Min.Y);

	return Vector2(x, y);
}

Vector2 LevelManager::GetWorldSize()
{
	float x = (worldLimits.Max.X - worldLimits.Min.X);
	float y = (worldLimits.Max.Y - worldLimits.Min.Y);

	return Vector2(x, y);
}

Vector2 LevelManager::GetLevelTarget()
{
	return Vector2(finishX, finishY);
}

float LevelManager::GetLevelLine()
{
	return fallLine;
}

void LevelManager::ReadLevelData(std::string levelName)
{
	//object container
	std::vector<GameObject> gameObjects;
	std::vector<BodyObject> bodyObjects;

	//level location
	std::string levelFile = _assetLocation + "Scenes/" + levelName;

	//loac main level file
	FileSystem::BaseFile* file = FileSystem::FileManager::Instance()->GetFile(levelFile);

	if (file == 0)
		return;

	file->Open(FileSystem::Read, FileSystem::Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	int bodyNumber = 0;

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
		//Error can't find root
		return;
	}

	hRoot = TiXmlHandle(pElem);

	//look for some objects ;)
	TiXmlElement* ObjectNode = hRoot.FirstChild("Objects").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		GameObject gameObject;
		memset(&gameObject, 0, sizeof(GameObject));

		//name
		strcpy(gameObject.name, ObjectNode->Attribute("name"));

		//position
		gameObject.posX = (float)std::stof(ObjectNode->Attribute("posX"));
		gameObject.posY = (float)std::stof(ObjectNode->Attribute("posY"));

		//angle
		gameObject.angle = (float)std::stof(ObjectNode->Attribute("angle"));

		//scale
		gameObject.scaleX = (float)std::stof(ObjectNode->Attribute("scaleX"));
		gameObject.scaleY = (float)std::stof(ObjectNode->Attribute("scaleY"));

		//material
		gameObject.material = 0.0f;
		if (ObjectNode->Attribute("material") != NULL)
		{
			gameObject.material = atoi(ObjectNode->Attribute("material"));
		}

		gameObject.isMotor = false;
		gameObject.isPlatform = false;

		gameObject.platformOffsetX = 0;
		gameObject.platformOffsetY = 0;
		gameObject.platformSecondsPerLoop = 0;
		gameObject.platformStartOffset = 0;
		gameObject.motorRadiansPerSecond = 0;

		//Add kinematic controls to the body
		TiXmlElement* KinematicElements = ObjectNode->FirstChildElement();
		for (KinematicElements; KinematicElements; KinematicElements = KinematicElements->NextSiblingElement())
		{

			TiXmlElement* Element = KinematicElements->FirstChild()->ToElement();
			while (Element != NULL)
			{
				const char* sKinematic;
				if (Element->Value() != NULL)
				{
					sKinematic = Element->Value();

					if (strcmp(sKinematic, "PlatformMotion") == 0)
					{
						gameObject.isPlatform = true;

						gameObject.platformOffsetX = std::stof(Element->Attribute("offsetX"));
						gameObject.platformOffsetY = std::stof(Element->Attribute("offsetY"));


						gameObject.platformSecondsPerLoop = std::stof(Element->Attribute("secondsPerLoop"));

						gameObject.platformStartOffset = 0.0f;

						if (Element->Attribute("startOffset") != NULL)
						{
							gameObject.platformStartOffset = std::stof(Element->Attribute("startOffset"));
						}

					}
					if (strcmp(sKinematic, "Motor") == 0)
					{
						gameObject.isMotor = true;
						gameObject.motorRadiansPerSecond = std::stof(Element->Attribute("radiansPerSecond"));
					}
				}
				Element = Element->NextSiblingElement();
			}
		}

		//get body data
		//Add this object
		std::string name = _assetLocation + "Scenes/" + gameObject.name + ".softbody";

		bool haveBody = false;

		for (unsigned int i = 0; i < gameBodiesNames.size(); i++)
		{
			if (name.compare(gameBodiesNames[i]) == 0)
			{
				haveBody = true;
			}
		}

		if (haveBody == false)
		{
			gameBodiesNames.push_back(name);

			//load body
			BodyObject bodyObject = ReadBodyData(name);

			//copy name
			strcpy(bodyObject.info.name, gameObject.name);

			bodyObjects.push_back(bodyObject);
		}

		gameObjects.push_back(gameObject);
	}

	//load car
	TiXmlElement* CarNode = hRoot.FirstChild("Car").Element();

	char carName[64];
	memset(carName, 0, 64);
	strcpy(carName, CarNode->Attribute("name"));

	_carPos.X = std::stof(CarNode->Attribute("posX"));
	_carPos.Y = std::stof(CarNode->Attribute("posY"));

	//Settings
	TiXmlElement* SettingsNode = hRoot.FirstChild("Settings").Element();
	finishX = std::stof(SettingsNode->Attribute("finishX"));
	finishY = std::stof(SettingsNode->Attribute("finishY"));
	fallLine = std::stof(SettingsNode->Attribute("fallLine"));

	//test save file
	{
		//loac main level file
		levelFile = levelFile + "c";
		FileSystem::BaseFile* saveFile = FileSystem::FileManager::Instance()->GetFile(levelFile);

		if (saveFile == 0)
			return;

		saveFile->Open(FileSystem::Write, FileSystem::Binary);

		//save body objects and info
		int number = bodyObjects.size();
		saveFile->Write(&number, sizeof(int), 1);

		//save info of each body
		for (int i = 0; i < number; i++)
		{
			//save info struct
			saveFile->Write(&bodyObjects[i].info, sizeof(BodyObjectInfo), 1);

			//save points
			saveFile->Write(&bodyObjects[i].points, sizeof(int), 1);
			saveFile->Write(bodyObjects[i].bodyPoints, sizeof(BodyPoint), bodyObjects[i].points);

			//save springs
			saveFile->Write(&bodyObjects[i].springs, sizeof(int), 1);
			saveFile->Write(bodyObjects[i].bodySprings, sizeof(BodySpring), bodyObjects[i].springs);

			//save polygons
			saveFile->Write(&bodyObjects[i].polygons, sizeof(int), 1);
			saveFile->Write(bodyObjects[i].bodyPolygons, sizeof(BodyPolygon), bodyObjects[i].polygons);
		}

		//save game objects info
		GameObject* objectsArray = new GameObject[gameObjects.size()];

		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			objectsArray[i] = gameObjects[i];
		}

		//objects count
		number = gameObjects.size();
		saveFile->Write(&number, sizeof(int), 1);

		//write all objects info
		saveFile->Write(objectsArray, sizeof(GameObject), number);

		//level info
		saveFile->Write(carName, sizeof(char), 64);

		//car position
		saveFile->Write(&_carPos.X, sizeof(float), 1);
		saveFile->Write(&_carPos.Y, sizeof(float), 1);

		//Settings
		saveFile->Write(&finishX, sizeof(float), 1);
		saveFile->Write(&finishY, sizeof(float), 1);
		saveFile->Write(&fallLine, sizeof(float), 1);

		//close file
		saveFile->Close();
		delete saveFile;
	}

	//test load
	{
		//clear containers
		bodyObjects.clear();
		gameObjects.clear();

		FileSystem::BaseFile* loadFile = FileSystem::FileManager::Instance()->GetFile(levelFile);

		if (loadFile == 0)
			return;

		loadFile->Open(FileSystem::Read, FileSystem::Binary);

		//load body objects and info
		int number = 0;
		loadFile->Read(&number, sizeof(int), 1);

		if (number == 0)
		{
			number = 0;
		}

		//save info of each body
		for (int i = 0; i < number; i++)
		{
			BodyObject bodyObject;
			memset(&bodyObject.info, 0, sizeof(BodyObjectInfo));

			//load info struct
			loadFile->Read(&bodyObject.info, sizeof(BodyObjectInfo), 1);

			//save points
			loadFile->Read(&bodyObject.points, sizeof(int), 1);
			bodyObject.bodyPoints = new BodyPoint[bodyObject.points];
			loadFile->Read(bodyObject.bodyPoints, sizeof(BodyPoint), bodyObject.points);

			//save springs
			loadFile->Read(&bodyObject.springs, sizeof(int), 1);
			bodyObject.bodySprings = new BodySpring[bodyObject.springs];
			loadFile->Read(bodyObject.bodySprings, sizeof(BodySpring), bodyObject.springs);

			//save polygons
			loadFile->Read(&bodyObject.polygons, sizeof(int), 1);
			bodyObject.bodyPolygons = new BodyPolygon[bodyObject.polygons];
			loadFile->Read(bodyObject.bodyPolygons, sizeof(BodyPolygon), bodyObject.polygons);

			bodyObjects.push_back(bodyObject);
		}

		//object count
		number = 0;
		loadFile->Read(&number, sizeof(int), 1);

		//objects
		GameObject* objectsArray = new GameObject[number];
		loadFile->Read(objectsArray, sizeof(GameObject), number);

		//level info
		char carName[64];
		memset(carName, 0, 64);

		loadFile->Read(carName, sizeof(char), 64);

		//car position
		loadFile->Read(&_carPos.X, sizeof(float), 1);
		loadFile->Read(&_carPos.Y, sizeof(float), 1);

		//Settings
		loadFile->Read(&finishX, sizeof(float), 1);
		loadFile->Read(&finishY, sizeof(float), 1);
		loadFile->Read(&fallLine, sizeof(float), 1);

		loadFile->Close();
		delete loadFile;
	}
}

BodyObject LevelManager::ReadBodyData(std::string bodyName)
{
	//loac main level file
	FileSystem::BaseFile* file = FileSystem::FileManager::Instance()->GetFile(bodyName);

	if (file == 0)
		return BodyObject();

	file->Open(FileSystem::Read, FileSystem::Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return BodyObject();
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem)
	{
		//Error - Can't find root :/
		return BodyObject();
	}

	BodyObject bodyObject;

	//reset object
	memset(&bodyObject, 0, sizeof(BodyObject));
	memset(&bodyObject.info, 0, sizeof(BodyObjectInfo));

	bodyObject.info.massPerPoint = std::stof(pElem->Attribute("massPerPoint"));
	bodyObject.info.edgeK = std::stof(pElem->Attribute("edgeK"));
	bodyObject.info.edgeDamping = std::stof(pElem->Attribute("edgeDamping"));

	//load color of the body
	if (pElem->Attribute("colorR") != NULL)
	{
		bodyObject.info.colorR = std::stof(pElem->Attribute("colorR"));
		bodyObject.info.colorG = std::stof(pElem->Attribute("colorG"));
		bodyObject.info.colorB = std::stof(pElem->Attribute("colorB"));
	}

	const char* sKinematic, *sShapeMatching;
	if (pElem->Attribute("kinematic") != NULL)
	{
		sKinematic = pElem->Attribute("kinematic");
		if (strcmp(sKinematic, "True") == 0)
			bodyObject.info.isKinematic = true;
		else
			bodyObject.info.isKinematic = false;
	}

	//shape matching
	if (pElem->Attribute("shapeMatching") != NULL)
	{
		sShapeMatching = pElem->Attribute("shapeMatching");
		if (strcmp(sShapeMatching, "True") == 0)
		{
			bodyObject.info.shapeMatching = true;
			bodyObject.info.shapeK = std::stof(pElem->Attribute("shapeK"));
			bodyObject.info.shapeDamping = std::stof(pElem->Attribute("shapeDamping"));
		}
	}

	//end of first section
	hRoot = TiXmlHandle(pElem);

	//pressure
	pElem = hRoot.FirstChild("Pressure").Element();
	if (pElem)
	{
		bodyObject.info.pressureized = true;
		bodyObject.info.pressure = std::stof(pElem->Attribute("amount"));
	}

	//points
	std::vector<BodyPoint> bodyPoints;

	TiXmlElement* PointNode = hRoot.FirstChild("Points").FirstChild().Element();
	for (PointNode; PointNode; PointNode = PointNode->NextSiblingElement())
	{
		BodyPoint bodyPoint;
		bodyPoint.x = std::stof(PointNode->Attribute("x"));
		bodyPoint.y = std::stof(PointNode->Attribute("y"));

		if (PointNode->Attribute("mass") != NULL)
		{
			bodyPoint.mass = std::stof(PointNode->Attribute("mass"));
		}
		else
		{
			bodyPoint.mass = -1;
		}

		bodyPoints.push_back(bodyPoint);
	}

	bodyObject.points = bodyPoints.size();
	bodyObject.bodyPoints = new BodyPoint[bodyObject.points];
	for (size_t i = 0; i < bodyObject.points; i++)
	{
		bodyObject.bodyPoints[i] = bodyPoints[i];
	}

	//Springs
	std::vector<BodySpring> bodySprings;

	TiXmlElement* SpingNode = hRoot.FirstChild("Springs").FirstChild().Element();
	for (SpingNode; SpingNode; SpingNode = SpingNode->NextSiblingElement())
	{
		BodySpring bodySpring;

		bodySpring.pt1 = atoi(SpingNode->Attribute("pt1"));
		bodySpring.pt2 = atoi(SpingNode->Attribute("pt2"));
		bodySpring.k = std::stof(SpingNode->Attribute("k"));
		bodySpring.damp = std::stof(SpingNode->Attribute("damp"));

		bodySprings.push_back(bodySpring);
	}

	bodyObject.springs = bodySprings.size();
	bodyObject.bodySprings = new BodySpring[bodyObject.springs];
	for (int i = 0; i < bodyObject.springs; i++)
	{
		bodyObject.bodySprings[i] = bodySprings[i];
	}

	//Polygons
	std::vector<BodyPolygon> bodyPolygons;

	TiXmlElement* PolygonNode = hRoot.FirstChild("Polygons").FirstChild().Element();
	for (PolygonNode; PolygonNode; PolygonNode = PolygonNode->NextSiblingElement())
	{
		BodyPolygon bodyPolygon;

		bodyPolygon.x = atoi(PolygonNode->Attribute("pt0"));
		bodyPolygon.y = atoi(PolygonNode->Attribute("pt1"));
		bodyPolygon.z = atoi(PolygonNode->Attribute("pt2"));

		bodyPolygons.push_back(bodyPolygon);
	}

	bodyObject.polygons = bodyPolygons.size();
	bodyObject.bodyPolygons = new BodyPolygon[bodyObject.polygons];
	for (int i = 0; i < bodyObject.polygons; i++)
	{
		bodyObject.bodyPolygons[i] = bodyPolygons[i];
	}

	return bodyObject;
}