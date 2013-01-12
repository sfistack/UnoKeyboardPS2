#include <Arduino.h>
#include "../src/UnoKeyboardPS2.h"

UnoKeyboardPS2 *keyboard;

unsigned long lastEventTime;

void setup() {
	keyboard = UnoKeyboardPS2::getInstance();
	UnoPS2Connector connector;
	connector.setDataPin(8).setIrqPin(3).setIrqNum(1);
	keyboard->initialize(connector);
	lastEventTime = millis();
	Serial.begin(9600);
}

void loop() {
	UnoKeyboardEvent* event = keyboard->getLastEvent();
	if(NULL != event) {
		unsigned long now = millis();
		int dump = (event->isKeyRelease() ? 10000 : 1000) + event->getKey();
		delete event;
		Serial.println(dump);
		lastEventTime = now;
	}
}

int main(void) {

  init();
  setup();

  while(1) {
    loop();
  }
}
