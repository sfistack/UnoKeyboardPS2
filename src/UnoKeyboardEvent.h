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

/**
 * Representation of event sent by keyboard.
 */
#define UNOKEYEVENT_MAX_BYTES_SIZE 6
#define UNOKEYEVENT_IS_PRESSED 1
#define UNOKEYEVENT_IS_RELEASED 2

class UnoKeyboardEvent {

private:
	int state;
	char eventBytesSize;
	byte bytes[UNOKEYEVENT_MAX_BYTES_SIZE];

public:
	UnoKeyboardEvent();

	void addByte(byte inByte);
	void setState(int inState);

	void markAsKeyPress();
	void markAsKeyRelease();

	bool isKeyPress();
	bool isKeyRelease();
	int getKey();
	int getState();
};

#endif /** UNOKEYBOARDEVENT_H **/
