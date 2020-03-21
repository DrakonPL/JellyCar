#include "InputHelper.h"
#include <Andromeda/Utils/Logger.h>
#include <Andromeda/FileSystem/FileManager.h>
#include "tinyxml.h"

using namespace Andromeda;

InputHelper* InputHelper::_inputHelper = 0;

void InputHelper::PrepareKeyboardMapping()
{
	//menu input
	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuDown, Key::Code::Down));
	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuUp, Key::Code::Up));
	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuLeft, Key::Code::Left));
	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuRight, Key::Code::Right));

	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuAccept, Key::Code::S));
	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuBack, Key::Code::D));

	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuPause, Key::Code::Return));
	_menuKeyboardMapping.insert(std::pair<MenuAction, Key::Code>(MenuAction::MenuExit, Key::Code::Space));


	//car input
	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::Left, Key::Code::Left));
	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::Right, Key::Code::Right));

	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::Transform, Key::Code::S));
	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::Ballon, Key::Code::D));
	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::Tire, Key::Code::A));
	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::Map, Key::Code::W));

	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::RotateLeft, Key::Code::Q));
	_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(CarAction::RotateRight, Key::Code::E));

	//all keys
	for (int key = 0; key < Key::Code::Count; key++)
	{
		Key::Code code = static_cast<Key::Code>(key);
		_keyPresseddMapping.insert(std::pair<Key::Code, bool>(code, false));
	}
}

void InputHelper::PrepareGamepadMapping()
{
	//menu input
    if (_mode == InputActionMode::SWITCH)
    {
		_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuAccept, Gamepad::Button::Circle));
		_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuBack, Gamepad::Button::Cross));
    }else if(_mode == InputActionMode::VITA)
    {
		_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuAccept, Gamepad::Button::Cross));
		_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuBack, Gamepad::Button::Circle));
    }else
    {
		_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuAccept, Gamepad::Button::Circle));
		_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuBack, Gamepad::Button::Cross));
    }

	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuAccept, Gamepad::Button::Circle));
	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuBack, Gamepad::Button::Cross));

	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuDown, Gamepad::Button::Down));
	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuUp, Gamepad::Button::Up));
	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuLeft, Gamepad::Button::Left));
	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuRight, Gamepad::Button::Right));

	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuPause, Gamepad::Button::Select));
	_menuGamepadMapping.insert(std::pair<MenuAction, Gamepad::Button>(MenuAction::MenuExit, Gamepad::Button::Start));

	//car input
	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::Left, Gamepad::Button::Left));
	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::Right, Gamepad::Button::Right));

	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::RotateLeft, Gamepad::Button::LTrigger));
	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::RotateRight, Gamepad::Button::RTrigger));

	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::Transform, Gamepad::Button::Cross));
	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::Ballon, Gamepad::Button::Circle));
	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::Tire, Gamepad::Button::Square));
	_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(CarAction::Map, Gamepad::Button::Triangle));


	//all gamepad buttons
	for (int key = 0; key < Gamepad::Button::Count; key++)
	{
		Gamepad::Button button = static_cast<Gamepad::Button>(key);
		_buttonPressed.insert(std::pair<Gamepad::Button, bool>(button, false));
	}

	//menu mapping
	//_menuMapping.in===
	//std::vector<Gamepad::Button> upButtons;

	//upButtons.push_back(Gamepad::Button::Up);
	//upButtons.push_back(Gamepad::Button::LAnalogUp);

	//_menuMapping.insert(std::pair<CarAction, std::vector< Gamepad::Button>>(CarAction::Up,upButtons));
}

void InputHelper::PrepareNameKeyboardMapping()
{
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::W, "W"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::D, "D"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::S, "S"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::A, "A"));

	//left,right...
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::Left, "Left"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::Right, "Right"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::Up, "Up"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::Down, "Down"));

	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::Q, "Q"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::E, "E"));

	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::Space, "Space"));
	_nameKeyboardMapping.insert(std::pair<Key::Code, std::string>(Key::Code::Return, "Return"));

}

void InputHelper::PrepareNameGamepadMapping()
{

}

