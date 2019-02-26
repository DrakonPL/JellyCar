#include "JellyOptions.h"

#include <Andromeda/Utils/Logger.h>
#include "../Utils/JellyHelper.h"

#include <stdio.h> 
#include <sstream> 

JellyOptions::JellyOptions(JellyCore* core)
{
	_core = core;
}

void JellyOptions::Init()
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
	//_audioHelper->PlayMusic();

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");


	//background paper sprite
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_backSelectSprite = new Sprite("selectBack", "Assets/Jelly/Texture/back.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_backRoundSprite = new Sprite("selectRoundBack", "Assets/Jelly/Texture/roundBack.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_leftSprite = new Sprite("selectLeft", "Assets/Jelly/Texture/left.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_rightSprite = new Sprite("selectRight", "Assets/Jelly/Texture/right.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_barSprite = new Sprite("optionsBar", "Assets/Jelly/Texture/bar.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_barBlueSprite = new Sprite("optionsBarBlue", "Assets/Jelly/Texture/barBlue.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	//


	_creditsTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/credits.png");
	_libsTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/libs.png");
	_keyboardTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/keyboard.png");
	_gamepadTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/gamepad.png");
	_secretTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/secret.png");
	_volumeTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/volume.png");

	//load menu level
	_world = new World();

	_levelManager->InitPhysic(_world);
	//_levelManager->LoadLevel(_world, "options_scene.scene", "");
	_levelManager->LoadCompiledLevel(_world, "options_scene.scene", "");

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	_screenBounds = glm::vec4(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f);

	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f, -1.0f, 1.0f);

	//timer
	_dt = 0;
	_timer = new Timer();

	InitCredits();
	InitLibs();
	InitActionNames();

	//texture test
	for (size_t i = 0; i < _gameBodies.size(); i++)
	{
		if (_gameBodies[i]->GetName() == "options_libs")
		{
			_gameBodies[i]->SetTexture(_libsTexture);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_credits")
		{
			_gameBodies[i]->SetTexture(_creditsTexture);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_keys")
		{
			_gameBodies[i]->SetTexture(_gamepadTexture);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_sound")
		{
			_gameBodies[i]->SetTexture(_volumeTexture);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_secret")
		{
			_gameBodies[i]->SetTexture(_secretTexture);
			_menuBodies.push_back(_gameBodies[i]);
		}
	}

	_menuBodySelected = 0;
	_alphaScale = 1.0f;

	//binding
	_changeBinding = false;

	_optionsCarLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Car) * 10.0f;
	_optionsSoundLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Sounds) * 10.0f;
	_optionsMusicLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Music) * 10.0f;

	_optionsState = JellyOptionsState::Menu;

	dragBody = NULL;	
}

void JellyOptions::InitCredits()
{
	_credits.push_back(Text("Thanks", 0));
	_credits.push_back(Text("Walaber for original game", 82));
	_credits.push_back(Text("Shadow for car skins", 142));
	_credits.push_back(Text("Ruben Wolfe, SMOKE, TheFloW", 202));
	_credits.push_back(Text("for support and testing", 242));
	_credits.push_back(Text("Rinnegatamante and EasyRPG Team", 302));
	_credits.push_back(Text("for the Audio Decoder used for Sound module", 342));
	_credits.push_back(Text("St3f  for splash screen", 402));
	_credits.push_back(Text("Team Molecule for HENkaku", 462));
	_credits.push_back(Text("Everybody who contributed on vitasdk", 522));
}

void JellyOptions::InitLibs()
{
	_libs.push_back(Text("Libs used for PC/Vita", 0));
	_libs.push_back(Text("freetype", 82));
	_libs.push_back(Text("freetype_gl", 142));
	_libs.push_back(Text("glew", 202));
	_libs.push_back(Text("glfw", 262));
	_libs.push_back(Text("glm", 322));
	_libs.push_back(Text("irrKlang", 382));
	_libs.push_back(Text("JellyPhysics", 442));
	_libs.push_back(Text("ogg", 502));
	_libs.push_back(Text("stb", 562));
	_libs.push_back(Text("speexdsp", 622));
	_libs.push_back(Text("tinyxml", 682));
	_libs.push_back(Text("vorbisfile", 742));
	_libs.push_back(Text("vorbisenc", 802));
	_libs.push_back(Text("vorbis", 862));
}

void JellyOptions::InitActionNames()
{
	_actionNames.push_back("Down");
	_actionNames.push_back("Up");
	_actionNames.push_back("Left");
	_actionNames.push_back("Right");
	_actionNames.push_back("Rotate Left");
	_actionNames.push_back("Rotate Right");
	_actionNames.push_back("Accept");
	_actionNames.push_back("Back");
	_actionNames.push_back("Transform");
	_actionNames.push_back("Ballon");
	_actionNames.push_back("Sticky tire");
	_actionNames.push_back("Map");
	_actionNames.push_back("Pause");
	_actionNames.push_back("Exit");
}

void JellyOptions::Enter()
{

}

void JellyOptions::CleanUp()
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
	delete _backSelectSprite;
	delete _backRoundSprite;
	delete _leftSprite;
	delete _rightSprite;
	delete _barSprite;
	delete _barBlueSprite;
}

void JellyOptions::Pause()
{

}

void JellyOptions::Resume()
{

}

void JellyOptions::GamePause()
{

}

void JellyOptions::GameResume()
{

}

void JellyOptions::HandleEvents(GameManager* manager)
{
	if (_optionsState == Menu)
	{
		//keys
		if (_inputHelper->ActionPressed(InputAction::Left))
		{
			_menuBodySelected--;

			if (_menuBodySelected < 0)
			{
				_menuBodySelected = 0;
			}
		}

		//move down
		if (_inputHelper->ActionPressed(InputAction::Right))
		{
			_menuBodySelected++;
			if (_menuBodySelected > _menuBodies.size() - 1)
			{
				_menuBodySelected = _menuBodies.size() - 1;
			}
		}

		//touch 
		if (_inputHelper->GetTouchCount() > 0)
		{
			glm::vec2 touch = _inputHelper->TouchToScreen(_screenBounds, _inputHelper->GetTouchPosition(0));

			dragX = touch.x;
			dragY = touch.y;

			touchF = true;
		}
		else
		{
			//testtouch = glm::vec2(0.0f, 0.0f);
			touchF = false;
		}

		if (touchF == true)
		{
			if (dragBody != NULL)
			{
				PointMass *pm = dragBody->getPointMass(dragPoint);
				dragBody->setDragForce(VectorTools::calculateSpringForce(pm->Position, pm->Velocity, Vector2(dragX, dragY), Vector2(0, 0), 0.0f, 100.0f, 10.0f), dragPoint);
			}
		}
		else
		{
			dragBody = NULL;
			dragPoint = -1;
		}

		if (touchF == true)
		{
			if (dragBody == NULL)
			{
				int body;
				_world->getClosestPointMass(Vector2(dragX, dragY), body, dragPoint);
				dragBody = _world->getBody(body);

				//click testing
				for (size_t i = 0; i < _gameBodies.size(); i++)
				{
					if(_gameBodies[i]->GetBody()->contains(Vector2(dragX, dragY)))
					{
						if (_gameBodies[i]->GetName() == "options_libs")
						{
							_optionsState = JellyOptionsState::Libs;
							_libsPosition = 510;
						}

						if (_gameBodies[i]->GetName() == "options_credits")
						{
							_optionsState = JellyOptionsState::Credits;
							_creditsPosition = 510;
						}

						if (_gameBodies[i]->GetName() == "options_keys")
						{
							_optionsState = JellyOptionsState::Controls;
							_selctedPosition = 0;
							_alphaScale = 1.0f;
						}

						if (_gameBodies[i]->GetName() == "options_sound")
						{
							_optionsState = JellyOptionsState::Sound;
							_soundPosition = 0;
							_alphaScale = 1.0f;
							_audioHelper->PlayFastEngine();
						}
					}					
				}				
			}
		}

		if (_inputHelper->ActionPressed(InputAction::Accept))
		{
			
			if (_menuBodies[_menuBodySelected]->GetName() == "options_libs")
			{
				_optionsState = JellyOptionsState::Libs;
				_libsPosition = 510;
			}

			else if (_menuBodies[_menuBodySelected]->GetName() == "options_credits")
			{
				_optionsState = JellyOptionsState::Credits;
				_creditsPosition = 510;
			}

			else if (_menuBodies[_menuBodySelected]->GetName() == "options_keys")
			{
				_optionsState = JellyOptionsState::Controls;
				_selctedPosition = 0;
				_alphaScale = 1.0f;
			}

			else if (_menuBodies[_menuBodySelected]->GetName() == "options_sound")
			{
				_optionsState = JellyOptionsState::Sound;
				_soundPosition = 0;
				_alphaScale = 1.0f;
				_audioHelper->PlayFastEngine();
			}
		}

		if (_inputHelper->ActionPressed(InputAction::Back))
		{
			manager->PopState();
			return;
		}
	}
	else if (_optionsState == JellyOptionsState::Controls)
	{
		//_inputHelper->ChangeMappingForAction(Action...)

		if (_changeBinding)
		{
			if (_inputHelper->ChangeInputForAction(_selectedAction))
			{
				_changeBinding = false;
			}
		}else
		{
			//move up
			if (_inputHelper->ActionPressed(InputAction::Up))
			{
				_selctedPosition--;
				if (_selctedPosition < 0)
				{
					_selctedPosition = 0;
				}
			}

			//move down
			if (_inputHelper->ActionPressed(InputAction::Down))
			{
				_selctedPosition++;
				if (_selctedPosition > _actionNames.size() - 1)
				{
					_selctedPosition = _actionNames.size() - 1;
				}
			}

			if (_inputHelper->ActionPressed(InputAction::Accept))
			{
				//check action to process
				_selectedAction = static_cast<InputAction>(_selctedPosition);
				_changeBinding = true;

				_inputHelper->UpdateAllInputs(true);
			}

			if (_inputHelper->ActionPressed(InputAction::Back))
			{
				//save settings
				_inputHelper->SaveSettings();
				
				//go back to options menu
				_optionsState = Menu;
			}
		}		
	}else if (_optionsState == JellyOptionsState::Sound)
	{
		//move up
		if (_inputHelper->ActionPressed(InputAction::Up))
		{
			_soundPosition--;
			if (_soundPosition < 0)
			{
				_soundPosition = 0;
			}

			_audioHelper->StopEngineSound();

			if (_soundPosition == 0)
			{
				_audioHelper->PlayFastEngine();
			}
		}

		//move down
		if (_inputHelper->ActionPressed(InputAction::Down))
		{
			_soundPosition++;
			if (_soundPosition > 2)
			{
				_soundPosition = 2;
			}

			_audioHelper->StopEngineSound();

			if (_soundPosition == 0)
			{
				_audioHelper->PlayFastEngine();
			}
		}

		if (_inputHelper->ActionPressed(InputAction::Left))
		{
			if (_soundPosition == 0)
			{
				_optionsCarLevel--;
				if (_optionsCarLevel < 0)
				{
					_optionsCarLevel = 0;
				}

				_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Car, (float)_optionsCarLevel / 10.0f);
			}

			if (_soundPosition == 1)
			{
				_optionsSoundLevel--;
				if (_optionsSoundLevel < 0)
				{
					_optionsSoundLevel = 0;
				}

				_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Sounds, (float)_optionsSoundLevel / 10.0f);
				_audioHelper->PlayHitSound();
			}

			if (_soundPosition == 2)
			{
				_optionsMusicLevel--;
				if (_optionsMusicLevel < 0)
				{
					_optionsMusicLevel = 0;
				}

				_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Music, (float)_optionsMusicLevel / 10.0f);
			}
		}

		//move down
		if (_inputHelper->ActionPressed(InputAction::Right))
		{
			if (_soundPosition == 0)
			{
				_optionsCarLevel++;
				if (_optionsCarLevel > 10)
				{
					_optionsCarLevel = 10;
				}

				_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Car, (float)_optionsCarLevel / 10.0f);
			}
			
			if (_soundPosition == 1)
			{
				_optionsSoundLevel++;
				if (_optionsSoundLevel > 10)
				{
					_optionsSoundLevel = 10;
				}

				_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Sounds, (float)_optionsSoundLevel / 10.0f);
				_audioHelper->PlayHitSound();
			}

			if (_soundPosition == 2)
			{
				_optionsMusicLevel++;
				if (_optionsMusicLevel > 10)
				{
					_optionsMusicLevel = 10;
				}

				_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Music, (float)_optionsMusicLevel / 10.0f);
			}
		}

		if (_inputHelper->ActionPressed(InputAction::Back))
		{
			//cave settings
			_audioHelper->SaveSettings();

			//stop engine sound
			_audioHelper->StopEngineSound();

			//go back to menu options
			_optionsState = Menu;
		}
	}
	else
	{
		if (_inputHelper->ActionPressed(InputAction::Back))
		{
			_optionsState = Menu;
		}
	}

	_inputHelper->Update();
}

