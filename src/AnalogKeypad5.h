#pragma once

///////////////////////////////////////////////////////////////////////////////
// Arduino Class for 5-key Analog Keypad
// Copyright (C) https://muman.ch + https://github.com/mumanchu, 2026.10.28
// If you re-use this software please include the above copyright notice
//
// See https://github.com/mumanchu/AnalogKeypad5

class AnalogKeypad5
{
public:
	// repeat key timing in milliseconds
	uint repeatKeyDelayMs = 500;	// > repeatKeyRateMs
	uint repeatKeyRateMs = 100;		// >= pollRateMs

	// Bit flags are used, for repeat key handling
	typedef enum : uint
	{
		None = 0x00,
		Up = 0x01,
		Down = 0x02,
		Left = 0x04,
		Right = 0x08,
		Next = 0x10		// sometimes called Enter, Back, Select, etc.

	} Key;

	bool begin(uint kbPin, uint pollRateMs = 100);
	Key pollKey();
	void enableRepeatKey(Key key);
	void disableRepeatKey(Key key);
	bool isRepeatKey(Key key);

protected:
	// this can be overridden in a derived class,
	// for different boards, VCC voltages and ADC resolutions
	virtual Key readKey();

	uint pin;
	uint pollRate;
	Key lastKey;
	ulong lastPoll;
	uint repeatKeys;
	ulong repeatStart;
	uint repeatRate;
};


// Initialize
bool AnalogKeypad5::begin(uint kbPin, uint pollRateMs /*=100*/)
{
	if (!pinIsAnalogInput(kbPin) || pollRateMs < 1 || pollRateMs > 500)
		return false;
	pin = kbPin;
	pinMode(kbPin, INPUT_ANALOG);
	pollRate = pollRateMs;
	lastKey = Key::None;
	lastPoll = 0;
	repeatKeys = 0;
	return true;
}

// Get the key code according to the analog input voltage
// modify this according to your boards
// or it can be overridden in a derived class
AnalogKeypad5::Key AnalogKeypad5::readKey()
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

// Returns the key code if a new key has been pressed, or 0 if no key pressed
// it also handles the repeat keys, see enableRepeatKey()
// this is normally called from loop(), faster than pollRateMs
AnalogKeypad5::Key AnalogKeypad5::pollKey()
{
	// begin() not called
	if (pollRate == 0)
		return Key::None;

	// time to poll? this delay also does the debouncing
	ulong t = millis();
	if ((t - lastPoll) < pollRate)
		return Key::None;
	lastPoll = t;

	// get the key code according to the analog input voltage
	Key key = readKey();

	if (key == Key::None) {
		// no key pressed or key released
		lastKey = key;
		return key;
	}
	if (lastKey == Key::None && key != lastKey) {
		// new key pressed only if the last key was released,
		// because only one key can be depressed at a time
		lastKey = key;
		repeatStart = t;
		repeatRate = repeatKeyDelayMs;
		return key;
	}
	if (isRepeatKey(key) && key == lastKey) {
		// repeat key handling
		if ((t - repeatStart) >= repeatRate) {
			repeatStart = t;
			repeatRate = repeatKeyRateMs;
			return key;
		}
	}
	return Key::None;
}

// Configure a key to repeat or not repeat
// Sometimes a key needs to repeat when held down, depending on the 
// current action or menu position. This enables/disables the repaet 
// key feature for a specific key, or for ALL keys if key == 0. 
// Delay and repeat timing is the same for all keys, see repeatKeyDelayMs 
// and repeatKeyRateMs
void AnalogKeypad5::enableRepeatKey(Key key)
{
	// 0 = all keys
	if ((uint)key == 0)
		key = (Key)0x1f;
	repeatKeys |= (uint)key;
	//repeatStart = 0;
}
void AnalogKeypad5::disableRepeatKey(Key key)
{
	// 0 = all keys
	if ((uint)key == 0)
		key = (Key)0x1f;
	repeatKeys &= ~(uint)key;
}

bool AnalogKeypad5::isRepeatKey(Key key)
{
	return (repeatKeys & (uint)key) != 0;
}