void InputHelper::PrepareSpriteGamepadMapping()
{
	_spriteShader = ShaderManager::Instance()->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	std::string imageFile = "Assets/Images/buttons_switch.png";

#ifdef ANDROMEDA_VITA
	imageFile = "Assets/Images/buttons_vita.png";
#endif

#ifdef ANDROMEDA_SWITCH
	imageFile = "Assets/Images/buttons_switch.png";
#endif

	//shapes
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Triangle, new Sprite("_control1", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(17.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Circle, new Sprite("_control2", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(102.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Cross, new Sprite("_control3", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(186.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Square, new Sprite("_control4", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(273.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));

	//left,right...
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Left, new Sprite("_control5", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(313.0f, 88.0f), glm::vec2(88.0f, 68.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Right, new Sprite("_control6", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(413, 82.0f), glm::vec2(88.0f, 68.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Up, new Sprite("_control7", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(159.0f, 94.0f), glm::vec2(68.0f, 88.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Down, new Sprite("_control8", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(239.0f, 94.0f), glm::vec2(68.0f, 88.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LTrigger, new Sprite("_control9", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(6.0f, 87.0f), glm::vec2(128.0f, 60.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RTrigger, new Sprite("_control10", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(369.0f, 11.0f), glm::vec2(128.0f, 60.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Select, new Sprite("_control11", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(159.0f, 193.0f), glm::vec2(74.0f, 38.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Start, new Sprite("_control12", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(239.0f, 194.0f), glm::vec2(74.0f, 38.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogLeft, new Sprite("_control13", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(3.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogRight, new Sprite("_control14", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(121.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogUp, new Sprite("_control15", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(242.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogDown, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(361.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogPushed, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(63.0f, 152.0f), glm::vec2(89.0f, 89.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogPushed, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(377.0f, 155.0f), glm::vec2(89.0f, 89.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogLeft, new Sprite("_control13", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(3.0f, 369.0f), glm::vec2(116.0f, 116.0f))));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogRight, new Sprite("_control14", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(122.0f, 369.0f), glm::vec2(116.0f, 116.0f))));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogUp, new Sprite("_control15", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(242.0f, 369.0f), glm::vec2(116.0f, 116.0f))));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogDown, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(361.0f, 369.0f), glm::vec2(116.0f, 116.0f))));

	_spriteGamepadMapping[Gamepad::Button::Triangle]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Circle]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Cross]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Square]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteGamepadMapping[Gamepad::Button::Left]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Right]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Up]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Down]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteGamepadMapping[Gamepad::Button::LTrigger]->SetScale(glm::vec2(0.5f, 0.5f));
	_spriteGamepadMapping[Gamepad::Button::RTrigger]->SetScale(glm::vec2(0.5f, 0.5f));

	_spriteGamepadMapping[Gamepad::Button::LAnalogPushed]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogPushed]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteGamepadMapping[Gamepad::Button::LAnalogLeft]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::LAnalogRight]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::LAnalogUp]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::LAnalogDown]->SetScale(glm::vec2(0.35f, 0.35f));

	_spriteGamepadMapping[Gamepad::Button::RAnalogLeft]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogRight]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogUp]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogDown]->SetScale(glm::vec2(0.35f, 0.35f));
}

void InputHelper::PrepareSpriteKeyboardMapping()
{
	_spriteShader = ShaderManager::Instance()->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	std::string imageFile = "Assets/Images/buttons_vita.png";

#ifdef ANDROMEDA_VITA
	imageFile = "Assets/Images/buttons_vita.png";
#endif

#ifdef ANDROMEDA_SWITCH
	imageFile = "Assets/Images/buttons_switch.png";
#endif

	//shapes
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::W, new Sprite("_control1", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(17.0f, 5.0f), glm::vec2(78.0f, 78.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::D, new Sprite("_control2", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(102.0f, 5.0f), glm::vec2(78.0f, 78.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::S, new Sprite("_control3", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(186.0f, 5.0f), glm::vec2(78.0f, 78.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::A, new Sprite("_control4", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(273.0f, 5.0f), glm::vec2(78.0f, 78.0f))));

	//left,right...
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Left, new Sprite("_control5", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(313.0f, 88.0f), glm::vec2(88.0f, 68.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Right, new Sprite("_control6", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(413, 82.0f), glm::vec2(88.0f, 68.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Up, new Sprite("_control7", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(159.0f, 94.0f), glm::vec2(68.0f, 88.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Down, new Sprite("_control8", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(239.0f, 94.0f), glm::vec2(68.0f, 88.0f))));

	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Q, new Sprite("_control9", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(6.0f, 87.0f), glm::vec2(128.0f, 60.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::E, new Sprite("_control10", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(369.0f, 11.0f), glm::vec2(128.0f, 60.0f))));

	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Space, new Sprite("_control11", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(159.0f, 193.0f), glm::vec2(74.0f, 38.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Return, new Sprite("_control12", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::NearestFilter), _spriteShader, glm::vec2(239.0f, 194.0f), glm::vec2(74.0f, 38.0f))));


	_spriteKeyboardMapping[Key::Code::Left]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteKeyboardMapping[Key::Code::Right]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteKeyboardMapping[Key::Code::Up]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteKeyboardMapping[Key::Code::Down]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteKeyboardMapping[Key::Code::W]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteKeyboardMapping[Key::Code::S]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteKeyboardMapping[Key::Code::A]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteKeyboardMapping[Key::Code::D]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteKeyboardMapping[Key::Code::Q]->SetScale(glm::vec2(0.5f, 0.5f));
	_spriteKeyboardMapping[Key::Code::E]->SetScale(glm::vec2(0.5f, 0.5f));
}

InputHelper::InputHelper()
{
	_inputManager = InputManager::Instance();

	if (_inputManager->GetKayboardCount() > 0)
	{
		_keyboard = _inputManager->GetKeyboardDevice(0);
		_mode = InputActionMode::PC;

		//check if we can use gamepad
		if (_inputManager->GetGamepadCount() > 0)
		{
			_gamepad = _inputManager->GetGamepadDevice(0);
			_mode = InputActionMode::PCGamepad;
		}
	}
	else
	{
		_gamepad = _inputManager->GetGamepadDevice(0);

#ifdef ANDROMEDA_SWITCH
		_mode = InputActionMode::SWITCH;
#endif

#ifdef ANDROMEDA_VITA
		_mode = InputActionMode::VITA;
#endif		
	}

	for (size_t i = 0; i < CarAction::Count; i++)
	{
		_carActionPressed.insert(std::pair<CarAction, bool>(static_cast<CarAction>(i), false));
	}

	for (size_t i = 0; i < MenuAction::MenuCount; i++)
	{
		_menuActionPressed.insert(std::pair<MenuAction, bool>(static_cast<MenuAction>(i), false));
	}

	if (_mode == InputActionMode::PC)
	{
		PrepareKeyboardMapping();
		PrepareSpriteKeyboardMapping();
		PrepareNameKeyboardMapping();
	}
	else
	{
		PrepareGamepadMapping();
		PrepareSpriteGamepadMapping();
	}	

	//touch devices
	if (_mode == InputActionMode::PC || _mode == InputActionMode::PCGamepad)
	{
		_mouseDevice = _inputManager->GetMouseDevice(0);
	}else
	{
		_touchDevice = _inputManager->GetTouchDevice(0);
	}

	//try load settings
	LoadSettings();

	_updateAll = false;
}

InputHelper* InputHelper::Instance()
{
	if (_inputHelper == 0)
	{
		_inputHelper = new InputHelper();
	}

	return _inputHelper;
}

InputHelper::~InputHelper()
{
	//_actionPressed.clear();
	//_keyboardMapping.clear();
	//_gamepadMapping.clear();

	{
		std::map<Gamepad::Button, Sprite*>::iterator iter;

		for (iter = _spriteGamepadMapping.begin(); iter != _spriteGamepadMapping.end(); ++iter)
		{
			delete iter->second;
		}

		_spriteGamepadMapping.clear();
	}

	{
		std::map<Key::Code, Sprite*>::iterator iter;

		for (iter = _spriteKeyboardMapping.begin(); iter != _spriteKeyboardMapping.end(); ++iter)
		{
			delete iter->second;
		}

		_spriteKeyboardMapping.clear();
	}
}

bool InputHelper::ActionPressed(CarAction action)
{
	if (_mode == InputActionMode::PC)
	{
		if (_keyboard->KeyDown(_carKeyboardMapping[action]) && !_carActionPressed[action])
		{
			_carActionPressed[action] = true;
			return true;
		}
	}
	else
	{
		if (_gamepad->KeyDown(_carGamepadMapping[action]) && !_carActionPressed[action])
		{
			_carActionPressed[action] = true;
			return true;
		}
	}

	return false;
}

bool InputHelper::MenuActionPressed(MenuAction action)
{
	if (_mode == InputActionMode::PC)
	{
		if (_keyboard->KeyDown(_menuKeyboardMapping[action]) && !_menuActionPressed[action])
		{
			_menuActionPressed[action] = true;
			return true;
		}
	}
	else
	{
		if (_gamepad->KeyDown(_menuGamepadMapping[action]) && !_menuActionPressed[action])
		{
			_menuActionPressed[action] = true;
			return true;
		}
	}

	return false;
}

bool InputHelper::ActionHold(CarAction action)
{
	if (_mode == InputActionMode::PC)
	{
		if (_keyboard->KeyDown(_carKeyboardMapping[action]))
		{
			return true;
		}
	}
	else
	{
		if (_gamepad->KeyDown(_carGamepadMapping[action]))
		{
			return true;
		}
	}

	return false;
}

int InputHelper::GetTouchCount()
{
	if (_mode == InputActionMode::PC || _mode == InputActionMode::PCGamepad)
	{
		if (_mouseDevice->ButtonDown(Mouse::Button::Left))
		{
			return 1;
		}

		return 0;
	}else
	{
		return _touchDevice->GetTouchCount();
	}
}

glm::vec2 InputHelper::GetTouchPosition(int touch)
{
	if (_mode == InputActionMode::PC || _mode == InputActionMode::PCGamepad)
	{
		int x = _mouseDevice->GetPosX();
		int y = _mouseDevice->GetPosY();

		return glm::vec2(x, y);
	}
	else
	{
        if (_mode == InputActionMode::SWITCH)
        {
			return glm::vec2(_touchDevice->GetTouchX(touch), _touchDevice->GetTouchY(touch));
        }

		if (_mode == InputActionMode::VITA)
		{
			float x = ((float)_touchDevice->GetTouchX(touch) / 1920.0f) * RenderManager::Instance()->GetWidth();
			float y = ((float)_touchDevice->GetTouchY(touch) / 1088.0f) * RenderManager::Instance()->GetHeight();

			return glm::vec2(x, y);
		}

		return glm::vec2(0, 0);
	}
}

void InputHelper::UpdateAllInputs(bool state)
{
	_updateAll = true;
}

void InputHelper::Update()
{
	for (size_t i = 0; i < CarAction::Count; i++)
	{
		CarAction action = static_cast<CarAction>(i);
		if (_mode == InputActionMode::PC)
		{
			// check action
			if (_keyboard->KeyUp(_carKeyboardMapping[action]))
			{
				_carActionPressed[action] = false;
			}else
			{
				_carActionPressed[action] = true;
			}				
		}
		else
		{
			if (_gamepad->KeyUp(_carGamepadMapping[action]))
			{
				_carActionPressed[action] = false;
			}else
			{
				_carActionPressed[action] = true;
			}			
		}
	}

	for (size_t i = 0; i < MenuAction::MenuCount; i++)
	{
		MenuAction action = static_cast<MenuAction>(i);
		if (_mode == InputActionMode::PC)
		{
			// check action
			if (_keyboard->KeyUp(_menuKeyboardMapping[action]))
			{
				_menuActionPressed[action] = false;
			}
			else
			{
				_menuActionPressed[action] = true;
			}
		}
		else
		{
			if (_gamepad->KeyUp(_menuGamepadMapping[action]))
			{
				_menuActionPressed[action] = false;
			}
			else
			{
				_menuActionPressed[action] = true;
			}
		}
	}

	if (_updateAll)
	{
		if (_mode == InputActionMode::PC)
		{
			//check all keys
			for (int key = 0; key < Key::Code::Count; key++)
			{
				Key::Code code = static_cast<Key::Code>(key);
				if (_keyboard->KeyUp(code))
				{
					_keyPresseddMapping[code] = false;
				}
				else
				{
					_keyPresseddMapping[code] = true;
				}
			}
		}
		else
		{
			for (int key = 0; key < Gamepad::Button::Count; key++)
			{
				Gamepad::Button button = static_cast<Gamepad::Button>(key);
				if (_gamepad->KeyUp(button))
				{
					_buttonPressed[button] = false;
				}
				else
				{
					_buttonPressed[button] = true;
				}
			}
		}		
	}
}

bool InputHelper::ChangeInputForAction(CarAction action)
{
	if (_mode == InputActionMode::PC)
	{
		for (int key = 0; key < Key::Code::Count; key++)
		{
			Key::Code code = static_cast<Key::Code>(key);

			if (_keyboard->KeyDown(code) && !_keyPresseddMapping[code])
			{
				_carKeyboardMapping[action] = code;
				_updateAll = false;
				return true;
			}			
		}		
	}
	else
	{
		for (int key = 0; key < Gamepad::Button::Count; key++)
		{
			Gamepad::Button button = static_cast<Gamepad::Button>(key);

			if (_gamepad->KeyDown(button) && !_buttonPressed[button])
			{
				_carGamepadMapping[action] = button;
				_updateAll = false;
				return true;
			}
		}		
	}

	return false;
}

Sprite* InputHelper::CarActionSprite(CarAction action)
{
	if (_mode == InputActionMode::PC)
	{
		return _spriteKeyboardMapping[_carKeyboardMapping[action]];
	}
	else
	{
		return _spriteGamepadMapping[_carGamepadMapping[action]];
	}

	return nullptr;
}

Sprite* InputHelper::MenuActionSprite(MenuAction action)
{
	if (_mode == InputActionMode::PC)
	{
		return _spriteKeyboardMapping[_menuKeyboardMapping[action]];
	}
	else
	{
		return _spriteGamepadMapping[_menuGamepadMapping[action]];
	}

	return nullptr;
}


std::string InputHelper::InputName(CarAction action)
{
	if (_mode == InputActionMode::PC)
	{
		return _nameKeyboardMapping[_carKeyboardMapping[action]];
	}
	else
	{
		return _nameGamepadMapping[_carGamepadMapping[action]];
	}

	return "";
}

glm::vec2 InputHelper::TouchToScreen(glm::vec4 screenBound, glm::vec2 touch)
{
	float width = fabsf(screenBound.x) + fabsf(screenBound.y);
	float widthFactor = width / (float)RenderManager::Instance()->GetWidth();
	float dragX = (touch.x * widthFactor) + screenBound.x;

	float height = fabsf(screenBound.w) + fabsf(screenBound.z);
	float heightFactor = height / (float)RenderManager::Instance()->GetHeight();
	
	float dragY = 0.0f;

	if(screenBound.w < screenBound.z)
		dragY = (touch.y * heightFactor) + screenBound.w;
	else
	{
		touch.y = (float)RenderManager::Instance()->GetHeight() - touch.y;
		dragY = (touch.y * heightFactor) + screenBound.z;
	}

	return glm::vec2(dragX, dragY);
}

std::map<int, int> InputHelper::GetActionKeyMapping()
{
	std::map<int, int> _actionKeyMapping;

	for (size_t i = 0; i < CarAction::Count; i++)
	{
		CarAction action = static_cast<CarAction>(i);
		if (_mode == InputActionMode::PC)
		{
			_actionKeyMapping.insert(std::pair<int, int>(i, _carKeyboardMapping[action]));
		}
		else
		{
			_actionKeyMapping.insert(std::pair<int, int>(i, _carGamepadMapping[action]));
		}
	}

	return _actionKeyMapping;
}


void InputHelper::SetActionKeyMapping(std::map<int, int> mapping)
{
	if (_mode == InputActionMode::PC)
	{
		_carKeyboardMapping.clear();
	}
	else
	{
		_carGamepadMapping.clear();
	}

	typedef std::map<int, int>::iterator it_type;
	for (it_type iterator = mapping.begin(); iterator != mapping.end(); iterator++)
	{
		CarAction action = static_cast<CarAction>(iterator->first);
		
		if (_mode == InputActionMode::PC)
		{
			Key::Code code = static_cast<Key::Code>(iterator->second);
			_carKeyboardMapping.insert(std::pair<CarAction, Key::Code>(action, code));
		}
		else
		{

			Gamepad::Button button = static_cast<Gamepad::Button>(iterator->second);
			_carGamepadMapping.insert(std::pair<CarAction, Gamepad::Button>(action, button));
		}
	}
}


void InputHelper::LoadSettings()
{
	std::string fileName = "JellyKeySettings.xml";

	//loac main level file
	Andromeda::FileSystem::BaseFile* file = Andromeda::FileSystem::FileManager::Instance()->GetFile(fileName, true);

	if (file == 0)
		return;

	file->Open(FileSystem::Read, FileSystem::Binary);

	if (!file->Exist())
	{
		delete file;
		return;
	}


	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	int bodyNumber = 0;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	std::map<int, int> actionKeyMapping;

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Settings").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		int action = atof(ObjectNode->Attribute("action"));
		int key = atof(ObjectNode->Attribute("key"));

		actionKeyMapping.insert(std::pair<int, int>(action, key));
	}

	if (actionKeyMapping.size() > 0)
	{
		SetActionKeyMapping(actionKeyMapping);
	}
}

void InputHelper::SaveSettings()
{
	std::string fileName = "JellyKeySettings.xml";

	std::string saveFile = Andromeda::FileSystem::FileManager::Instance()->GetSaveDirPath() + fileName;

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Settings");
	doc.LinkEndChild(root);

	//get mapping
	std::map<int, int> actionKeyMapping = GetActionKeyMapping();

	typedef std::map<int, int>::iterator it_type;
	for (it_type iterator = actionKeyMapping.begin(); iterator != actionKeyMapping.end(); iterator++)
	{
		TiXmlElement * cxn = new TiXmlElement("ActionMapping");
		root->LinkEndChild(cxn);
		cxn->SetDoubleAttribute("action", iterator->first);
		cxn->SetDoubleAttribute("key", iterator->second);
	}

	doc.SaveFile(saveFile.c_str());
}

