/**
 * Implementation
 */

#include <Arduino.h>
#include "UnoKeyboardPS2.h"

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
	/** We must quiet interrupts for assignment time! A panda dies w/o interrupts mute **/
	noInterrupts();
	UnoKeyboardEvent* event = lastCompletedEvent;
	lastCompletedEvent = NULL;
	interrupts();
	return event;
}

void UnoKeyboardPS2::onClockInterrupt() {
	static byte transmissionValue = 0;
	static byte processedBits = 0;
	static byte continueStream = 0;
	static int lineState = 0;

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
		if (now - lastBitSentStamp > UnoKeyboardConfig::TIMEOUT_MILLIS_DIFF_TRESHOLD) {
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
			/** Short ISR path, now its time to malloc. Let`s hope mem space is there. **/
			eventInProgress = new UnoKeyboardEvent();
		}
	}

	if (processedBits < UnoKeyboardConfig::IGNORE_INITIAL_BITS_COUNT) {
		processedBits++;
		return;
	}

	if (processedBits
			< (UnoKeyboardConfig::DATA_BITS_COUNT
					+ UnoKeyboardConfig::IGNORE_INITIAL_BITS_COUNT)) {
		lineState = digitalRead(connector.DATA_PIN);
		transmissionValue |= (lineState
				<< (processedBits - UnoKeyboardConfig::IGNORE_INITIAL_BITS_COUNT));
		processedBits++;
#ifdef USE_TRANSMISSION_PARITY_CHECK
		parityValue += lineState;
#endif
		return;
	}

#ifdef USE_TRANSMISSION_PARITY_CHECK
	/** This is a odd parity bit. Make check if parity number is odd **/
	if (processedBits
			== (UnoKeyboardConfig::DATA_BITS_COUNT
					+ UnoKeyboardConfig::IGNORE_INITIAL_BITS_COUNT)) {
		parityValue += digitalRead(connector.DATA_PIN);
	}

	/** Parity blocker, if no match for parity check, skip to end of byte **/
	if ((parityValue % 2) == 0) {
		if (processedBits == UnoKeyboardConfig::DATA_PACKAGE_BITS_SIZE - 1) {
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
	if (processedBits == UnoKeyboardConfig::DATA_PACKAGE_BITS_SIZE - 1) {
		processedBits = 0;
		if(eventInProgress != NULL) {
			/** To be honest there`s a problem in case of event object absence **/
			eventInProgress->addByte(transmissionValue);
		}
		/** Check if its longer key code or its a key release event **/
		if (transmissionValue == UnoKeyConfig::RELEASE_KEY_VALUE
				|| transmissionValue == UnoKeyConfig::EXTENDED_KEY_VALUE) {
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