void JellyOptions::Update(GameManager* manager)
{
	_dt = _timer->GetDelta();

	if (_optionsState == Menu)
	{
		//Update physic
		for (int i = 0; i < 6; i++)
		{
			_world->update(0.004f);

			for (size_t i = 0; i < _gameBodies.size(); i++)
				_gameBodies[i]->Update(0.004f);
		}

		//update alpha scale
		_alphaScale += (_scaleFactor  * _dt);

		if (_alphaScale > 1.0f)
		{
			_scaleFactor = -2.0f;
		}

		if (_alphaScale < 0.0f)
		{
			_scaleFactor = 2.0f;
		}
	}	

	if (_optionsState == Credits)
	{
		_creditsPosition -= (_dt * 40);

		bool reset = true;
		for (size_t i = 0; i < _credits.size(); i++)
		{
			if (_credits[i].StartPosition + _creditsPosition > 110)
			{
				reset = false;
			}
		}

		if (reset)
		{
			_creditsPosition = 510;
		}
	}

	if (_optionsState == Libs)
	{
		_libsPosition -= (_dt * 40);

		bool reset = true;
		for (size_t i = 0; i < _libs.size(); i++)
		{
			if (_libs[i].StartPosition + _libsPosition > 110)
			{
				reset = false;
			}
		}

		if (reset)
		{
			_libsPosition = 510;
		}
	}

	if (_optionsState == Controls)
	{
		_alphaScale += (_scaleFactor  * _dt);

		if (_alphaScale > 1.0f)
		{
			_scaleFactor = -2.0f;
		}

		if (_alphaScale < 0.0f)
		{
			_scaleFactor = 2.0f;
		}
	}

	if (_optionsState == Sound)
	{
		_alphaScale += (_scaleFactor  * _dt);

		if (_alphaScale > 1.0f)
		{
			_scaleFactor = -2.0f;
		}

		if (_alphaScale < 0.0f)
		{
			_scaleFactor = 2.0f;
		}
	}
}

