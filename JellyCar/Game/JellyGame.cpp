#include "JellyGame.h"

#include "../Utils/JellyHelper.h"

#include <stdio.h> 
#include <sstream> 


template <typename T>
std::string to_string(const T& value)
{
	std::ostringstream s;
	s << value;
	return s.str();
}

JellyGame::JellyGame(JellyCore* core)
{
	_core = core;

	_renderManager = NULL;
	_shaderManager = NULL;
	_textureManager = NULL;

	_shader = NULL;

	_targetSprite = NULL;
	_backSprite = NULL;
	_tireSprite = NULL;
	_ballonSprite = NULL;
	_tireSpriteBack = NULL;
	_ballonSpriteBack = NULL;

	_transformMeter = NULL;

	_menuFont = NULL;

	_timer = NULL;

	_inputHelper = NULL;
	_audioHelper = NULL;

	mWorld = NULL;
	_car = NULL;

	_levelManager = NULL;
	_menuLevelManager = NULL;

    _jellyOptions = nullptr;

	initialized = false;
}

void JellyGame::Init()
{
	if (initialized)
	{
		return;
	}

	_renderManager = RenderManager::Instance();
	_shaderManager = ShaderManager::Instance();
	_textureManager = TextureManager::Instance();

	_renderManager->SetClearColor(0xffffffff);

	//font
	_menuFont = _core->_menuFont;

	//load shader
	_shader = _shaderManager->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	_projection = glm::ortho(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f, -1.0f, 1.0f);
	_jellyProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -1.0f, 1.0f);

	//
	_screenBounds = glm::vec4(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f);

	//
	_renderManager->SetDepth(false);

	//input
	_inputHelper = InputHelper::Instance();

	//init audio helper
	_audioHelper = AudioHelper::Instance();
	_audioHelper->PlaySlowEngine();

	_fastCar = false;
	_slowCar = true;

	//jelly physics
	mWorld = new World();

	_levelManager = 0;
	//
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", _shader);

	//load target sprite
	_targetSprite = new Sprite("target", "Assets/Jelly/Texture/finish.png", _shader);
	_targetSprite->SetScale(glm::vec2(0.04f, 0.04f));

	_tireSprite = new Sprite("tireSprite", "Assets/Jelly/Texture/tire.png", _shader);
	_tireSprite->SetScale(glm::vec2(0.04f, 0.04f));

	_ballonSprite = new Sprite("ballonSprite", "Assets/Jelly/Texture/ballon.png", _shader);
	_ballonSprite->SetScale(glm::vec2(0.04f, 0.04f));	

	_tireSpriteBack = new Sprite("tireBack", "Assets/Jelly/Texture/roundBackRed.png", _shader);
	_ballonSpriteBack = new Sprite("ballonBack", "Assets/Jelly/Texture/roundBack.png", _shader);

	_transformMeter = new Sprite("transformmeter", "Assets/Jelly/Texture/transform_meter.png", _shader);
	_transformMeter->SetPosition(glm::vec2(128,40));

	//menu
	_showMap = false;

	//timer
	_dt = 0.0f;
	_time = 0.0f;
	_timer = new Timer();

	_hitTime = 0.0f;
	_chassisHit = 0.0f;

	//jumping
	_isJumping = false;
	_inTheAir = false;
	_jumpStartPosition = 0.0f;
	_bestJumpLenght = 0.0f;

	mTransformMeter = 1.0f;
	mTransformMeterRechargeSpeed = 1.0f / 13.0f;
	mTransformMeterGrowDir = 0;

	//checkpoint
	_checkpoint = false;

	//ballon and tire initialization
	_haveBallon = false;
	_haveTire = false;

	_ballonActive = false;
	_tireActive = false;

	_ballonPressed = false;
	_tirePressed = false;

	_ballonTime = 0.0f;
	_tireTime = 0.0f;

	_ballonAABB.expandToInclude(Vector2(((float)_renderManager->GetWidth() / 2.0f) + 128.0f + 64.0f, 450.0f + 64.0f));
	_ballonAABB.expandToInclude(Vector2(((float)_renderManager->GetWidth() / 2.0f) + 128.0f - 64.0f, 450.0f - 64.0f));

	_tireAABB.expandToInclude(Vector2(((float)_renderManager->GetWidth() / 2.0f) - 128.0f + 64.0f, 450.0f + 64.0f));
	_tireAABB.expandToInclude(Vector2(((float)_renderManager->GetWidth() / 2.0f) - 128.0f - 64.0f, 450.0f - 64.0f));

	_gamePlayState = GamePlayState::Play;

	initialized = true;
}

