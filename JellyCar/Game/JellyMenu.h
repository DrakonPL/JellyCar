#ifndef JellyMenu_H
#define JellyMenu_H

#include <Andromeda/System/GameState.h>
#include <Andromeda/System/GameManager.h>

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/GraphicsEnums.h>
#include <Andromeda/Graphics/VertexTypes.h>
#include <Andromeda/Graphics/TextureManager.h>
#include <Andromeda/Graphics/Sprite.h>
#include <Andromeda/Input/KeyboardDevice.h>
#include <Andromeda/Input/InputManager.h>

#include <Andromeda/Graphics/Text/TextureAtlas.h>
#include <Andromeda/Graphics/Text/TexturedFont.h>

#include <Andromeda/System/Timer.h>

#include "JellyPhysics/JellyPhysics.h"

#include "../Car/Car.h"
#include "../Levels/LevelManager.h"
#include "../Levels/LevelSoftBody.h"

#include "InputHelper.h"
#include "JellyCore.h"

using namespace Andromeda::System;
using namespace Andromeda::Graphics;
using namespace Andromeda::Input;
using namespace JellyPhysics;

enum CarGameState
{
	Menu,
	Game
};

class JellyMenu : public GameState
{
private:

	JellyCore* _core;

	//render manager
	RenderManager* _renderManager;

	//shader manager
	ShaderManager* _shaderManager;

	//texture manager
	TextureManager* _textureManager;

	//font
	TexturedFont* _menuFont;
	TexturedFont* _titleFont;

	//shader
	Shader* _shader;
	Sprite* _backSprite;
	Sprite* _sprite;

	glm::mat4 _projection;

	//timer
	Timer* _timer;
	float _dt;

	//input
	InputManager* _inputManager;
	InputHelper* _inputHelper;

	//game
	CarGameState _gameState;
	
	Sprite* _smallChassis;
	Sprite* _bigChassis;
	Sprite* _smallTire;
	Sprite* _bigTire;

	//level data
	LevelManager* _levelManager;
	
	//menu
	int currentPosition;
	int columnStartPosition;
	int positionsInColumn;
	std::vector<std::string> _sceneFiles;

	//car skins names
	int carcurrentPosition;
	int carcolumnStartPosition;
	int carpositionsInColumn;
	std::vector<SkinInfo> _carSkins;
	

public:

	void Init();
	void Enter();
	void CleanUp();

	void Pause();
	void Resume();

	void GamePause();
	void GameResume();

	void HandleEvents(GameManager* manager);
	void Update(GameManager* manager);
	void Draw(GameManager* manager);
};

#endif