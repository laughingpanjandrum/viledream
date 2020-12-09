#pragma once

#include <string>
#include <memory>

#include "color_constants.h"
#include "libtcod.hpp"
#include "utility.h"
#include "keybindings.h"

using namespace std;



#define CHAR_SET_LOWERCASE "abcdefghijklmnopqrstuvwxyz"
#define CHAR_SET_UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"



enum buttonAction
{
	BTN_NONE,
};



struct button
{
	button() {}
	button(string name, colorType color, int x, int y, buttonAction action, int data) :
		_name(name), _color(color), _x(x), _y(y), _action(action), _data(data) {}

	string			_name;
	colorType		_color;
	int				_x, _y;
	buttonAction	_action;
	int				_data;
};
typedef shared_ptr<button> buttonPtr;



class inputHandler
{
public:

	inputHandler();
	~inputHandler() {}

	//					Check for new events
	void				checkForEvents() { TCODSystem::checkForEvent(TCOD_EVENT_ANY, &_key, &_mouse); }
	void				checkForKeypress() { TCODSystem::checkForEvent(TCOD_EVENT_KEY_RELEASE, &_key, &_mouse); }
	void				waitForKeypress() { TCODSystem::waitForEvent(TCOD_EVENT_KEY_RELEASE, &_key, &_mouse, true); }

	//					modifier keys
	bool				isCtrlPressed() const { return _key.lctrl || _key.rctrl; }
	bool				isShiftKeyPressed() const { return _key.shift; }

	//					Checking key pressed
	bool				isKeyPressed() const { return _key.pressed; }
	bool				isKeyPressed(TCOD_keycode_t kp) const { return _key.vk == kp; }
	bool				isKeyPressed(char c) const { return _key.c == c; }
	bool				isKeyPressed(int ka) const;

	//					Number keys
	bool				isNumberKey() const { return isNumberKey(_key.vk); }
	bool				isNumberKey(TCOD_keycode_t vk) const { return numberKeyToInt(vk) != -1; }
	int					numberKeyToInt(TCOD_keycode_t vk) const;
	int					numberKeyToInt() const { return numberKeyToInt(_key.vk); }

	//					Movement keys
	bool				isDirectionalKeyPressed() const;
	intpair				getVectorFromKeypress() const;

	//					Getting characters
	char				getKeypressCharacter() const { return _key.c; }
	bool				isKeypressACharacter() const;
	int					getIntFromKeypressChar() const;

	//					Mouse info
	intpair				getMouseCoord() const { return intpair(_mouse.cx, _mouse.cy); }

	//					Buttons
	void				clearButtons() { _buttons.clear(); }
	vector<buttonPtr>	getAllButtons() const { return _buttons; }
	buttonPtr			addButton(string name, colorType color, int x, int y, buttonAction action, int data = -1);

	//					Mouse/button interactions
	bool				isMouseOverButton(buttonPtr btn) const;
	buttonPtr			getButtonUnderMouse() const;

private:

	TCOD_key_t			_key;
	TCOD_mouse_t		_mouse;
	vector<buttonPtr>	_buttons;

	keybindings			_kb;

};
typedef shared_ptr<inputHandler> inputHandlerPtr;
