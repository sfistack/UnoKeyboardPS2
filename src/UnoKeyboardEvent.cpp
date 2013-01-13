/*
 * UnoKeyboardEvent.cpp
 *
 *  Created on: 12-01-2013
 *      Author: sfistack
 */

#include "UnoKeyboardEvent.h"

/** UnoKeyboardEvent implementation **/

UnoKeyboardEvent::UnoKeyboardEvent() {
	eventBytesSize = 0;
	for (int i = 0; i < UNOKEYEVENT_MAX_BYTES_SIZE; i++) {
		bytes[i] = 0;
	}
}

bool UnoKeyboardEvent::isKeyPress() {
	switch (eventBytesSize) {
		case 1:
			return true;
		case 2:
			/** Key release byte in simple 1 byte codes **/
			return bytes[0] != UNOKEYTRANSFER_KEY_RELEASE;
		default:
			/** Key release byte in longer codes **/
			return bytes[1] != UNOKEYTRANSFER_KEY_RELEASE;
	}
}

bool UnoKeyboardEvent::isKeyRelease() {
	return !isKeyPress();
}

void UnoKeyboardEvent::addByte(byte inByte) {
	bytes[(int) eventBytesSize] = inByte;
	eventBytesSize++;
}

int UnoKeyboardEvent::getKey() {
	if (eventBytesSize > 1) {
		return bytes[1];
	}
	return bytes[0];
}