bool JellyGame::LoadLevel(LevelManager* levelManager, std::string levelName,std::string sceneName, std::string carFileName)
{
	_sceneFile = sceneName;
	_levelName = levelName;
	_menuLevelManager = levelManager;

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");

	//init physic
	_levelManager->InitPhysic(mWorld);

	// filter callbacks (for sound playback)
	mWorld->setMaterialPairFilterCallback(0, 2, this);
	mWorld->setMaterialPairFilterCallback(1, 2, this);
	mWorld->setMaterialPairFilterCallback(4, 2, this);

	mWorld->setMaterialPairFilterCallback(0, 3, this);
	mWorld->setMaterialPairFilterCallback(1, 3, this);
	mWorld->setMaterialPairFilterCallback(4, 3, this);

	//ballon and tire collision
	mWorld->setMaterialPairFilterCallback(5, 2, this);
	mWorld->setMaterialPairFilterCallback(5, 3, this);

	//load level
	//_levelManager->LoadLevel(mWorld, _sceneFile, carFileName);
	_levelManager->LoadCompiledLevel(mWorld, _sceneFile, carFileName);

	//car
	_car = _levelManager->GetCar();

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	//set map size
	_wholeMapPosition = _levelManager->GetWorldCenter();
	_wholeMapSize = _levelManager->GetWorldSize();
	_worldLimits = _levelManager->GetWorldLimits();

	//compute map view
	//_mapLimits
	//doing this at 2 am was not so easy :p
	if (_wholeMapSize.X > _wholeMapSize.Y)
	{
		float sizeX = _wholeMapSize.X;
		float sizeY = _wholeMapSize.X  * 0.56f;

		float startX, startY;
		float endX, endY;

		if (sizeY < _wholeMapSize.Y)
		{
			startY = _worldLimits.Min.Y - 5.0f;
			endY = _worldLimits.Max.Y + 5.0f;

			sizeX = ((_wholeMapSize.Y + 10.0f)/ 0.56f);

			startX = _wholeMapPosition.X - (sizeX / 2.0f);
			endX = _wholeMapPosition.X + (sizeX / 2.0f);
		}
		else
		{
			startX = _worldLimits.Min.X;
			endX = _worldLimits.Max.X;

			startY = _wholeMapPosition.Y - (sizeY / 2.0f);
			endY = _wholeMapPosition.Y + (sizeY / 2.0f);
		}

		_mapLimits.Min.X = startX;
		_mapLimits.Min.Y = startY;
		_mapLimits.Max.X = endX;
		_mapLimits.Max.Y = endY;
	}
	else
	{
		float sizeX = _wholeMapSize.Y  / 0.56f;
		float sizeY = _wholeMapSize.Y;

		float startX = 0.0f, startY = 0.0f;
		float endX = 0.0f, endY = 0.0f;

		if (sizeX < _wholeMapSize.X)
		{
			//??
		}
		else
		{
			startY = _worldLimits.Min.Y;
			endY = _worldLimits.Max.Y;

			startX = _wholeMapPosition.X - (sizeX / 2.0f);
			endX = _wholeMapPosition.X + (sizeX / 2.0f);
		}

		_mapLimits.Min.X = startX;
		_mapLimits.Min.Y = startY;
		_mapLimits.Max.X = endX;
		_mapLimits.Max.Y = endY;
	}

	_wholeMapSize.X += 5;
	_wholeMapSize.Y = (_wholeMapSize.X *0.56f) / 2.0f;
	_wholeMapSize.X = _wholeMapSize.X / 2.0f;

	_levelTarget = _levelManager->GetLevelTarget();
	_levelLine = _levelManager->GetLevelLine();

	_targetSprite->SetPosition(glm::vec2(_levelTarget.X, _levelTarget.Y));

	carBreakCount = 0;

	_newTimeRecord = false;
	_newJumpRecord = false;

	_gamePlayState = GamePlayState::Play;

	return true;
}

