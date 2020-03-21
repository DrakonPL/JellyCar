#include "JellySplash.h"
#include "JellyIntro.h"

void JellySplash::Init()
{
	_renderManager = RenderManager::Instance();
	_shaderManager = ShaderManager::Instance();
	_textureManager = TextureManager::Instance();

	_renderManager->SetClearColor(0x00000000);

	//load shader
	_shader = _shaderManager->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);


	std::string splashFile = "Assets/Images/splash_switch.png";

#ifdef ANDROMEDA_VITA

	splashFile = "Assets/Images/splash_vita.png";

#endif

#ifdef ANDROMEDA_SWITCH

	splashFile = "Assets/Images/splash_switch.png";

#endif

	_image = _textureManager->LoadFromFile(splashFile);



	//sprite
	_sprite = new Sprite("splash", _image, _shader);
	_sprite->SetPosition(glm::vec2(_renderManager->GetWidth() / 2, _renderManager->GetHeight() / 2));

	//projections
	_projection = glm::ortho(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f, -1.0f, 1.0f);

	_dt = 0;
	_splashTimer = 0;
	_alpha = 0;

	_timer = new Timer();

	_sprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

	_end = false;
}

void JellySplash::Enter()
{

}

void JellySplash::CleanUp()
{
	delete _sprite;
}

void JellySplash::Pause()
{

}

void JellySplash::Resume()
{

}

void JellySplash::GamePause()
{

}

void JellySplash::GameResume()
{

}

void JellySplash::HandleEvents(GameManager* manager)
{

}

void JellySplash::Update(GameManager* manager)
{
	if (_end)
	{
		manager->Quit();
		return;
	}	


	_dt = _timer->GetDelta();

	_splashTimer += _dt;

	if (_splashTimer < 2.0)
	{
		_alpha += _dt;
		if (_alpha >= 1.0f)
		{
			_alpha = 1.0f;
		}
	}

	if (_splashTimer >= 3.0 && _splashTimer <= 4.0)
	{
		_alpha -= _dt;
		if (_alpha < 0.0f)
		{
			_alpha = 0.0f;
		}
	}

	if (_splashTimer >= 4.0)
	{
		JellyIntro* jelly = new JellyIntro();
		//jelly->Init();

		manager->PushState(jelly);
		_end = true;
	}

	_sprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alpha));
}

void JellySplash::Draw(GameManager* manager)
{
	if (_end)
		return;

	//start frame
	_renderManager->StartFrame();

	//clear screen
	_renderManager->ClearScreen();

	_sprite->Draw(_projection);

	//end frame
	_renderManager->EndFrame();
}