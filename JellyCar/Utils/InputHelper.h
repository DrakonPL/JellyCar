#ifndef InputHelper_H
#define InputHelper_H

#include <Andromeda/Input/InputManager.h>

using namespace Andromeda::Input;

#include <Andromeda/Graphics/Sprite.h>
using namespace Andromeda::Graphics;

#include <map>
#include <vector>
#include <string>

enum CarAction
{
	Left = 0,
	Right = 1,
	RotateLeft = 2,
	RotateRight = 3,
	Transform = 4,
	Ballon = 5,
	Tire = 6,
	Map = 7,
	Count = 8
};

enum MenuAction
{
    MenuUp,
	MenuDown,
	MenuLeft,
	MenuRight,
	MenuAccept,
	MenuBack,
	MenuPause,
	MenuExit,
	MenuCount
};

enum InputActionMode
{
	PC,
	PCGamepad,
	VITA,
	SWITCH
};

class InputHelper
{
private:

	static InputHelper* _inputHelper;

	//input
	InputManager* _inputManager;

	KeyboardDevice* _keyboard;
	GamepadDevice* _gamepad;

	MouseDevice* _mouseDevice;
	TouchDevice* _touchDevice;

	InputActionMode _mode;
	bool _updateAll;

	//key state
	std::map<Gamepad::Button, bool> _buttonPressed;
	std::map<Key::Code, bool> _keyPresseddMapping;

	//keys mapping
	std::map<CarAction, Key::Code> _carKeyboardMapping;
	std::map<CarAction, Gamepad::Button> _carGamepadMapping;

	std::map<MenuAction, Key::Code> _menuKeyboardMapping;
	std::map<MenuAction, Gamepad::Button> _menuGamepadMapping;

	//
	std::map<CarAction, bool> _carActionPressed;
	std::map<MenuAction, bool> _menuActionPressed;

	//menu mapping
	std::map<MenuAction, std::vector< Gamepad::Button>> _menuMapping;

	//sprite mapping
	Shader* _spriteShader;

	std::map<Gamepad::Button, Sprite*> _spriteGamepadMapping;
	std::map<Key::Code, Sprite*> _spriteKeyboardMapping;

	std::map<Gamepad::Button, std::string> _nameGamepadMapping;
	std::map<Key::Code, std::string> _nameKeyboardMapping;

	void PrepareKeyboardMapping();
	void PrepareGamepadMapping();

	void PrepareNameKeyboardMapping();
	void PrepareNameGamepadMapping();

	void PrepareSpriteGamepadMapping();
	void PrepareSpriteKeyboardMapping();

	InputHelper();

public:

	static InputHelper* Instance();
	~InputHelper();

	void Update();
	void UpdateAllInputs(bool state);

	bool ActionPressed(CarAction action);
	bool ActionHold(CarAction action);

	bool MenuActionPressed(MenuAction action);

	int GetTouchCount();
	glm::vec2 GetTouchPosition(int touch);

	glm::vec2 TouchToScreen(glm::vec4 screenBound,glm::vec2 touch);

	Sprite* CarActionSprite(CarAction action);
	Sprite* MenuActionSprite(MenuAction action);

	bool ChangeInputForAction(CarAction action);

	std::map<int, int> GetActionKeyMapping();
	void SetActionKeyMapping(std::map<int, int> mapping);

	std::string InputName(CarAction action);

	void LoadSettings();
	void SaveSettings();
};

#endif