void JellyGame::SetChassisTextures(Texture* small, Texture* big)
{
	_car->SetChassisTextures(small, big);
}

void JellyGame::SetTireTextures(Texture* small, Texture* big)
{
	_car->SetTireTextures(small, big);
}

bool JellyGame::collisionFilter(Body* bodyA, int bodyApm, Body* bodyB, int bodyBpm1, int bodyBpm2, Vector2 hitPt, float normalVel)
{
	if (bodyB->getMaterial() == 5 || bodyA->getMaterial() == 5)
	{
		Body* itemBody = 0;

		if (bodyB->getMaterial() == 5)
		{
			itemBody = bodyB;
		}
		else
		{
			itemBody = bodyA;
		}

		if (itemBody->getDisable())
		{
			return false;
		}

		if (itemBody->GetName() == "itemstick")
		{
			_haveTire = true;
			_tireTime = 30.0f;
		}
		else if(itemBody->GetName() == "itemballoon")
		{
			_haveBallon = true;
			_ballonTime = 30.0f;
		}
		else if(itemBody->GetName() == "checkpoint")
		{
			_checkpoint = true;
			_checkpointPosition = glm::vec2(itemBody->mDerivedPos.X, itemBody->mDerivedPos.Y + 1);
		}

		itemBody->setDisable(true);

		return false;
	}

	_isJumping = false;

	//hit sound
	float hitVel = fabsf(normalVel);

	if (hitVel > _chassisHit) 
	{
		_chassisHit = hitVel;
	}

	return true;
}

void JellyGame::Enter()
{

}

void JellyGame::CleanUp()
{
	//remove sprites
	delete _targetSprite;
	delete _backSprite;
	delete _transformMeter;
    delete _tireSprite;
    delete _ballonSprite;
    delete _tireSpriteBack;
    delete _ballonSpriteBack;

	//remove timer
	delete _timer;

	//delete level manager
	//and all info
	delete _levelManager;

	//remove physic world
	delete mWorld;

	//clear pointers
	_gameBodies.clear();
}

void JellyGame::Pause()
{

}

void JellyGame::Resume()
{
    if (_jellyOptions != nullptr)
        delete _jellyOptions;

	_inputHelper->Update();
}

void JellyGame::GamePause()
{

}

void JellyGame::GameResume()
{

}

