#ifndef UNOKEYBOARDEVENT_H
#define UNOKEYBOARDEVENT_H

#ifdef DEBUG_UNOKEYBOARDEVENT
# define DEBUG_PRINT_UNOKEYBOARDEVENT(x) Serial.print(x);
# define DEBUG_PRINTLN_UNOKEYBOARDEVENT(x) Serial.println(x);
#else
# define DEBUG_PRINT_UNOKEYBOARDEVENT(x)
# define DEBUG_PRINTLN_UNOKEYBOARDEVENT(x)
#endif

#include <Arduino.h>

namespace UnoKeyConfig {
	const int RELEASE_KEY_VALUE = 0xF0;
	const int EXTENDED_KEY_VALUE = 0xE0;
	/**
	 * Representation of event sent by keyboard.
	 * Longest is pause - 8 bytes
	 */
	const int MAX_KEY_BYTES_SIZE = 8;
}

/** Keys codes. In namespace just to avoid boring static declaration **/
namespace Keys {
	const int KEY_NOT_HANDLED = 0;
	const int KEY_W = 29;
	const int KEY_S = 27;
	const int KEY_A = 28;
	const int KEY_D = 35;
	const int KEY_LCTRL = 20;
	const int KEY_LSHIFT = 18;
	const int KEY_TAB = 13;
	const int KEY_SPACE = 41;
	const int KEY_RETURN = 90;
	const int KEY_RETURN_NUM = 4154;
	const int KEY_LALT = 17;
	const int KEY_UP = 4181;
	const int KEY_DOWN = 4178;
	const int KEY_LEFT = 4171;
	const int KEY_RIGHT = 4180;
	const int KEY_ESC = 118;
}

class UnoKeyboardEvent {

private:
	char eventBytesSize;
	byte bytes[UnoKeyConfig::MAX_KEY_BYTES_SIZE];

public:
	UnoKeyboardEvent();

	void addByte(byte inByte);

	bool isKeyPress();
	bool isKeyRelease();
	int getKey();
};

#endif /** UNOKEYBOARDEVENT_H **/
