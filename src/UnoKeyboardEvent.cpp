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
	for (int i = 0; i < UnoKeyConfig::MAX_KEY_BYTES_SIZE; i++) {
		bytes[i] = 0;
	}
}

bool UnoKeyboardEvent::isKeyPress() {
	switch (eventBytesSize) {
		case 1:
			return true;
		case 2:
			/** Key release byte in simple 1 byte codes **/
			return bytes[0] != UnoKeyConfig::RELEASE_KEY_VALUE;
		default:
			/** Key release byte in longer codes **/
			return bytes[1] != UnoKeyConfig::RELEASE_KEY_VALUE;
	}
}

bool UnoKeyboardEvent::isKeyRelease() {
	return !isKeyPress();
}

void UnoKeyboardEvent::addByte(byte inByte) {
	bytes[(int) eventBytesSize] = inByte;
	eventBytesSize++;
}

/** Currently not handling PAUSE and PRINT_SCREEN **/
int UnoKeyboardEvent::getKey() {
	int keyValue = 0;

	if (eventBytesSize > 3) {
		return Keys::KEY_NOT_HANDLED;
	}
	/** Scan bytes for non-release values. Without PAUSE and PRINT_SCREEN
	 *  keys, there can be only one byte with key value + optional extended
	 *  flag.
	 */
	for(int i = 0; i < eventBytesSize; i++) {
		if(bytes[i] == UnoKeyConfig::RELEASE_KEY_VALUE) {
			continue;
		}
		if(bytes[i] == UnoKeyConfig::EXTENDED_KEY_VALUE) {
			keyValue += 0x0F00;
		}
		keyValue += bytes[i];
	}
	return keyValue;
}