void JellyGame::HandleEvents(GameManager* manager)
{
	if (_gamePlayState == GamePlayState::Play)
	{
		if (_inputHelper->MenuActionPressed(MenuAction::MenuPause))
		{
			_gamePlayState = GamePlayState::Paused;
			_audioHelper->StopEngineSound();
			return;
		}

		if (_inputHelper->ActionPressed(CarAction::Map))
		{
			_showMap = !_showMap;
		}

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

		if (_inputHelper->ActionPressed(CarAction::Transform))
		{
			if (mTransformMeter > 0.0f)
			{
				if (_car->Transform())
					mTransformMeterGrowDir = (mTransformMeterGrowDir >= 0) ? -1 : 1;
			}
		}

		if (_haveBallon && _inputHelper->ActionPressed(CarAction::Ballon))
		{
			_ballonActive = !_ballonActive;
			_car->UseBallon(_ballonActive);
		}

		if (_haveTire && _inputHelper->ActionPressed(CarAction::Tire))
		{
			_tireActive = !_tireActive;
			_car->UseNearestGracity(_tireActive);
		}

		if (_inputHelper->ActionHold(CarAction::Left) || _inputHelper->ActionHold(CarAction::Right))
		{
			if (_car->getVelocity().length() > 2.0f && _slowCar)
			{
				_audioHelper->PlayFastEngine();
				_slowCar = false;
				_fastCar = true;
			}
		}
		else
		{
			if (_fastCar)
			{
				_audioHelper->PlaySlowEngine();
				_slowCar = true;
				_fastCar = false;
			}
		}

		//touch support
		//tire and ballon
		if (_haveBallon || _haveTire)
		{
			if (_inputHelper->GetTouchCount() > 0)
			{
				glm::vec2 touch = _inputHelper->TouchToScreen(_screenBounds, _inputHelper->GetTouchPosition(0));

				//check if pressed ballon trigger
				if (_haveBallon && _ballonAABB.contains(Vector2(touch.x, touch.y)))
				{
					if (!_ballonPressed)
					{
						//change ballon state
						_ballonActive = !_ballonActive;
						
						//activate or deactivate ballon
						_car->UseBallon(_ballonActive);

						_ballonPressed = true;
					}
				}

				//check if pressed tire trigger
				if (_haveTire && _tireAABB.contains(Vector2(touch.x, touch.y)))
				{
					if (!_tirePressed)
					{
						//change ballon state
						_tireActive = !_tireActive;

						//activate or deactivate ballon
						_car->UseNearestGracity(_tireActive);

						_tirePressed = true;
					}
				}
			}
			else
			{
				_ballonPressed = false;
				_tirePressed = false;
			}
		}

	}

	if (_gamePlayState == GamePlayState::Paused)
	{
		if (_checkpoint)
		{
			if (_inputHelper->ActionPressed(CarAction::Tire))
			{
				Vector2 pos = Vector2(_checkpointPosition.x, _checkpointPosition.y);
				Vector2 scale = Vector2(1.0f, 1.0f);

				_car->getChassisBody()->setVelocity(Vector2(0, 0));
				_car->getTire(0)->setVelocity(Vector2(0, 0));
				_car->getTire(1)->setVelocity(Vector2(0, 0));

				_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
				_car->getTire(0)->setPositionAngle(Vector2(_checkpointPosition.x + 1.5f, _checkpointPosition.y - 0.3f), 0.0f, scale);
				_car->getTire(1)->setPositionAngle(Vector2(_checkpointPosition.x - 1.3f, _checkpointPosition.y - 0.3f), 0.0f, scale);


				_gamePlayState = GamePlayState::Play;

				if (_fastCar)
				{
					_audioHelper->PlayFastEngine();
				}
				else
				{
					_audioHelper->PlaySlowEngine();
				}
			}
		}

		if (_inputHelper->MenuActionPressed(MenuAction::MenuBack))
		{
			_gamePlayState = GamePlayState::Play;

			if (_fastCar)
			{
				_audioHelper->PlayFastEngine();
			}
			else
			{
				_audioHelper->PlaySlowEngine();
			}
		}

		if (_inputHelper->MenuActionPressed(MenuAction::MenuPause))
		{
            _jellyOptions = new JellyOptions(_core);

			manager->PushState(_jellyOptions);
			return;
		}

		if (_inputHelper->MenuActionPressed(MenuAction::MenuExit))
		{
			_levelManager->ClearLevel(mWorld);
			manager->PopState();
			return;
		}				
	}

	if (_gamePlayState != GamePlayState::Play && _gamePlayState != GamePlayState::Paused)
	{
		if (_inputHelper->MenuActionPressed(MenuAction::MenuBack))
		{
			_levelManager->ClearLevel(mWorld);
			manager->PopState();
			return;
		}
	}

	_inputHelper->Update();
}

void JellyGame::UpdateTransformMeter(float dt)
{
	// transform meter update.
	if (mTransformMeterGrowDir != 0)
	{
		//float oldValue = mTransformMeter;
		mTransformMeter += (mTransformMeterRechargeSpeed * dt * (float)mTransformMeterGrowDir);

		if (mTransformMeter <= 0.0f)
		{
			if (_car->TransformStatus != Car::Normal)
				_car->Transform();
		}

		if (mTransformMeter <= -0.2f)
		{
			mTransformMeter = -0.2f;
			mTransformMeterGrowDir = 1;
		}

		if ((mTransformMeter > 1.0f))
		{
			mTransformMeter = 1.0f;
			mTransformMeterGrowDir = 0;
		}

	}
}

