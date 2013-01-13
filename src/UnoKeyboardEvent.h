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

#define UNOKEYTRANSFER_KEY_RELEASE 0xF0
#define UNOKEYTRANSFER_KEY_SPECIAL 0xE0

/**
 * Representation of event sent by keyboard.
 */
#define UNOKEYEVENT_MAX_BYTES_SIZE 6

class UnoKeyboardEvent {

private:
	char eventBytesSize;
	byte bytes[UNOKEYEVENT_MAX_BYTES_SIZE];

public:
	UnoKeyboardEvent();

	void addByte(byte inByte);

	bool isKeyPress();
	bool isKeyRelease();
	int getKey();
};

#endif /** UNOKEYBOARDEVENT_H **/
