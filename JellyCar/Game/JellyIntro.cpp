#include "JellyIntro.h"
#include "JellyOptions.h"
#include "JellyMenuBetter.h"

#include "../Utils/JellyHelper.h"

#include <iostream> 
#include <stdio.h> 
#include <string> 
#include <sstream> 

void JellyIntro::Init()
{
	_renderManager = RenderManager::Instance();
	_shaderManager = ShaderManager::Instance();
	_textureManager = TextureManager::Instance();

	_renderManager->SetClearColor(0xffffffff);

	_core = new JellyCore();
	_core->Init();

	//font
	_menuFont = _core->_menuFont;
	_titleFont = _core->_titleFont;
	_smallFont = _core->_smallFont;


	//load shader
	_shader = _shaderManager->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	_projection = glm::ortho(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f, -1.0f, 1.0f);

	_renderManager->SetDepth(false);

	//input
	_inputHelper = InputHelper::Instance();

	JellyHellper::Instance()->LoadShaders();

	//init audio helper
	_audioHelper = AudioHelper::Instance();
	//_audioHelper->PlayMusic();
	_audioHelper->PlayFastEngine();

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadCarSkins("car_skins.xml");

	//background paper sprite
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_logoSprite = new Sprite("logo", "Assets/Jelly/Texture/logo.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	//load menu level
	_world = new World();

	_levelManager->InitPhysic(_world);
	//_levelManager->LoadLevel(_world, "intro.scene", "Assets/Jelly/car_and_truck.car");
	_levelManager->LoadCompiledLevel(_world, "intro.scene", "Assets/Jelly/car_and_truck.car");

	_levelTarget = _levelManager->GetLevelTarget();

	//car
	_car = _levelManager->GetCar();
	_car->SetChassisTextures(_levelManager->GetCarImage(_levelManager->GetCarSkins()[0].chassisSmall), _levelManager->GetCarImage(_levelManager->GetCarSkins()[0].chassisBig));
	_car->SetTireTextures(_levelManager->GetCarImage(_levelManager->GetCarSkins()[0].tireSmall), _levelManager->GetCarImage(_levelManager->GetCarSkins()[0].tireBig));

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 0, 0 + 18.2f, -1.0f, 1.0f);

	//timer
	_dt = 0;
	_timer = new Timer();

	_end = false;
}

void JellyIntro::Enter()
{

}

void JellyIntro::CleanUp()
{
	if (_levelManager != 0)
	{
		//clear level
		_levelManager->ClearLevel(_world);

		//remove level manager
		delete _levelManager;
	}

	//remove physic world
	delete _world;
	_gameBodies.clear();

	delete _backSprite;
	delete _inputHelper;
	delete _core;
	delete _logoSprite;
	delete _audioHelper;

	_shaderManager->RemoveAll();
	_textureManager->RemoveAll();
}

void JellyIntro::Pause()
{

}

void JellyIntro::Resume()
{

}

void JellyIntro::GamePause()
{

}

void JellyIntro::GameResume()
{

}

void JellyIntro::HandleEvents(GameManager* manager)
{
	_inputHelper->Update();
}

void JellyIntro::Update(GameManager* manager)
{
	if (_end)
	{
		manager->PopState();
		return;
	}

	_car->setTorque(1);

	//Update physic
	for (int i = 0; i < 6; i++)
	{
		_world->update(0.004f);

		for (size_t i = 0; i < _gameBodies.size(); i++)
			_gameBodies[i]->Update(0.004f);

		_car->clearForces();
		_car->update(0.004f);
	}

	//reset car position
	if (_car->getPosition().Y < _levelManager->GetLevelLine())
	{
		Vector2 pos = _levelManager->GetCarStartPos();
		Vector2 scale = Vector2(1.0f, 1.0f);

		_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(0)->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(1)->setPositionAngle(pos, 0.0f, scale);
	}

	bool enterMenu = false;

	//end of scene
	if (_car->getChassisBody()->getAABB().contains(_levelTarget))
	{
		if (_car->getChassisBody()->contains(_levelTarget))
		{
			enterMenu = true;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		if (_car->getTire(i)->getAABB().contains(_levelTarget))
		{
			if (_car->getTire(i)->contains(_levelTarget))
			{
				enterMenu = true;
			}
		}
	}

	if (enterMenu)
	{
		_audioHelper->StopEngineSound();

		JellyMenuBetter* jelly = new JellyMenuBetter(_core);
		//JellyOptions* jelly = new JellyOptions(_core);
		jelly->Init();

		manager->PushState(jelly);
		_end = true;
	}
}

void JellyIntro::Draw(GameManager* manager)
{
	if (_end)
		return;

	//start frame
	_renderManager->StartFrame();

	//clear screen
	_renderManager->ClearScreen();

	//paper background
    {
        int backWidth = _backSprite->GetTexture()->GetWidth();
        int backHeight = _backSprite->GetTexture()->GetHeight();

        int columns = ceil((float)_renderManager->GetWidth() / (float)backWidth);
        int rows = ceil((float)_renderManager->GetHeight() / (float)backHeight);

        _backSprite->SetScale(glm::vec2(1.0f, 1.0f));

        for (size_t y = 0; y < rows; y++)
        {
            for (size_t x = 0; x < columns; x++)
            {
                int posx = (backWidth * x) + (backWidth / 2);
                int posy = (backHeight * y) + (backHeight / 2);

                _backSprite->SetPosition(glm::vec2(posx, posy));
                _backSprite->Draw(_projection);
            }
        }
    }

	_logoSprite->SetPosition(glm::vec2(_renderManager->GetWidth() / 2, 200));
	_logoSprite->Draw(_projection);
	//menu level drawing
	{
		for (size_t i = 0; i < _gameBodies.size(); i++)
			_gameBodies[i]->Draw(_jellyProjection);

		_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.3f + 0, 0 + 18.1f, -1.0f, 1.0f);

		_car->Draw(_jellyProjection);
	}

	//draw main text
	_menuFont->Draw(_projection);
	_titleFont->Draw(_projection);
	_smallFont->Draw(_projection);

	//end frame
	_renderManager->EndFrame();
}