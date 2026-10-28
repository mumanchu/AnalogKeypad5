///////////////////////////////////////////////////////////////////////////////
// Example sketch for AnalogKeypad5 library
// Copyright (C) https://muman.ch + https://github.com/mumanchu, 2026.10.28
// If you re-use this software please include the above copyright notice
//
// See https://github.com/mumanchu/AnalogKeypad5

#include "AnalogKeypad5.h"

// Derived class to adapt it for your MCU and keypad
class MyAnalogKeypad5 : public AnalogKeypad5
{
protected:
	MyAnalogKeypad5::Key readKey();
};

// Get the key code according to the analog input voltage
// modify this according to your boards
// or it can be overridden in a derived class
MyAnalogKeypad5::Key MyAnalogKeypad5::readKey()
{
	// calculate millivolts
	// this depends on your VCC and the ADC resolution
	int mv = (analogRead(pin) * 3300) / 1023;

	if (mv > 2845)
		return Key::None;
	if (mv > 2008)
		return Key::Next;
	if (mv > 1340)
		return Key::Right;
	if (mv > 760)
		return Key::Down;
	if (mv > 232)
		return Key::Up;
	return Key::Left;
}

MyAnalogKeypad5 keypad;


// Get ready
void setup()
{
	// I use different pins for Serial logging
	// this is only for the STM32 Nucleo-64 boards
	#ifdef ARDUINO_NUCLEO_64
	Serial.setTx(PC_10);
	Serial.setRx(PC_11);
	#endif

	Serial.begin(115200);
	delay(3000);

	// PuTTY clear screen and scrollback
	// comment this out if you are not using PuTTY
	//Serial.print("\033[2J\033[H\033[3J");

	Serial.println("\n\rAnalogKeypad5 Started\n\r");
	Serial.flush();

	// enable this to measure the key voltages in millivolts
	#if 0
	//pinMode(A0, INPUT_ANALOG);
	while (1) {
		// calculate millivolts
		// this depends on your VCC and the ADC resolution
		// 3300 = analog reference voltage, use 5000 for 5V
		// 1023 = max ADC count at reference voltage, 1023=10 bit, 4095=12 bit
		int mv = (analogRead(A0) * 3300) / 1023;
		Serial.println(mv);
		Serial.flush();
		delay(1000);
	}
	#endif

	// analog keypad is connected to analog input A0
	keypad.begin(A0);

	// up and down keys repeat
	keypad.enableRepeatKey((AnalogKeypad5::Key)(keypad.Down | keypad.Up));
	keypad.repeatKeyDelayMs = 750;		// increase delay until repeating starts
}

// Go
void loop()
{
	// has a key been pressed?
	AnalogKeypad5::Key key = keypad.pollKey();
	if (key) {
		char buf[16];
		sprintf(buf, "%02X", key);
		Serial.println(buf);
		Serial.flush();
	}

}

