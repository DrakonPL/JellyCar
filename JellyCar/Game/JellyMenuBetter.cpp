#include "JellyMenuBetter.h"

#include "JellySplash.h"
#include "JellyGame.h"
#include "JellyOptions.h"

#include "../Utils/JellyHelper.h"

#include <stdio.h> 
#include <sstream> 

JellyMenuBetter::JellyMenuBetter(JellyCore* core)
{
	_core = core;
}

void JellyMenuBetter::Init()
{
	_renderManager = RenderManager::Instance();
	_shaderManager = ShaderManager::Instance();
	_textureManager = TextureManager::Instance();

	_renderManager->SetClearColor(0xffffffff);

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

	//init audio helper
	_audioHelper = AudioHelper::Instance();
	_audioHelper->PlayMusic();

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadAllScenes("scene_list.xml");
	_levelManager->LoadCarSkins("car_skins.xml");
	_levelManager->LoadScores("JellyScore.xml");

	//get scenes names
	_sceneFiles = _levelManager->GetScenes();

	//get first image
	_levelImage = new Sprite("thumb", _levelManager->GetThumb(_sceneFiles[0]), _shader);
	_levelImage->SetPosition(glm::vec2(_renderManager->GetWidth() / 2, 222));

	//get car skin names
	_carSkins = _levelManager->GetCarSkins();

	//background paper sprite
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

    _jellyGame = nullptr;

	//load menu level
	_world = new World();

	_levelManager->InitPhysic(_world);
	//_levelManager->LoadLevel(_world, "menu.scene", "Assets/Jelly/car_and_truck.car");
	_levelManager->LoadCompiledLevel(_world, "menu.scene", "Assets/Jelly/car_and_truck.car");

	//car
	_car = _levelManager->GetCar();
	_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[0].chassisSmall), _levelManager->GetCarImage(_carSkins[0].chassisBig));
	_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[0].tireSmall), _levelManager->GetCarImage(_carSkins[0].tireBig));

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);


	//menu
	currentPosition = 0;
	columnStartPosition = 0;
	positionsInColumn = 5;

	carcurrentPosition = 0;
	carcolumnStartPosition = 0;
	carpositionsInColumn = 5;

	if (_sceneFiles.size() < 5)
	{
		positionsInColumn = _sceneFiles.size();
	}



	//timer
	_dt = 0;
	_timer = new Timer();
}

void JellyMenuBetter::Enter()
{

}

void JellyMenuBetter::CleanUp()
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
	delete _levelImage;

	_shaderManager->RemoveAll();
	_textureManager->RemoveAll();
}

void JellyMenuBetter::Pause()
{

}

void JellyMenuBetter::Resume()
{
    if (_jellyGame != nullptr)
    {
        delete _jellyGame;
        _jellyGame = nullptr;
    }

	_inputHelper->Update();
}

void JellyMenuBetter::GamePause()
{

}

void JellyMenuBetter::GameResume()
{

}

void JellyMenuBetter::HandleEvents(GameManager* manager)
{
	if (_inputHelper->MenuActionPressed(MenuAction::MenuExit))
	{
		manager->Quit();
		return;
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuPause))
	{
		JellyOptions* jelly = new JellyOptions(_core);
		//jelly->Init();

		manager->PushState(jelly);
		return;
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuUp))
	{
		currentPosition--;

		if (currentPosition < 0)
		{
			currentPosition = 0;
		}

		_levelImage->SetTexture(_levelManager->GetThumb(_sceneFiles[currentPosition]));

		_audioHelper->PlayHitSound();
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuDown))
	{
		currentPosition++;

		if (currentPosition >= _sceneFiles.size())
		{
			currentPosition = _sceneFiles.size() - 1;
		}

		_levelImage->SetTexture(_levelManager->GetThumb(_sceneFiles[currentPosition]));

		_audioHelper->PlayHitSound();
	}

	if (_inputHelper->ActionPressed(CarAction::Tire))
	{
		carcurrentPosition--;

		if (carcurrentPosition < 0)
		{
			carcurrentPosition = 0;
		}

		_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));

	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuBack))
	{
		carcurrentPosition++;

		if (carcurrentPosition >= _carSkins.size())
		{
			carcurrentPosition = _carSkins.size() - 1;
		}

		_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuAccept))
	{
		_inputHelper->Update();

        _jellyGame = new JellyGame(_core);
        _jellyGame->Init();
        _jellyGame->LoadLevel(_levelManager, _sceneFiles[currentPosition], _levelManager->GetLevelFile(_sceneFiles[currentPosition]), "Assets/Jelly/car_and_truck.car");
        _jellyGame->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
        _jellyGame->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));

		manager->PushState(_jellyGame);

		_audioHelper->PlayMusic();

		return;
	}

	if (_inputHelper->ActionPressed(CarAction::Map))
	{
		_car->Transform();
	}

	//car sterins
	//car torque
	if (_inputHelper->ActionHold(CarAction::Left))
	{
		_car->setTorque(-1);
	}

	if (_inputHelper->ActionHold(CarAction::Right))
	{
		_car->setTorque(1);
	}

	//chasis torque
	_car->mChassis->torque = 0.0f;

	if (_inputHelper->ActionHold(CarAction::RotateLeft))
	{
		_car->mChassis->torque = -1.0f;
	}

	if (_inputHelper->ActionHold(CarAction::RotateRight))
	{
		_car->mChassis->torque = 1.0f;
	}

	_inputHelper->Update();
}

