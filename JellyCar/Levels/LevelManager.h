#ifndef LevelManagr_H
#define LevelManagr_H

#include "JellyPhysics/JellyPhysics.h"
using namespace JellyPhysics;

#include "LevelSoftBody.h"
#include "LevelInfo.h"
#include "SkinInfo.h"
#include "../Car/Car.h"

#include "SimpleStruct/BodyObject.h"

#include <map>

class LevelManager
{
private:

	std::string _assetLocation;

	Car* _car;
	Vector2 _carPos;

	std::vector<LevelInfo> _levels;
	std::vector<SkinInfo> _skins;
	std::map<std::string, Texture*> _images;

	std::vector<LevelSoftBody*> gameBodies;
	std::vector<std::string> gameBodiesNames;
	std::vector<int> gameBodiesNumbers;

	float finishX;
	float finishY;
	float fallLine;

	//for map limits
	AABB worldLimits;

	std::string GetPathName(std::string& fileName);

	void LoadCarImage(std::string& imageName);
	void LoadLevelImage(std::string& imageName);

public:

	~LevelManager();

	void SetAssetsLocation(std::string location);

	void LoadAllScenes(std::string fileName);
	void LoadCarSkins(std::string fileName);

	std::vector<LevelInfo> GetLoadedLevels() { return _levels; }

	void InitPhysic(World *world);

	bool LoadLevel(World *world, std::string levelName, std::string carFileName);	
	bool LoadCompiledLevel(World *world, std::string levelName, std::string carFileName);
	bool ClearLevel(World *world);


	std::string GetLevelFile(std::string levelName);

	void LoadScores(std::string fileName);
	void SaveScores(std::string fileName);

	float GetTime(std::string levelName);
	float GetJump(std::string levelName);

	void SetTime(std::string levelName,float time);
	void SetJump(std::string levelName,float jump);

	Texture* GetThumb(std::string levelName);
	Texture* GetCarImage(std::string carImage);

	Car* GetCar();
	Vector2 GetCarStartPos();

	std::vector<std::string> GetScenes();
	std::vector<SkinInfo> GetCarSkins();
	std::vector<LevelSoftBody*> GetLevelBodies();

	AABB GetWorldLimits();

	Vector2 GetWorldCenter();
	Vector2 GetWorldSize();

	Vector2 GetLevelTarget();
	float GetLevelLine();

	//serialization tests
	void ReadLevelData(std::string levelName);
	BodyObject ReadBodyData(std::string bodyName);
};

#endif