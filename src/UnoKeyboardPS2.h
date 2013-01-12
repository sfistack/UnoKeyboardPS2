#ifndef UNOKEYBOARDPS2_H
#define UNOKEYBOARDPS2_H

#ifndef UnoPS2Connector
#define UnoPs2Connector

/** Switch debug on **/
#define DEBUG_UNOKEYBOARD
//#define USE_TRANSMISSION_TIMEOUTS

#ifdef DEBUG_UNOKEYBOARD
# define DEBUG_PRINT_UNOKEYBOARD(x) Serial.print(x);
# define DEBUG_PRINTLN_UNOKEYBOARD(x) Serial.println(x);
#else
# define DEBUG_PRINT_UNOKEYBOARD(x)
# define DEBUG_PRINTLN_UNOKEYBOARD(x)
#endif


#include "UnoKeyboardEvent.h"

/**
 * Connector for PS/2 Devices for UNO
 */
struct UnoPS2Connector {
	int IRQ_PIN;
	int DATA_PIN;
	/** This IRQ_NUM value should be got from final map **/
	int IRQ_NUM;

	UnoPS2Connector& setIrqNum(int num) {
		IRQ_NUM = num;
		return *this;
	}
	UnoPS2Connector& setIrqPin(int num) {
		IRQ_PIN = num;
		return *this;

	}
	UnoPS2Connector& setDataPin(int num) {
		DATA_PIN = num;
		return *this;
	}
};
#endif

class UnoKeyboardPS2 {

private:

	UnoPS2Connector connector;
	static UnoKeyboardPS2* instance;
	/** Initialized by getInstance **/
	UnoKeyboardPS2();
	/**
	 * This is not comfortable solution. Client of UnoKeyboardPS2 is
	 * responsible for whole memory state of the arduino device.
	 */
	UnoKeyboardEvent* eventInProgress;
	UnoKeyboardEvent* lastCompletedEvent;

public:
	static UnoKeyboardPS2* getInstance();

	/** Turn all devices on **/
	void initialize(UnoPS2Connector inConnector);

	/** Turning of all device features **/
	~UnoKeyboardPS2();

	/** Fetch last event caught by UnoKeyboardPS2. You are obligated to free it. **/
	UnoKeyboardEvent* getLastEvent();

	/** Translate keyboard event to integer key representation **/
	int getKey(UnoKeyboardEvent event);

	/** Has key been pressed **/
	bool isKeyPress(UnoKeyboardEvent event);

	/** Has key been released **/
	bool isKeyRelease(UnoKeyboardEvent event);

	/** Handle on interrupt **/
	void onClockInterrupt();
};

#endif /* UNOKEYBOARDPS2_H */
