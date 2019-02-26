#ifndef JellySplash_H
#define JellySplash_H

#include <Andromeda/System/GameState.h>
#include <Andromeda/System/GameManager.h>
#include <Andromeda/System/Timer.h>

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/TextureManager.h>
#include <Andromeda/Graphics/Sprite.h>

#include <Andromeda/Input/InputManager.h>

using namespace Andromeda::System;
using namespace Andromeda::Graphics;
using namespace Andromeda::Input;

class JellySplash : public GameState
{
private:

	//render manager
	RenderManager* _renderManager;

	//shader manager
	ShaderManager* _shaderManager;

	//texture manager
	TextureManager* _textureManager;

	//shader
	Shader* _shader;
	Sprite* _sprite;
	Texture* _image;

	glm::mat4 _projection;

	bool _end;

	//timer
	Timer* _timer;
	float _dt;
	float _alpha;
	float _splashTimer;

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