void JellyOptions::Draw(GameManager* manager)
{
	//start frame
	_renderManager->StartFrame();

	//clear screen
	_renderManager->ClearScreen();

	//paper background
	_backSprite->SetScale(glm::vec2(1.0f, 1.0f));
	_backSprite->SetPosition(glm::vec2(_renderManager->GetWidth() / 4, 20));
	_backSprite->Draw(_projection);
	_backSprite->SetPosition(glm::vec2((_renderManager->GetWidth() / 4) * 3, 20));
	_backSprite->Draw(_projection);

	_backSprite->SetPosition(glm::vec2(_renderManager->GetWidth() / 4, 520));
	_backSprite->Draw(_projection);
	_backSprite->SetPosition(glm::vec2((_renderManager->GetWidth() / 4) * 3, 520));
	_backSprite->Draw(_projection);

	//menu level drawing
	if (_optionsState == Menu)
	{
		for (size_t i = 0; i < _gameBodies.size(); i++)
		{
			_gameBodies[i]->SetLineColor(glm::vec4(0, 0, 0, 1.0f));
		}

		//set alpha value for selected menu
		_menuBodies[_menuBodySelected]->SetLineColor(glm::vec4(0,0,1,_alphaScale));

		//menu level
		for (size_t i = 0; i < _gameBodies.size(); i++)
		{
			_gameBodies[i]->Draw(_jellyProjection);
		}
	}else
	{
		//menu level
		for (size_t i = 0; i < _gameBodies.size(); i++)
		{
			if (_gameBodies[i]->GetBody()->getIsStatic())
			{
				_gameBodies[i]->Draw(_jellyProjection);
			}			
		}
	}

	int centerX = _renderManager->GetWidth() / 2;

	switch (_optionsState)
	{
		case JellyOptionsState::Menu:
		{
			_titleFont->AddText("Options", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Options", centerX, 55, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);
		}
		break;
		case JellyOptionsState::Sound:
		{
			_titleFont->AddText("Sound Levels", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Sound Levels", centerX, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

			_menuFont->AddText("Car", 250, 115, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
			_menuFont->AddText("Car", 250, 113, glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

			_menuFont->AddText("Sounds", 250, 255, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
			_menuFont->AddText("Sounds", 250, 253, glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

			_menuFont->AddText("Music", 250, 395, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
			_menuFont->AddText("Music", 250, 393, glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

			//rounded back
			_backRoundSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			_backRoundSprite->SetScale(glm::vec2(0.8f, 0.8f));

			_backRoundSprite->SetPosition(glm::vec2(210, 170 + (_soundPosition * 140)));
			_backRoundSprite->Draw(_projection);

			_backRoundSprite->SetPosition(glm::vec2(750, 170 + (_soundPosition * 140)));
			_backRoundSprite->Draw(_projection);

			//left side
			_leftSprite->SetScale(glm::vec2(0.8f, 0.8f));

			_leftSprite->SetPosition(glm::vec2(210,170));
			_leftSprite->Draw(_projection);

			_leftSprite->SetPosition(glm::vec2(210, 310));
			_leftSprite->Draw(_projection);

			_leftSprite->SetPosition(glm::vec2(210, 450));
			_leftSprite->Draw(_projection);

			//right side
			_rightSprite->SetScale(glm::vec2(0.8f, 0.8f));

			_rightSprite->SetPosition(glm::vec2(750, 170));
			_rightSprite->Draw(_projection);

			_rightSprite->SetPosition(glm::vec2(750, 310));
			_rightSprite->Draw(_projection);

			_rightSprite->SetPosition(glm::vec2(750, 450));
			_rightSprite->Draw(_projection);

			//sound bars
			_barSprite->SetScale(glm::vec2(0.8f, 0.8f));
			_barBlueSprite->SetScale(glm::vec2(0.8f, 0.8f));

			int startBar = 300;

			//car
			for (int i = 0; i < 10;i++)
			{
				if (i < _optionsCarLevel)
				{
					_barSprite->SetPosition(glm::vec2(startBar + (i * 40), 170));
					_barSprite->Draw(_projection);
				}else
				{
					_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), 170));
					_barBlueSprite->Draw(_projection);
				}
			}

			//sounds
			for (int i = 0; i < 10; i++)
			{
				if (i < _optionsSoundLevel)
				{
					_barSprite->SetPosition(glm::vec2(startBar + (i * 40), 310));
					_barSprite->Draw(_projection);
				}else
				{
					_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), 310));
					_barBlueSprite->Draw(_projection);
				}
			}

			//music
			for (int i = 0; i < 10; i++)
			{
				if (i < _optionsMusicLevel)
				{
					_barSprite->SetPosition(glm::vec2(startBar + (i * 40), 450));
					_barSprite->Draw(_projection);
				}else
				{
					_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), 450));
					_barBlueSprite->Draw(_projection);					
				}
			}
		}
		break;
		case JellyOptionsState::Controls: 
		{
			if (_changeBinding)
			{
				_backSelectSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			}
			else
			{
				_backSelectSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}

			int startPosition = 170;
			int step = 45;

			//draw backgroud bar
			_backSelectSprite->SetScale(glm::vec2(0.5f,1.0f));

			if (_selctedPosition < 7)
			{
				_backSelectSprite->SetPosition(glm::vec2(centerX - 128, startPosition - 10 + (_selctedPosition * step)));
			}
			else
			{
				_backSelectSprite->SetPosition(glm::vec2(centerX + 128, startPosition - 10 + (_selctedPosition * step) - (step * 7)));
			}			
			
			_backSelectSprite->Draw(_projection);

			_titleFont->AddText("Controls", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Controls", centerX, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);			

			//draw text
			int startY = startPosition;

			//7 left positions
			for (size_t i = 0; i < 7; i++)
			{
				_menuFont->AddText(_actionNames[i], centerX - 90, startY, glm::vec3(0.19f, 0.14f, 0.17f), FontRight);
				_menuFont->AddText(_actionNames[i], centerX - 90, startY - 2, glm::vec3(1.0f, 0.65f, 0.0f), FontRight);

				startY += step;
			}

			//7 right positions
			startY = startPosition;
			for (size_t i = 7; i < _actionNames.size(); i++)
			{
				_menuFont->AddText(_actionNames[i], centerX + 90, startY, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
				_menuFont->AddText(_actionNames[i], centerX + 90, startY - 2 , glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

				startY += step;
			}

			//draw 7 left icons
			startY = startPosition - 10;
			for (size_t i = 0; i < 7; i++)
			{
				InputAction action = static_cast<InputAction>(i);

				if (!(_changeBinding && i == _selctedPosition))
				{
					_inputHelper->ActionSprite(action)->SetPosition(glm::vec2(centerX - 40, startY));
					_inputHelper->ActionSprite(action)->Draw(_projection);

					//_menuFont->AddText(_inputHelper->InputName(action), glm::vec2(centerX - 40, startY + 10), glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
				}

				startY += step;
			}

			//draw 7 right icons
			startY = startPosition - 10;
			for (size_t i = 7; i < InputAction::Count; i++)
			{
				InputAction action = static_cast<InputAction>(i);

				if (!(_changeBinding && i == _selctedPosition))
				{
					_inputHelper->ActionSprite(action)->SetPosition(glm::vec2(centerX + 40, startY));
					_inputHelper->ActionSprite(action)->Draw(_projection);

					//_menuFont->AddText(_inputHelper->InputName(action), glm::vec2(centerX + 40, startY + 10), glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
				}				

				startY += step;
			}
		}
		break;
		case JellyOptionsState::Credits:
		{
			_titleFont->AddText("Credits", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Credits", centerX, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

			for (size_t i = 0; i < _credits.size(); i++)
			{
				if (_credits[i].StartPosition + _creditsPosition  > 110 && _credits[i].StartPosition + _creditsPosition < 510 )
				{
					_menuFont->AddText(_credits[i].Content, centerX, _credits[i].StartPosition + _creditsPosition, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
					_menuFont->AddText(_credits[i].Content, centerX, _credits[i].StartPosition - 2 + _creditsPosition, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);
				}
			}

		}
		break;
		case JellyOptionsState::Libs:
		{
			_titleFont->AddText("Libs", _renderManager->GetWidth() / 2, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Libs", _renderManager->GetWidth() / 2, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

			for (size_t i = 0; i < _libs.size(); i++)
			{
				if (_libs[i].StartPosition + _libsPosition  > 110 && _libs[i].StartPosition + _libsPosition < 510)
				{
					_menuFont->AddText(_libs[i].Content, centerX, _libs[i].StartPosition + _libsPosition, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
					_menuFont->AddText(_libs[i].Content, centerX, _libs[i].StartPosition - 2 + _libsPosition, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);
				}
			}
		}
		break;
		case JellyOptionsState::Secret:
		{
			//?
		}
		break;
	}

	//back to menu
	_inputHelper->ActionSprite(InputAction::Back)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), 528));
	_inputHelper->ActionSprite(InputAction::Back)->Draw(_projection);
	_menuFont->AddText("Back", (_renderManager->GetWidth() / 2) + 30, 538, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

	//draw main text
	_menuFont->Draw(_projection);
	_titleFont->Draw(_projection);
	_smallFont->Draw(_projection);

	//end frame
	_renderManager->EndFrame();
}