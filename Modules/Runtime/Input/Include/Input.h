#pragma once

#include "Core/Core.h"
#include "Data/Set.h"
#include "InputGen.h"

#define MAX_KEYS 256
#define MAX_BUTTONS 256

namespace Ry
{

	enum KeyAction
	{
		PRESS, RELEASE, REPEAT
	};

	struct GLFWwindow;

	/**
	 * A button listener that receives input for mouse buttons.
	 */
	class INPUT_MODULE ButtonListener
	{

	public:

		ButtonListener() {};
		virtual ~ButtonListener() {};

		virtual void onButtonPressed(int key) = 0;
		virtual void onButtonReleased(int key) = 0;

	};

	/**
	 * A listener for key events.
	 */
	class INPUT_MODULE KeyListener
	{

	public:

		KeyListener() {};
		virtual ~KeyListener() {};

		virtual void onKeyPressed(int key) = 0;
		virtual void onKeyReleased(int key) = 0;

	};

	/**
	 * A listener for character input events.
	 */
	class INPUT_MODULE CharListener
	{

	public:

		CharListener() {};
		virtual ~CharListener() {};

		virtual void onChar(unsigned int codepoint) = 0;
	};

	/**
	* A listener for scroll input events.
	*/
	class INPUT_MODULE ScrollListener
	{

	public:

		ScrollListener() {};
		virtual ~ScrollListener() {};

		virtual void onScroll(double amount) = 0;
	};

	/**
	 * Main class responsible for providing input information.
	 */
	class INPUT_MODULE InputHandler
	{

	public:

		InputHandler();
		~InputHandler();

		/**
		 * @param key The key code to query.
		 * @return bool Whether the specified key code is pressed.
		 */
		bool isKeyDown(int32 key);

		/**
		 * @param button The button code to query.
		 * @return bool Whether the specified button code is pressed.
		 */
		bool isButtonDown(int32 button);
		
		/**
		 * @return float The pixel x position of the cursor.
		 */
		float getX() const;
		
		/**
		 * @return float The pixel y position of the cursor.
		 */
		float getY() const;

		/**
		 * @return float The differential x coordinate of the cursor.
		 */
		float getDx() const;

		/**
		 * @return float The differential y coordinate of the cursor.
		 */
		float getDy() const;

		/**
		* @return float The scroll amount.
		*/
		float getScroll() const;

		/**
		 * Registers a new key listener to receive key input events.
		 * @param listener The key listener.
		 */
		void addKeyListener(KeyListener* listener);

		/**
		 * Removes an already registered key listener.
		 * @param listener The key listener to remove.
		 */
		void removeKeyListener(KeyListener* listener);

		/**
		 * Registers a new button listener to receive button input events.
		 * @param listener The button listener.
		 */
		void addButtonListener(ButtonListener* listener);
		
		/**
		 * Removes an already registered button listener.
		 * @param listener The button listener to remove.
		 */
		void removeButtonListener(ButtonListener* listener);

		/**
		 * Registers a new character listener to receive character input events.
		 * @param listener The character listener.
		 */
		void addCharListener(CharListener* listener);

		/**
		 * Removes an already registered character listener.
		 * @param listener The character listener to remove.
		 */
		void removeCharListener(CharListener* listener);

		/**
		 * Registers a new scroll listener to receive scroll input events.
		 * @param listener The scroll listener.
		 */
		void addScrollListener(ScrollListener* listener);

		/**
		 * Removes an already registered scroll listener.
		 * @param listener The scroll listener to remove.
		 */
		void removeScrollListener(ScrollListener* listener);

		// Functions not exposed in the API
		void setKey(int32 key, bool value);
		void setButton(int32 key, bool value);
		void setScroll(double scroll);
		void sendChar(unsigned int codepoint);
		void setMouse(float x, float y);

	private:

		Ry::ArrayList<KeyListener*> keyListeners;
		//std::set<ButtonListener*> buttonListeners;
		//std::set<CharListener*> charListeners;
		Ry::OASet<ScrollListener*> scrollListeners;

		bool keys_down[MAX_KEYS];
		bool buttons_down[MAX_BUTTONS];

		float mouse_x;
		float mouse_y;
		float last_mouse_x;
		float last_mouse_y;

		double scroll;
		double last_scroll;

	};

}