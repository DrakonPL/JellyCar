#include "InputHelper.h"
#include <Andromeda/Utils/Logger.h>
#include <Andromeda/FileSystem/FileManager.h>
#include "tinyxml.h"

using namespace Andromeda;

InputHelper* InputHelper::_inputHelper = 0;

void InputHelper::PrepareKeyboardMapping()
{
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Down, Key::Code::Down));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Up, Key::Code::Up));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Left, Key::Code::Left));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Right, Key::Code::Right));

	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Accept, Key::Code::S));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Back, Key::Code::D));

	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Transform, Key::Code::S));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Ballon, Key::Code::D));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Tire, Key::Code::A));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Map, Key::Code::W));

	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::RotateLeft, Key::Code::Q));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::RotateRight, Key::Code::E));

	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Pause, Key::Code::Return));
	_keyboardMapping.insert(std::pair<InputAction, Key::Code>(InputAction::Exit, Key::Code::Space));

	for (int key = 0; key < Key::Code::Count; key++)
	{
		Key::Code code = static_cast<Key::Code>(key);
		_keyPresseddMapping.insert(std::pair<Key::Code, bool>(code, false));
	}
}

void InputHelper::PrepareGamepadMapping()
{
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Accept, Gamepad::Button::Cross));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Back, Gamepad::Button::Circle));

	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Transform, Gamepad::Button::Cross));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Ballon, Gamepad::Button::Circle));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Tire, Gamepad::Button::Square));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Map, Gamepad::Button::Triangle));

	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Down, Gamepad::Button::Down));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Up, Gamepad::Button::Up));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Left, Gamepad::Button::Left));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Right, Gamepad::Button::Right));

	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::RotateLeft, Gamepad::Button::LTrigger));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::RotateRight, Gamepad::Button::RTrigger));

	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Pause, Gamepad::Button::Start));
	_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(InputAction::Exit, Gamepad::Button::Select));

	for (int key = 0; key < Gamepad::Button::Count; key++)
	{
		Gamepad::Button button = static_cast<Gamepad::Button>(key);
		_buttonPressed.insert(std::pair<Gamepad::Button, bool>(button, false));
	}
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

	//shapes
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Triangle, new Sprite("_control1", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(17.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Circle, new Sprite("_control2", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(102.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Cross, new Sprite("_control3", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(186.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Square, new Sprite("_control4", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(273.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));

	//left,right...
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Left, new Sprite("_control5", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(313.0f, 88.0f), glm::vec2(88.0f, 68.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Right, new Sprite("_control6", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(413, 82.0f), glm::vec2(88.0f, 68.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Up, new Sprite("_control7", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(159.0f, 94.0f), glm::vec2(68.0f, 88.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Down, new Sprite("_control8", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(239.0f, 94.0f), glm::vec2(68.0f, 88.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LTrigger, new Sprite("_control9", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(6.0f, 87.0f), glm::vec2(128.0f, 60.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RTrigger, new Sprite("_control10", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(369.0f, 11.0f), glm::vec2(128.0f, 60.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Select, new Sprite("_control11", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(303.0f, 180.0f), glm::vec2(74.0f, 38.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Start, new Sprite("_control12", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(431.0f, 180.0f), glm::vec2(74.0f, 38.0f)) ));

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
}

void InputHelper::PrepareSpriteKeyboardMapping()
{
	_spriteShader = ShaderManager::Instance()->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	//shapes
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::W, new Sprite("_control1", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(17.0f, 5.0f), glm::vec2(78.0f, 78.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::D, new Sprite("_control2", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(102.0f, 5.0f), glm::vec2(78.0f, 78.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::S, new Sprite("_control3", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(186.0f, 5.0f), glm::vec2(78.0f, 78.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::A, new Sprite("_control4", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(273.0f, 5.0f), glm::vec2(78.0f, 78.0f))));

	//left,right...
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Left, new Sprite("_control5", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(313.0f, 88.0f), glm::vec2(88.0f, 68.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Right, new Sprite("_control6", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(413, 82.0f), glm::vec2(88.0f, 68.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Up, new Sprite("_control7", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(159.0f, 94.0f), glm::vec2(68.0f, 88.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Down, new Sprite("_control8", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(239.0f, 94.0f), glm::vec2(68.0f, 88.0f))));

	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Q, new Sprite("_control9", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(6.0f, 87.0f), glm::vec2(128.0f, 60.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::E, new Sprite("_control10", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(369.0f, 11.0f), glm::vec2(128.0f, 60.0f))));

	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Space, new Sprite("_control11", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(303.0f, 180.0f), glm::vec2(74.0f, 38.0f))));
	_spriteKeyboardMapping.insert(std::pair<Key::Code, Sprite*>(Key::Code::Return, new Sprite("_control12", TextureManager::Instance()->LoadFromFile("Assets/Images/vita_buttons.png", TextureFilerType::NearestFilter), _spriteShader, glm::vec2(431.0f, 180.0f), glm::vec2(74.0f, 38.0f))));

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
		_mode = InputActionMode::VITA;
	}

	for (size_t i = 0; i < InputAction::Count; i++)
	{
		_actionPressed.insert(std::pair<InputAction, bool>(static_cast<InputAction>(i), false));
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
	_actionPressed.clear();
	_keyboardMapping.clear();
	_gamepadMapping.clear();

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

bool InputHelper::ActionPressed(InputAction action)
{
	if (_mode == InputActionMode::PC)
	{
		if (_keyboard->KeyDown(_keyboardMapping[action]) && !_actionPressed[action])
		{
			_actionPressed[action] = true;
			return true;
		}
	}
	else
	{
		if (_gamepad->KeyDown(_gamepadMapping[action]) && !_actionPressed[action])
		{
			_actionPressed[action] = true;
			return true;
		}
	}

	return false;
}

bool InputHelper::ActionHold(InputAction action)
{
	if (_mode == InputActionMode::PC)
	{
		if (_keyboard->KeyDown(_keyboardMapping[action]))
		{
			return true;
		}
	}
	else
	{
		if (_gamepad->KeyDown(_gamepadMapping[action]))
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
		float x = ((float)_touchDevice->GetTouchX(touch) / 1920.0f) * RenderManager::Instance()->GetWidth();
		float y = ((float)_touchDevice->GetTouchY(touch) / 1088.0f) * RenderManager::Instance()->GetHeight();

		return glm::vec2(x, y);
	}
}

void InputHelper::UpdateAllInputs(bool state)
{
	_updateAll = true;
}

void InputHelper::Update()
{
	for (size_t i = 0; i < InputAction::Count; i++)
	{
		InputAction action = static_cast<InputAction>(i);
		if (_mode == InputActionMode::PC)
		{
			// check action
			if (_keyboard->KeyUp(_keyboardMapping[action]))
			{
				_actionPressed[action] = false;
			}else
			{
				_actionPressed[action] = true;
			}				
		}
		else
		{
			if (_gamepad->KeyUp(_gamepadMapping[action]))
			{
				_actionPressed[action] = false;
			}else
			{
				_actionPressed[action] = true;
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

bool InputHelper::ChangeInputForAction(InputAction action)
{
	if (_mode == InputActionMode::PC)
	{
		for (int key = 0; key < Key::Code::Count; key++)
		{
			Key::Code code = static_cast<Key::Code>(key);

			if (_keyboard->KeyDown(code) && !_keyPresseddMapping[code])
			{
				_keyboardMapping[action] = code;
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
				_gamepadMapping[action] = button;
				_updateAll = false;
				return true;
			}
		}		
	}

	return false;
}

Sprite* InputHelper::ActionSprite(InputAction action)
{
	if (_mode == InputActionMode::PC)
	{
		return _spriteKeyboardMapping[_keyboardMapping[action]];
	}
	else
	{
		return _spriteGamepadMapping[_gamepadMapping[action]];
	}

	return 0;
}

std::string InputHelper::InputName(InputAction action)
{
	if (_mode == InputActionMode::PC)
	{
		return _nameKeyboardMapping[_keyboardMapping[action]];
	}
	else
	{
		return _nameGamepadMapping[_gamepadMapping[action]];
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

	for (size_t i = 0; i < InputAction::Count; i++)
	{
		InputAction action = static_cast<InputAction>(i);
		if (_mode == InputActionMode::PC)
		{
			_actionKeyMapping.insert(std::pair<int, int>(i, _keyboardMapping[action]));
		}
		else
		{
			_actionKeyMapping.insert(std::pair<int, int>(i, _gamepadMapping[action]));
		}
	}

	return _actionKeyMapping;
}


void InputHelper::SetActionKeyMapping(std::map<int, int> mapping)
{
	if (_mode == InputActionMode::PC)
	{
		_keyboardMapping.clear();
	}
	else
	{
		_gamepadMapping.clear();
	}

	typedef std::map<int, int>::iterator it_type;
	for (it_type iterator = mapping.begin(); iterator != mapping.end(); iterator++)
	{
		InputAction action = static_cast<InputAction>(iterator->first);
		
		if (_mode == InputActionMode::PC)
		{
			Key::Code code = static_cast<Key::Code>(iterator->second);
			_keyboardMapping.insert(std::pair<InputAction, Key::Code>(action, code));
		}
		else
		{

			Gamepad::Button button = static_cast<Gamepad::Button>(iterator->second);
			_gamepadMapping.insert(std::pair<InputAction, Gamepad::Button>(action, button));
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

