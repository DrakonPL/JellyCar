#ifndef JellyMenuBetter_H
#define JellyMenuBetter_H

#include <Andromeda/System/GameState.h>
#include <Andromeda/System/GameManager.h>

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/TextureManager.h>
#include <Andromeda/Graphics/Sprite.h>

#include <Andromeda/Graphics/FrameBufferObject.h>

#include <Andromeda/Input/InputManager.h>

#include <Andromeda/Graphics/Text/TextureAtlas.h>
#include <Andromeda/Graphics/Text/TexturedFont.h>

#include <Andromeda/System/Timer.h>

#include "JellyPhysics/JellyPhysics.h"

#include "../Car/Car.h"
#include "../Levels/LevelManager.h"
#include "../Levels/LevelSoftBody.h"

#include "../Utils/InputHelper.h"
#include "../Utils/AudioHelper.h"
#include "JellyCore.h"
#include "JellyGame.h"


using namespace Andromeda::System;
using namespace Andromeda::Graphics;
using namespace Andromeda::Input;
using namespace JellyPhysics;



class JellyMenuBetter : public GameState
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
	TexturedFont* _smallFont;
	TexturedFont* _titleFont;

	//shader
	Shader* _shader;
	Sprite* _backSprite;
	Sprite* _levelImage;

	glm::mat4 _projection;
	glm::mat4 _jellyProjection;

	//timer
	Timer* _timer;
	float _dt;

	//input
	InputHelper* _inputHelper;

	//audio
	AudioHelper* _audioHelper;
	
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

	//
	//jelly physics
	World* _world;

	//car
	Car* _car;

	//level objects
	std::vector<LevelSoftBody*> _gameBodies;

    JellyGame* _jellyGame;

public:

	JellyMenuBetter(JellyCore* core);

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