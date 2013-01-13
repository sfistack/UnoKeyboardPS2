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
	Serial.begin(38400);
}

void loop() {
	UnoKeyboardEvent* event = keyboard->getLastEvent();
	unsigned long now = millis();
	if(now - lastEventTime > 100) {
		Serial.print("PING ");
		Serial.println(now);
		lastEventTime = now;
	}
	if(NULL != event) {
		Serial.print(event->isKeyRelease() ? "R: " : "P: ");
		Serial.println(event->getKey());
		delete event;
	}

}

int main(void) {

  init();
  setup();

  while(1) {
    loop();
  }
}