void JellyMenuBetter::Update(GameManager* manager)
{
	if (!_inputHelper->ActionHold(CarAction::Left) && !_inputHelper->ActionHold(CarAction::Right))
		_car->setTorque(0);

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
}

void JellyMenuBetter::Draw(GameManager* manager)
{
	//_core->frameObject->Bind();

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

	//menu level drawing
	{
		//menu level
		for (size_t i = 0; i < _gameBodies.size(); i++)
		{
			_gameBodies[i]->Draw(_jellyProjection);
		}

		//car
		_car->Draw(_jellyProjection);
	}

	//level text
	int levelTextPositionY = (_renderManager->GetHeight() / 2) - _levelImage->GetTexture()->GetHeight() - 30;

	_titleFont->AddText(_sceneFiles[currentPosition], _renderManager->GetWidth() / 2, levelTextPositionY + 3, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
	_titleFont->AddText(_sceneFiles[currentPosition], _renderManager->GetWidth() / 2, levelTextPositionY, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);


	int timePositionX = (_renderManager->GetWidth() / 2) - (_levelImage->GetTexture()->GetWidth() / 2);
	int jumpPositionX = (_renderManager->GetWidth() / 2) + (_levelImage->GetTexture()->GetWidth() / 2);

	int textPosY = (_renderManager->GetHeight() / 2) + 30;

	//time
	char bufferTime[10];
	sprintf(bufferTime, "%.2f", _levelManager->GetTime(_sceneFiles[currentPosition]));

	_menuFont->AddText(std::string(bufferTime) + "s", timePositionX, textPosY + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	_menuFont->AddText(std::string(bufferTime) + "s", timePositionX, textPosY, glm::vec3(0.0f, 0.84f, 0.0f), FontLeft);

	//jump
	char bufferJump[10];
	sprintf(bufferJump, "%.2f", _levelManager->GetJump(_sceneFiles[currentPosition]));

    _menuFont->AddText(std::string(bufferJump) + "m", jumpPositionX, textPosY+2, glm::vec3(0.19f, 0.14f, 0.17f), FontRight);
	_menuFont->AddText(std::string(bufferJump) + "m", jumpPositionX, textPosY, glm::vec3(0.71f, 0.16f, 0.18f), FontRight);


	//options sprite
	_inputHelper->MenuActionSprite(MenuAction::MenuPause)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2) - 40, _renderManager->GetHeight() - 29));
	_inputHelper->MenuActionSprite(MenuAction::MenuPause)->Draw(_projection);

	//options text
	_menuFont->AddText("Options", (_renderManager->GetWidth() / 2) - 90, _renderManager->GetHeight() - 19, glm::vec3(0.19f, 0.14f, 0.17f), FontRight);

	//exit sprite
	_inputHelper->MenuActionSprite(MenuAction::MenuExit)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2) + 40, _renderManager->GetHeight() - 29));
	_inputHelper->MenuActionSprite(MenuAction::MenuExit)->Draw(_projection);

	//exit text
	_menuFont->AddText("Exit", (_renderManager->GetWidth() / 2) + 90, _renderManager->GetHeight() - 19, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);


	//level image
	_levelImage->SetPosition(glm::vec2(_renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - (_levelImage->GetTexture()->GetHeight() / 2)));
	_levelImage->Draw(_projection);

	//draw main text
	_menuFont->Draw(_projection);
	_titleFont->Draw(_projection);
	_smallFont->Draw(_projection);

	//end render to texture
	/*_core->frameObject->UnBind();

	//render normal
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_core->renderSprite->Draw(_projection);*/

	//end frame
	_renderManager->EndFrame();
}