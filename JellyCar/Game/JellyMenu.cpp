#include "JellyMenu.h"

#include "JellySplash.h"
#include "JellyGame.h"

#include "../Utils/JellyHelper.h"

void JellyMenu::Init()
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


	//load shader
	_shader = _shaderManager->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	_projection = glm::ortho(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f, -1.0f, 1.0f);

	_renderManager->SetDepth(false);

	//input
	_inputManager = InputManager::Instance();
	_inputHelper = new InputHelper(_inputManager);

	JellyHellper::Instance()->LoadShaders();

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadAllScenes("scene_list.xml");
	_levelManager->LoadCarSkins("car_skins.xml");
	_levelManager->LoadScores("scores.xml");

	//get scenes names
	_sceneFiles = _levelManager->GetScenes();

	//get first image
	_sprite = new Sprite("thumb", _levelManager->GetThumb(_sceneFiles[0]), _shader);
	_sprite->SetPosition(glm::vec2(_renderManager->GetWidth() / 4, 380));

	//get car skin names
	_carSkins = _levelManager->GetCarSkins();

	_smallChassis = new Sprite("smallChassis", _levelManager->GetCarImage(_carSkins[0].chassisSmall), _shader);
	_bigChassis = new Sprite("bigChassis", _levelManager->GetCarImage(_carSkins[0].chassisBig), _shader);
	_smallTire = new Sprite("smallTire", _levelManager->GetCarImage(_carSkins[0].tireSmall), _shader);
	_bigTire = new Sprite("bigTire", _levelManager->GetCarImage(_carSkins[0].tireBig), _shader);

	_smallChassis->SetScale(glm::vec2(0.5f, 0.5f));
	_bigChassis->SetScale(glm::vec2(0.5f, 0.5f));
	_smallTire->SetScale(glm::vec2(0.4f, 0.4f));
	_bigTire->SetScale(glm::vec2(0.4f, 0.4f));

	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

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

	_gameState = CarGameState::Menu;
}

void JellyMenu::Enter()
{

}

void JellyMenu::CleanUp()
{
	//delete _menuFont;

	_shaderManager->RemoveAll();
	_textureManager->RemoveAll();
}

void JellyMenu::Pause()
{

}

void JellyMenu::Resume()
{

}

void JellyMenu::GamePause()
{

}

void JellyMenu::GameResume()
{

}

void JellyMenu::HandleEvents(GameManager* manager)
{
	if (_inputHelper->ActionPressed(InputAction::Back))
	{
		if (_gameState == CarGameState::Game)
		{
			_gameState = CarGameState::Menu;
		}
		else
			manager->Quit();
	}

	if (_inputHelper->ActionPressed(InputAction::Up))
	{
		currentPosition--;

		if (currentPosition < 0)
		{
			currentPosition = 0;
		}

		if (currentPosition >= 2 && currentPosition < _sceneFiles.size() - 3)
		{
			columnStartPosition--;
			if (columnStartPosition < 0)
			{
				columnStartPosition = 0;
			}
		}

		_sprite->SetTexture(_levelManager->GetThumb(_sceneFiles[currentPosition]));
	}

	if (_inputHelper->ActionPressed(InputAction::Down))
	{
		currentPosition++;

		if (currentPosition >= _sceneFiles.size())
		{
			currentPosition = _sceneFiles.size() - 1;
		}

		if (currentPosition > 2 && currentPosition < _sceneFiles.size() - 2)
		{
			columnStartPosition++;
		}

		_sprite->SetTexture(_levelManager->GetThumb(_sceneFiles[currentPosition]));
	}

	if (_inputHelper->ActionPressed(InputAction::Left))
	{
		carcurrentPosition--;

		if (carcurrentPosition < 0)
		{
			carcurrentPosition = 0;
		}

		if (carcurrentPosition >= 2 && carcurrentPosition < _carSkins.size() - 3)
		{
			carcolumnStartPosition--;
			if (carcolumnStartPosition < 0)
			{
				carcolumnStartPosition = 0;
			}
		}

		_smallChassis->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall));
		_bigChassis->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_smallTire->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall));
		_bigTire->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));
	}

	if (_inputHelper->ActionPressed(InputAction::Right))
	{
		carcurrentPosition++;

		if (carcurrentPosition >= _carSkins.size())
		{
			carcurrentPosition = _carSkins.size() - 1;
		}

		if (carcurrentPosition > 2 && carcurrentPosition < _carSkins.size() - 2)
		{
			carcolumnStartPosition++;
		}

		_smallChassis->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall));
		_bigChassis->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_smallTire->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall));
		_bigTire->SetTexture(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));
	}

	if (_inputHelper->ActionPressed(InputAction::Accept))
	{
		JellyGame* jellyGame = new JellyGame(_core);
		jellyGame->Init();
		jellyGame->LoadLevel(_sceneFiles[currentPosition], "Assets/Jelly/car_and_truck.car");
		jellyGame->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		jellyGame->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));

		manager->PushState(jellyGame);

		_gameState = CarGameState::Game;
	}

	_inputHelper->Update();
}