void JellyGame::Update(GameManager* manager)
{
	if (_gamePlayState == GamePlayState::Play)
	{
		if (!_inputHelper->ActionHold(CarAction::Left) && !_inputHelper->ActionHold(CarAction::Right))
			_car->setTorque(0);

		_dt = _timer->GetDelta();
		_time += _dt;
		_hitTime += _dt;

		//hit info - zero before update/callback event
		_chassisHit = 0;

		//jumping info - true before update/callback event
		_isJumping = true;

		//Update physic
		for (int i = 0; i < 6; i++)
		{
			mWorld->update(0.004f);

			for (size_t i = 0; i < _gameBodies.size(); i++)
				_gameBodies[i]->Update(0.004f);

			_car->clearForces();
			_car->update(0.004f);

			UpdateTransformMeter(0.004f);
		}


		if (_ballonActive)
		{
			_ballonTime -= _dt;

			if (_ballonTime <= 0.0f)
			{
				_car->UseBallon(false);
				_ballonActive = false;
				_haveBallon = false;
			}
		}

		if (_tireActive)
		{
			_tireTime -= _dt;

			if (_tireTime <= 0.0f)
			{
				_car->UseNearestGracity(false);
				_tireActive = false;
				_haveTire = false;
			}
		}

		//update camera*
		//screen ratio 0.56
		if (_showMap)
		{
			_jellyProjection = glm::ortho(_mapLimits.Min.X, _mapLimits.Max.X, _mapLimits.Min.Y, _mapLimits.Max.Y, -1.0f, 1.0f);
		}
		else
		{
			_jellyProjection = glm::ortho(-20.0f + _car->getPosition().X, _car->getPosition().X + 20.0f, -11.2f + _car->getPosition().Y, _car->getPosition().Y + 11.2f, -1.0f, 1.0f);
		}

		//check level ending
		if (_car->getChassisBody()->getAABB().contains(_levelTarget))
		{
			if (_car->getChassisBody()->contains(_levelTarget))
			{
				_gamePlayState = GamePlayState::Finish;
				_audioHelper->StopEngineSound();

				if (_time < _menuLevelManager->GetTime(_levelName))
				{
					_newTimeRecord = true;

					_menuLevelManager->SetTime(_levelName, _time);
					_menuLevelManager->SaveScores("JellyScore.xml");
				}

				if (_bestJumpLenght > _menuLevelManager->GetJump(_levelName))
				{
					_newJumpRecord = true;

					_menuLevelManager->SetJump(_levelName, _bestJumpLenght);
					_menuLevelManager->SaveScores("JellyScore.xml");
				}

				return;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			if (_car->getTire(i)->getAABB().contains(_levelTarget))
			{
				if (_car->getTire(i)->contains(_levelTarget))
				{
					_gamePlayState = GamePlayState::Finish;
					_audioHelper->StopEngineSound();

					if (_time < _menuLevelManager->GetTime(_levelName))
					{
						_newTimeRecord = true;

						_menuLevelManager->SetTime(_levelName, _time);
						_menuLevelManager->SaveScores("JellyScore.xml");
					}

					if (_bestJumpLenght > _menuLevelManager->GetJump(_levelName))
					{
						_newJumpRecord = true;

						_menuLevelManager->SetJump(_levelName, _bestJumpLenght);
						_menuLevelManager->SaveScores("JellyScore.xml");
					}

					return;
				}
			}
		}

		//check out of bounds
		if (_car->getPosition().Y < _levelLine)
		{
			if (_checkpoint)
			{
				Vector2 pos = Vector2(_checkpointPosition.x, _checkpointPosition.y);
				Vector2 scale = Vector2(1.0f, 1.0f);

				_car->getChassisBody()->setVelocity(Vector2(0, 0));
				_car->getTire(0)->setVelocity(Vector2(0, 0));
				_car->getTire(1)->setVelocity(Vector2(0, 0));

				_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
				_car->getTire(0)->setPositionAngle(Vector2(_checkpointPosition.x + 1.5f,_checkpointPosition.y - 0.3f), 0.0f, scale);
				_car->getTire(1)->setPositionAngle(Vector2(_checkpointPosition.x - 1.3f, _checkpointPosition.y - 0.3f), 0.0f, scale);
			}
			else
			{
				_gamePlayState = GamePlayState::OutOfBounds;
				_audioHelper->StopEngineSound();
				return;
			}
		}

		//check car broken
		AABB chassisAABB = _car->getChassisBody()->getAABB();
		Vector2 chassisSize = chassisAABB.Max - chassisAABB.Min;
		if ((fabsf(chassisSize.X) > 17.0f) || (fabsf(chassisSize.Y) > 17.0f))
		{
			_gamePlayState = GamePlayState::CarBroken;
			_audioHelper->StopEngineSound();
			return;
		}

		//car broken
		if (mWorld->getPenetrationCount() > 20)
		{
			carBreakCount++;
			if (carBreakCount > 5)
			{
				_gamePlayState = GamePlayState::CarBroken;
				_audioHelper->StopEngineSound();
				return;
			}
		}

		//jumping
		if (_isJumping && !_inTheAir)
		{
			//we just started jumping
			_jumpStartPosition = _car->getPosition().X;

			_inTheAir = true;
		}

		if (!_isJumping && _inTheAir)
		{
			//end of jump - check if there is new jump record
			float jumpLenght = fabsf(_jumpStartPosition - _car->getPosition().X);

			if (jumpLenght > _bestJumpLenght)
			{
				_bestJumpLenght = jumpLenght;
			}

			_inTheAir = false;
		}

		//hit sound
		if (_chassisHit > 0.0f)
		{
			if ((_chassisHit > 3.0f) && (_hitTime > 0.3f))
			{
				//play hit sound
				_audioHelper->PlayHitSound();

				//set timer
				_hitTime = 0.0f;
			}
		}

	}	
}

void JellyGame::Draw(GameManager* manager)
{
	//_core->frameObject->Bind();

	//start frame
	_renderManager->StartFrame();

	//clear screen
	_renderManager->ClearScreen();

	//render background
	_backSprite->SetScale(glm::vec2(0.1f, 0.1f));

	if (_showMap)
	{
		float x = _mapLimits.Min.X - 51.0f;
		float y = _mapLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= _mapLimits.Max.X + 51.0f; xx += 51.0f)
		{
			for (float yy = y; yy <= _mapLimits.Max.Y + 51.0f; yy += 51.0f)
			{
				_backSprite->SetPosition(glm::vec2(xx, yy));
				_backSprite->Draw(_jellyProjection);
			}
		}
	}
	else
	{
		float x = _worldLimits.Min.X - 51.0f;
		float y = _worldLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= _worldLimits.Max.X + 51.0f; xx += 51.0f)
		{
			for (float yy = y; yy <= _worldLimits.Max.Y + 51.0f; yy += 51.0f)
			{
				_backSprite->SetPosition(glm::vec2(xx, yy));
				_backSprite->Draw(_jellyProjection);
			}
		}
	}	

	//camera aabb
	Vector2 camMin = _car->getPosition() - Vector2(20.0f, 11.2f);
	Vector2 camMax = _car->getPosition() + Vector2(20.0f, 11.2f);
	JellyPhysics::AABB camAABB(camMin, camMax);

	//ignore aabb
	Vector2 ignoreMin = _car->getPosition() - Vector2(40.0f, 22.4f);
	Vector2 ignoreMax = _car->getPosition() + Vector2(40.0f, 22.4f);
	JellyPhysics::AABB ignoreAABB(ignoreMin, ignoreMax);

	//render level bodies
	for (size_t i = 0; i < _gameBodies.size(); i++)
	{
		if (_showMap)
		{
			_gameBodies[i]->Draw(_jellyProjection);

			if (ignoreAABB.intersects(_gameBodies[i]->GetIgnoreAABB()))
			{
				_gameBodies[i]->SetIgnore(false);
			}
			else
			{
				_gameBodies[i]->SetIgnore(true);
			}
		}
		else
		{
			if (ignoreAABB.intersects(_gameBodies[i]->GetIgnoreAABB()))
			{
				_gameBodies[i]->SetIgnore(false);

				if (_gameBodies[i]->GetIgnoreAABB().intersects(camAABB))
				{
					_gameBodies[i]->Draw(_jellyProjection);
				}
			}
			else
			{
				_gameBodies[i]->SetIgnore(true);
			}
		}
	}

	//car
	_car->Draw(_jellyProjection);

	//timer
	char bufferTime[10];
	sprintf(bufferTime,"%.2f", _time);
	_menuFont->AddText(std::string(bufferTime) + "s", _renderManager->GetWidth() - 125, 50 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	_menuFont->AddText(std::string(bufferTime) + "s", _renderManager->GetWidth() - 125, 50, glm::vec3(0.0f, 0.84f, 0.0f), FontLeft);

	//jumping
	char bufferJump[10];
	sprintf(bufferJump, "%.2f", _bestJumpLenght);
	_menuFont->AddText(std::string(bufferJump) + "m", _renderManager->GetWidth() - 125, 80 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	_menuFont->AddText(std::string(bufferJump) + "m", _renderManager->GetWidth() - 125, 80, glm::vec3(0.71f, 0.16f, 0.18f), FontLeft);

	//sound debug
	//if ((_chassisHit > 3.0f))
	//{
	//	_menuFont->AddText("SOUND", _renderManager->GetWidth() - 525, 50 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	//}

	//ballon - tire
	for (size_t i = 0; i < _gameBodies.size(); i++)
	{
		if (!_gameBodies[i]->GetBody()->getDisable())
		{
			if (_gameBodies[i]->GetName() == "itemballoon")
			{
				//draw ballon
				_ballonSprite->SetScale(glm::vec2(0.02f, -0.02f));
				_ballonSprite->SetPosition(_gameBodies[i]->GetStartPosition());
				_ballonSprite->Draw(_jellyProjection);
			}
			else if (_gameBodies[i]->GetName() == "itemstick")
			{
				_tireSprite->SetScale(glm::vec2(0.02f, 0.02f));
				_tireSprite->SetPosition(_gameBodies[i]->GetStartPosition());
				_tireSprite->Draw(_jellyProjection);
			}
		}		
	}

	if (!_showMap && _haveBallon && _ballonTime > 0.0f)
	{
		float step = 100.0f / 30.0f;
		float scale = (_ballonTime * step) / 100.0f;

		_ballonSpriteBack->SetScale(glm::vec2(scale, scale));
		_ballonSpriteBack->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
		_ballonSpriteBack->SetPosition(glm::vec2((_renderManager->GetWidth() / 2) + 128, 450));
		_ballonSpriteBack->Draw(_projection);

		_ballonSprite->SetScale(glm::vec2(scale, scale));
		//_ballonSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
		_ballonSprite->SetPosition(glm::vec2((_renderManager->GetWidth() / 2) + 128,450));
		_ballonSprite->Draw(_projection);
	}

	if (!_showMap && _haveTire && _tireTime > 0.0f)
	{
		float step = 100.0f / 30.0f;
		float scale = (_tireTime * step) / 100.0f;

		_tireSpriteBack->SetScale(glm::vec2(scale, scale));
		_tireSpriteBack->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
		_tireSpriteBack->SetPosition(glm::vec2((_renderManager->GetWidth() / 2) - 128, 450));
		_tireSpriteBack->Draw(_projection);

		_tireSprite->SetScale(glm::vec2(scale, scale));
		//_tireSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
		_tireSprite->SetPosition(glm::vec2((_renderManager->GetWidth() / 2) - 128, 450));
		_tireSprite->Draw(_projection);
	}

	//end level target
	_targetSprite->Draw(_jellyProjection);

	if (_gamePlayState == GamePlayState::Paused)
	{
		if (_checkpoint)
		{
			_menuFont->AddText("Pause", _renderManager->GetWidth() / 2, _renderManager->GetHeight() / 2  - 72+ 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_menuFont->AddText("Pause", _renderManager->GetWidth() / 2, _renderManager->GetHeight() / 2 - 72, glm::vec3(0.71f, 0.16f, 0.18f), FontCenter);
			
			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), _renderManager->GetHeight() / 2  - 42));
			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->Draw(_projection);

			_menuFont->AddText("Resume", (_renderManager->GetWidth() / 2) + 50, _renderManager->GetHeight() / 2 - 32, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

			_inputHelper->CarActionSprite(CarAction::Tire)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), _renderManager->GetHeight() / 2 - 2));
			_inputHelper->CarActionSprite(CarAction::Tire)->Draw(_projection);

			_menuFont->AddText("Go to checkpoint", (_renderManager->GetWidth() / 2) + 50, _renderManager->GetHeight() / 2 + 8, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
		}
		else
		{
			_menuFont->AddText("Pause", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 40 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_menuFont->AddText("Pause", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 40, glm::vec3(0.71f, 0.16f, 0.18f), FontCenter);

			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), (_renderManager->GetHeight() / 2) - 2));
			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->Draw(_projection);

			_menuFont->AddText("Resume", (_renderManager->GetWidth() / 2) + 50, (_renderManager->GetHeight() / 2) + 8, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
		}

		_inputHelper->MenuActionSprite(MenuAction::MenuPause)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), _renderManager->GetHeight() / 2 + 38));
		_inputHelper->MenuActionSprite(MenuAction::MenuPause)->Draw(_projection);

		_menuFont->AddText("Options", (_renderManager->GetWidth() / 2) + 50, _renderManager->GetHeight() / 2 + 48, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

		_inputHelper->MenuActionSprite(MenuAction::MenuExit)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), _renderManager->GetHeight() / 2 + 78));
		_inputHelper->MenuActionSprite(MenuAction::MenuExit)->Draw(_projection);

		_menuFont->AddText("Main menu", (_renderManager->GetWidth() / 2) + 50, _renderManager->GetHeight() / 2 + 88, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	}



	if (_gamePlayState == GamePlayState::Finish)
	{
		_menuFont->AddText("Level finished", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 120 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
		_menuFont->AddText("Level finished", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 120, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

		if (_newJumpRecord && _newTimeRecord)
		{
			_menuFont->AddText("New Jump Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_menuFont->AddText("New Jump Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60, glm::vec3(0.71f, 0.16f, 0.18f), FontCenter);

			_menuFont->AddText("New Time Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_menuFont->AddText("New Time Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2), glm::vec3(0.0f, 0.84f, 0.0f), FontCenter);
		}
		else if (_newJumpRecord)
		{
			_menuFont->AddText("New Jump Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_menuFont->AddText("New Jump Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60, glm::vec3(0.71f, 0.16f, 0.18f), FontCenter);
		}
		else if (_newTimeRecord)
		{
			_menuFont->AddText("New Time Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_menuFont->AddText("New Time Record!", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60, glm::vec3(0.0f, 0.84f, 0.0f), FontCenter);
		}
	}
	else if (_gamePlayState == GamePlayState::CarBroken)
	{
		_menuFont->AddText("Car broken", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
		_menuFont->AddText("Car broken", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

	}
	else if (_gamePlayState == GamePlayState::OutOfBounds)
	{
		_menuFont->AddText("Out of level", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
		_menuFont->AddText("Out of level", _renderManager->GetWidth() / 2, (_renderManager->GetHeight() / 2) - 60, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

	}

	//end level text
	if (_gamePlayState != GamePlayState::Play && _gamePlayState != GamePlayState::Paused)
	{
		if (_newJumpRecord && _newTimeRecord)
		{
			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), _renderManager->GetHeight() / 2 + 38));
			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->Draw(_projection);
			_menuFont->AddText("Main menu", (_renderManager->GetWidth() / 2) + 50, _renderManager->GetHeight() / 2 + 48, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
		}
		else
		{
			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), _renderManager->GetHeight() / 2 - 2));
			_inputHelper->MenuActionSprite(MenuAction::MenuBack)->Draw(_projection);
			_menuFont->AddText("Main menu", (_renderManager->GetWidth() / 2) + 50, _renderManager->GetHeight() / 2 + 8, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
		}
	}

	//draw main text
	_menuFont->Draw(_projection);

	//_transformMeter
	if (mTransformMeter >= 0.0f)
	{
		float test = 1.0f - mTransformMeter;
		_transformMeter->SetPosition(glm::vec2(128 - (test * 64.0f),40.0f));
		_transformMeter->SetSize(glm::vec2(128 * mTransformMeter, 32));
		_transformMeter->Draw(_projection);
	}

	//end render to texture
	/*_core->frameObject->UnBind();

	//render normal
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_core->renderSprite->Draw(_projection);*/

	//end frame
	_renderManager->EndFrame();
}

