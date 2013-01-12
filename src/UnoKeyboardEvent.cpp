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
	for(int i = 0; i < UNOKEYEVENT_MAX_BYTES_SIZE; i++) {
		bytes[i] = 0;
	}
	state = 0;
}

void UnoKeyboardEvent::setState(int inState) {
	state = inState;
}

int UnoKeyboardEvent::getState() {
	return state;
}

bool UnoKeyboardEvent::isKeyPress() {
	return (state & UNOKEYEVENT_IS_PRESSED) == UNOKEYEVENT_IS_PRESSED;
}

bool UnoKeyboardEvent::isKeyRelease() {
	return (state & UNOKEYEVENT_IS_RELEASED) == UNOKEYEVENT_IS_RELEASED;
}

void UnoKeyboardEvent::markAsKeyPress() {
	state |= UNOKEYEVENT_IS_PRESSED;
}

void UnoKeyboardEvent::markAsKeyRelease() {
	state |= UNOKEYEVENT_IS_RELEASED;
}

void UnoKeyboardEvent::addByte(byte inByte) {
	bytes[(int) eventBytesSize] = inByte;
	eventBytesSize++;
}

int UnoKeyboardEvent::getKey() {
	if(eventBytesSize > 1) {
		return bytes[1];
	}
	return bytes[0];
}
