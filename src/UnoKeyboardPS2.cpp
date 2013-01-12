/**
 * Implementation
 */

#include <Arduino.h>
#include "UnoKeyboardPS2.h"

#define UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH 1
#define UNOKEYBOARD_CRUCIAL_DATA_BITS_WIDTH 8
#define UNOKEYBOARD_DATA_ENTITY_BITS_WIDTH 11
#define DATA_TRANSFER_FAILURE_MILLIS_TRESHOLD 25

#define UNOKEYTRANSFER_KEY_RELEASE 0xF0
#define UNOKEYTRANSFER_KEY_SPECIAL 0xE0

/** UnoKeyboardPS2 implementation **/

UnoKeyboardPS2* UnoKeyboardPS2::instance = NULL;
void onUNOKeyboardPS2ClockInterrupt();

UnoKeyboardPS2* UnoKeyboardPS2::getInstance() {
	if (UnoKeyboardPS2::instance == NULL) {
		UnoKeyboardPS2::instance = new UnoKeyboardPS2();
	}
	return UnoKeyboardPS2::instance;
}

UnoKeyboardPS2::UnoKeyboardPS2() {
}

void UnoKeyboardPS2::initialize(UnoPS2Connector inConnector) {
	connector = inConnector;
	pinMode(inConnector.IRQ_PIN, INPUT_PULLUP);
	pinMode(inConnector.DATA_PIN, INPUT_PULLUP);
	digitalWrite(inConnector.IRQ_PIN, HIGH);
	digitalWrite(inConnector.DATA_PIN, HIGH);
	attachInterrupt(inConnector.IRQ_NUM, onUNOKeyboardPS2ClockInterrupt,
			FALLING);
}

UnoKeyboardEvent* UnoKeyboardPS2::getLastEvent() {
	if (lastCompletedEvent == NULL) {
		return NULL;
	}
	UnoKeyboardEvent *event = lastCompletedEvent;
	lastCompletedEvent = NULL;
	return event;
}

void UnoKeyboardPS2::onClockInterrupt() {
	static byte transmissionValue = 0;
	static byte processedBits = 0;
	static byte continueStream = 0;
	static int lastBitSentStamp = millis();
	static int now = 0;
	static int lineState = 0;

	lineState = digitalRead(connector.DATA_PIN);
#ifdef USE_TRANSMISSION_TIMEOUTS
	/** If multibyte transmission was detected, check for timeout **/
	if(continueStream) {
		now = millis();
		/** Check for data transfer failure. If it happens, abort current construction **/
		if (now - lastBitSentStamp > DATA_TRANSFER_FAILURE_MILLIS_TRESHOLD) {
			lastBitSentStamp = now;
			transmissionValue = 0;
			processedBits = 0;
			continueStream = 0;
			if(NULL != eventInProgress) {
				delete eventInProgress;
				eventInProgress = new UnoKeyboardEvent();
			}
		}
	}
#endif
	//DEBUG_PRINTLN_UNOKEYBOARD(processedBits);
	/** New bit transmission, get time stamp **/
	lastBitSentStamp = millis();

	if(processedBits < UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH) {
		processedBits++;
		return;
	}

	if (processedBits < (UNOKEYBOARD_CRUCIAL_DATA_BITS_WIDTH + UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH)) {
		transmissionValue |= (lineState << (processedBits - UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH));
		DEBUG_PRINT_UNOKEYBOARD(lineState);
		processedBits++;
		return;
	}

	/** Substract 1 because we are counting up from 0 **/
	if (processedBits == UNOKEYBOARD_DATA_ENTITY_BITS_WIDTH - 1) {
		processedBits = 0;
		DEBUG_PRINTLN_UNOKEYBOARD(" EOB");
		eventInProgress->addByte(transmissionValue);
		/** check if its longer key code or its a key release event **/
		if (transmissionValue == UNOKEYTRANSFER_KEY_RELEASE) {
			continueStream = 1;
			eventInProgress->markAsKeyRelease();
		} else if (transmissionValue == UNOKEYTRANSFER_KEY_SPECIAL) {
			continueStream = 1;
		} else {
			if (!eventInProgress->isKeyRelease()) {
				eventInProgress->markAsKeyPress();
			}
			/** move completed event **/
			if (NULL != lastCompletedEvent) {
				delete lastCompletedEvent;
			}
			lastCompletedEvent = eventInProgress;
			eventInProgress = new UnoKeyboardEvent();
			continueStream = 0;
		}
		transmissionValue = 0;
	} else {
		processedBits++;
	}
}

/** To get (void*) handle for attachInterrupt **/
void onUNOKeyboardPS2ClockInterrupt() {
	UnoKeyboardPS2::getInstance()->onClockInterrupt();
}
