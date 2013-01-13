/**
 * Implementation
 */

#include <Arduino.h>
#include "UnoKeyboardPS2.h"

#define UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH 1
#define UNOKEYBOARD_CRUCIAL_DATA_BITS_WIDTH 8
#define UNOKEYBOARD_DATA_ENTITY_BITS_WIDTH 11
#define DATA_TRANSFER_FAILURE_MILLIS_TRESHOLD 25

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
	eventInProgress = NULL;
	lastCompletedEvent = NULL;
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
	static UnoKeyboardEvent *event = lastCompletedEvent;
	lastCompletedEvent = NULL;
	return event;
}

void UnoKeyboardPS2::onClockInterrupt() {
	static byte transmissionValue = 0;
	static byte processedBits = 0;
	static byte continueStream = 0;
	int lineState = 0;

#ifdef USE_TRANSMISSION_PARITY_CHECK
	static byte parityValue = 0;
#endif
#ifdef USE_TRANSMISSION_TIMEOUTS
	static int lastBitSentStamp = millis();
	static int now = 0;
#endif

#ifdef USE_TRANSMISSION_TIMEOUTS
	/** If multiple bytes transmission was detected, check for timeout **/
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

	if (processedBits == 0) {
		transmissionValue = 0;
		if (continueStream == 0) {
			/** Short ISR path, now its time to malloc **/
			eventInProgress = new UnoKeyboardEvent();
		}
	}

	if (processedBits < UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH) {
		processedBits++;
		return;
	}

	if (processedBits
			< (UNOKEYBOARD_CRUCIAL_DATA_BITS_WIDTH
					+ UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH)) {
		lineState = digitalRead(connector.DATA_PIN);
		transmissionValue |= (lineState
				<< (processedBits - UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH));
		processedBits++;
#ifdef USE_TRANSMISSION_PARITY_CHECK
		parityValue += lineState;
#endif
		return;
	}

#ifdef USE_TRANSMISSION_PARITY_CHECK
	/** This is a odd parity bit. Make check if parity number is odd **/
	if (processedBits
			== (UNOKEYBOARD_CRUCIAL_DATA_BITS_WIDTH
					+ UNOKEYBOARD_IGNORE_INITIAL_BITS_WIDTH)) {
		parityValue += digitalRead(connector.DATA_PIN);
	}

	/** Parity blocker, if no match for parity check, skip to end of byte **/
	if ((parityValue % 2) == 0) {
		if (processedBits == UNOKEYBOARD_DATA_ENTITY_BITS_WIDTH - 1) {
			parityValue = 0;
			continueStream = 0;
			transmissionValue = 0;
			processedBits = 0;
			return;
		}
		processedBits++;
		return;
	}
#endif
	/** Subtract 1 because we are counting up from 0 **/
	if (processedBits == UNOKEYBOARD_DATA_ENTITY_BITS_WIDTH - 1) {
		processedBits = 0;
		eventInProgress->addByte(transmissionValue);
		/** Check if its longer key code or its a key release event **/
		if (transmissionValue == UNOKEYTRANSFER_KEY_RELEASE
				|| transmissionValue == UNOKEYTRANSFER_KEY_SPECIAL) {
			continueStream = 1;
			return;
		}
		/** This is end of package **/
		continueStream = 0;
		/** Move completed event **/
		if (NULL != lastCompletedEvent) {
			delete lastCompletedEvent;
		}
		lastCompletedEvent = eventInProgress;
		eventInProgress = NULL;
		/** Leave regeneration of eventInProgress to next iteration **/
	} else {
		processedBits++;
	}
}

/** To get (void*) handle for attachInterrupt **/
void onUNOKeyboardPS2ClockInterrupt() {
	/** Speed up a little by static reference holder **/
	static UnoKeyboardPS2 *useInstance = UnoKeyboardPS2::getInstance();
	useInstance->onClockInterrupt();
}
