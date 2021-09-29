#include "Input.h"
#include <iostream>

namespace Ry
{
	
	InputHandler::InputHandler()
	{
		// Init keys down to all false
		for(int32 Key = 0; Key < MAX_KEYS; Key++)
		{
			keys_down[Key] = false;
		}
		
		for (int32 Button = 0; Button < MAX_BUTTONS; Button++)
		{
			buttons_down[Button] = false;
		}
	}
	
	InputHandler::~InputHandler()
	{
		
	}
	
	bool InputHandler::isKeyDown(int32 key)
	{
		return keys_down[key];
	}
	
	bool InputHandler::isButtonDown(int32 button)
	{
		return buttons_down[button];
	}
	
	float InputHandler::getX() const
	{
		return mouse_x;
	}
	
	float InputHandler::getY() const
	{
		return mouse_y;
	}

	float InputHandler::getDx() const
	{
		return mouse_x - last_mouse_x;
	}

	float InputHandler::getDy() const
	{
		return mouse_y - last_mouse_y;
	}

	float InputHandler::getScroll() const
	{
		//std::cout << last_scroll << " " << scroll << std::endl;
		if (last_scroll == scroll)
			return 0;
		else
			return (float) scroll;
	}
	
	void InputHandler::setKey(int32 key, bool value)
	{
		if (keys_down[key] != value)
		{
			for (KeyListener* handler : keyListeners)
			{
				if (value)
					handler->onKeyPressed(key);
				else
					handler->onKeyReleased(key);
			}
		}

		keys_down[key] = value;
	}

	
	void InputHandler::setButton(int32 key, bool value)
	{

		//std::cout << "Button " << key << " set to " << value << std::endl;

		buttons_down[key] = value;
	
		/*for (ButtonListener* handler : buttonListeners)
		{
			if (value)
			{
				handler->onButtonPressed(key);
			}
			else
			{
				handler->onButtonReleased(key);
			}
		}*/
	}

	void InputHandler::setScroll(double scroll)
	{
		for (ScrollListener* handler : scrollListeners)
		{
			handler->onScroll(scroll);
		}
	}
	
	void InputHandler::sendChar(unsigned int codepoint)
	{
		/*for (CharListener* handler : charListeners)
		{
			handler->onChar(codepoint);
		}*/
	}
	
	void InputHandler::setMouse(float x, float y)
	{

		if (last_mouse_x == 0 && last_mouse_y == 0)
		{
			last_mouse_x = x;
			last_mouse_y = y;
		}
		else
		{
			last_mouse_x = mouse_x;
			last_mouse_y = mouse_y;
		}

		mouse_x = x;
		mouse_y = y;
	}
	
	void InputHandler::addKeyListener(KeyListener* listener)
	{
		keyListeners.insert(listener);
	}
	
	void InputHandler::removeKeyListener(KeyListener* listener)
	{
	//	keyListeners.erase(listener);
	}
	
	void InputHandler::addButtonListener(ButtonListener* listener)
	{
	//	buttonListeners.insert(listener);
	}
	
	void InputHandler::removeButtonListener(ButtonListener* listener)
	{
	//	buttonListeners.erase(listener);
	}
	
	void InputHandler::addCharListener(CharListener* listener)
	{
	//	charListeners.insert(listener);
	}
	
	void InputHandler::removeCharListener(CharListener* listener)
	{
	//	charListeners.erase(listener);
	}

	void InputHandler::addScrollListener(ScrollListener* listener)
	{
		scrollListeners.insert(listener);
	}

	void InputHandler::removeScrollListener(ScrollListener* listener)
	{
		scrollListeners.erase(listener);
	}
}