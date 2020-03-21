#ifndef JellyOptions_H
#define JellyOptions_H

#include <Andromeda/System/GameState.h>
#include <Andromeda/System/GameManager.h>
#include <Andromeda/System/Timer.h>

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/TextureManager.h>
#include <Andromeda/Graphics/Sprite.h>
#include <Andromeda/Graphics/Text/TexturedFont.h>

#include <Andromeda/Input/InputManager.h>

#include "JellyPhysics/JellyPhysics.h"

#include "../Levels/LevelManager.h"
#include "../Levels/LevelSoftBody.h"

#include "../Utils/InputHelper.h"
#include "../Utils/AudioHelper.h"
#include "JellyCore.h"

using namespace Andromeda::System;
using namespace Andromeda::Graphics;
using namespace Andromeda::Input;
using namespace JellyPhysics;



class Text
{
public:

	float StartPosition;
	std::string Content;

	Text(std::string content, float startPostion)
	{
		Content = content;
		StartPosition = startPostion;
	}
};

class JellyOptions : public GameState
{
public:

	enum JellyOptionsState
	{
		Menu,
		Sound,
		Controls,
		Credits,
		Libs,
		Secret
	};

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

	glm::mat4 _projection;
	glm::mat4 _jellyProjection;
	glm::vec4 _screenBounds;

	//timer
	Timer* _timer;
	float _dt;

	//input
	InputHelper* _inputHelper;

	//audio
	AudioHelper* _audioHelper;
	
	//level data
	LevelManager* _levelManager;
	
	//jelly physics
	World* _world;

	//level objects
	std::vector<LevelSoftBody*> _gameBodies;


	//menu bodies
	std::vector<LevelSoftBody*> _menuBodies;
	int _menuBodySelected;

	//dragging
	float dragX, dragY;
	bool touchF = false;
	int dragPoint = -1;
	Body *dragBody;

	//
	JellyOptionsState _optionsState;

	std::vector<Text> _credits;
	float _creditsPosition = 600.0f;

	std::vector<Text> _libs;
	float _libsPosition = 600.0f;

	//
	Texture* _creditsTexture;
	Texture* _libsTexture;
	Texture* _keyboardTexture;
	Texture* _gamepadTexture;
	Texture* _secretTexture;
	Texture* _volumeTexture;

	//key binding opions
	Sprite* _backSelectSprite;
	int _selctedPosition;
	bool _changeBinding;
	CarAction _selectedAction;

	//sound options
	Sprite* _backRoundSprite;
	Sprite* _leftSprite;
	Sprite* _rightSprite;

	Sprite* _barSprite;
	Sprite* _barBlueSprite;

	int _soundPosition = 0;
	float _alphaScale = 1.0f;
	float _scaleFactor = 0.01f;

	int _optionsCarLevel;
	int _optionsSoundLevel;
	int _optionsMusicLevel;

	//
	std::vector<CarAction> _carActions;
	std::map<CarAction, std::string> _actionTranslation;
	
	void InitCredits();
	void InitLibs();
	void InitActionNames();

public:

	JellyOptions(JellyCore* core);

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