void JellyMenu::Update(GameManager* manager)
{

}

void JellyMenu::Draw(GameManager* manager)
{
	//start frame
	_renderManager->StartFrame();

	//clear screen
	_renderManager->ClearScreen();

	_backSprite->SetScale(glm::vec2(1.0f, 1.0f));
	_backSprite->SetPosition(glm::vec2(_renderManager->GetWidth() / 4, 20));
	_backSprite->Draw(_projection);
	_backSprite->SetPosition(glm::vec2((_renderManager->GetWidth() / 4) * 3, 20));
	_backSprite->Draw(_projection);

	_backSprite->SetPosition(glm::vec2(_renderManager->GetWidth() / 4, 520));
	_backSprite->Draw(_projection);
	_backSprite->SetPosition(glm::vec2((_renderManager->GetWidth() / 4) * 3, 520));
	_backSprite->Draw(_projection);

	_titleFont->AddText("JELLY CAR", _renderManager->GetWidth() / 2, 66, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
	_titleFont->AddText("JELLY CAR", _renderManager->GetWidth() / 2, 64, glm::vec3(0.71f, 0.16f, 0.18f), FontCenter);

	//level menu
	_menuFont->AddText("Levels", _renderManager->GetWidth() / 4, 100, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);

	int counter = 0;

	for (size_t i = columnStartPosition; i < columnStartPosition + positionsInColumn; i++)
	{
		//shadow
		_menuFont->AddText(_sceneFiles[i], _renderManager->GetWidth() / 8, (140 + (counter * 30)) + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

		//score?
		char time[10];
		sprintf(time, "%.2f", _levelManager->GetTime(_sceneFiles[i]));    // prints out 0.94
		_menuFont->AddText(time, (_renderManager->GetWidth() / 3), (140 + (counter * 30)), glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

		//positions
		if (i == currentPosition)
		{
			_menuFont->AddText(_sceneFiles[i], _renderManager->GetWidth() / 8, 140 + (counter * 30), glm::vec3(0.71f, 0.16f, 0.18f), FontLeft);
		}
		else
		{
			_menuFont->AddText(_sceneFiles[i], _renderManager->GetWidth() / 8, (140 + (counter * 30)), glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);
		}

		counter++;
	}

	_sprite->Draw(_projection);

	//car skins
	_menuFont->AddText("Skins", _renderManager->GetWidth() / 4 * 3, 100, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);

	counter = 0;

	for (size_t i = carcolumnStartPosition; i < carcolumnStartPosition + carpositionsInColumn; i++)
	{
		_menuFont->AddText(_carSkins[i].name, (_renderManager->GetWidth() / 8) * 5, 140 + (counter * 30) + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

		if (i == carcurrentPosition)
		{
			_menuFont->AddText(_carSkins[i].name, (_renderManager->GetWidth() / 8) * 5, 140 + (counter * 30), glm::vec3(0.71f, 0.16f, 0.18f), FontLeft);
		}
		else
		{
			_menuFont->AddText(_carSkins[i].name, (_renderManager->GetWidth() / 8) * 5, 140 + (counter * 30), glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);
		}

		counter++;
	}

	float carPos = ((_renderManager->GetWidth() / 4) * 3) - 80;


	//
	_bigChassis->SetPosition(glm::vec2(carPos, 380));
	_bigChassis->Draw(_projection);

	_bigTire->SetPosition(glm::vec2(carPos - 50, 380 + 50));
	_bigTire->Draw(_projection);

	_bigTire->SetPosition(glm::vec2(carPos + 50, 380 + 50));
	_bigTire->Draw(_projection);

	carPos = ((_renderManager->GetWidth() / 4) * 3) + 80;

	_smallChassis->SetPosition(glm::vec2(carPos, 380));
	_smallChassis->Draw(_projection);

	_smallTire->SetPosition(glm::vec2(carPos - 40, 380 + 30));
	_smallTire->Draw(_projection);

	_smallTire->SetPosition(glm::vec2(carPos + 40, 380 + 30));
	_smallTire->Draw(_projection);

	//draw main text
	_menuFont->Draw(_projection);

	_titleFont->Draw(_projection);

	//end frame
	_renderManager->EndFrame();
}