#include <Arduino.h>
#include "../src/UnoKeyboardPS2.h"

UnoKeyboardPS2 *keyboard;

unsigned long lastEventTime;
unsigned long lastEventPtr;

void setup() {
	keyboard = UnoKeyboardPS2::getInstance();
	UnoPS2Connector connector;
	connector.setDataPin(8).setIrqPin(3).setIrqNum(1);
	keyboard->initialize(connector);
	lastEventTime = millis();
	Serial.begin(38400);
}

void loop() {
	int keyValue;
	int keyState;
	UnoKeyboardEvent* event = keyboard->getLastEvent();
	unsigned long now = millis();
	if(now - lastEventTime > 3000) {
		Serial.print("PING? ");
		Serial.println(now);
		lastEventTime = now;
	}
	if(NULL != event) {
		keyValue = event->getKey();
		keyState = event->isKeyRelease();
		delete event;
		Serial.print(keyState ? "R: " : "P: ");
		switch(keyValue) {
			case Keys::KEY_UP:
				Serial.println("UP!");
				break;
			case Keys::KEY_ESC:
				Serial.println("ESC!");
				break;
			case Keys::KEY_SPACE:
				Serial.println("SPACE!");
				break;
			default:
				Serial.print("V- 0x");
				Serial.println(keyValue, HEX);
				break;
		}
	}

}

int main(void) {

  init();
  setup();

  while(1) {
    loop();
  }
}
