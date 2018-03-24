/*
 Name:		MidiBug.ino
 Created:	3/24/2018 9:57:04 PM
 Author:	oysteinf
*/

#include <MIDI.h>
#include <Encoder.h>
#include <LiquidCrystal.h>

#include "MidiProc.h"
#include "UserInterface.h"

Encoder knob1(15, 14);
Encoder knob2(12, 13);
Encoder knob3(5, 6);

const int btn1 = 16;
const int btn2 = 11;
const int btn3 = 4;

LiquidCrystal lcd(17, 19, 20, 21, 22, 23);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi1);

MidiProcessor proc;
UserInterface ui(proc);


// the setup function runs once when you press reset or power the board
void setup() {
	lcd.begin(16, 2);
	// Print a message to the LCD.
	lcd.print("heisann!");

	midi1.begin(MIDI_CHANNEL_OMNI);
	midi1.turnThruOff();

	pinMode(btn1, INPUT);
	pinMode(btn2, INPUT);
	pinMode(btn3, INPUT);

	ui.UpdateDisplay(lcd);
}

int knob1Pos = 0;
int knob2Pos = 0;
int knob3Pos = 0;

int btn1Cur = 0;
int btn2Cur = 0;
int btn3Cur = 0;



bool internalClock = true;
int BPM = 105;
long tempo = 1000 / (BPM / 60);
long prevMillis = 0;
long interval = tempo / 24;

// the loop function runs over and over again until power down or reset
void loop() {
	char buf[10];
	int newKnob1Pos = knob1.read() >> 2;
	int newKnob2Pos = knob2.read() >> 2;
	int newKnob3Pos = knob3.read() >> 2;

	int btn1State = digitalRead(btn1);
	int btn2State = digitalRead(btn2);
	int btn3State = digitalRead(btn3);

	bool updateDisplay = false;

	while (newKnob1Pos < knob1Pos) {
		ui.SelectorDown();
		knob1Pos--;
		updateDisplay = true;
	}
	while (newKnob1Pos > knob1Pos) {
		ui.SelectorUp();
		knob1Pos++;
		updateDisplay = true;
	}

	while (newKnob2Pos < knob2Pos) {
		ui.LeftDown();
		knob2Pos--;
		updateDisplay = true;
	}
	while (newKnob2Pos > knob2Pos) {
		ui.LeftUp();
		knob2Pos++;
		updateDisplay = true;
	}

	while (newKnob3Pos < knob3Pos) {
		ui.RightDown();
		knob3Pos--;
		updateDisplay = true;
	}
	while (newKnob3Pos > knob3Pos) {
		ui.RightUp();
		knob3Pos++;
		updateDisplay = true;
	}

	if (btn1State != btn1Cur)
	{
		btn1Cur = btn1State;
		if (btn1State == HIGH) {
			ui.SelectorPush();
			updateDisplay = true;
		}
	}
	if (btn2State != btn2Cur)
	{
		btn2Cur = btn2State;
		if (btn2State == HIGH) {
			ui.LeftPush();
			updateDisplay = true;
		}
	}
	if (btn3State != btn3Cur)
	{
		btn3Cur = btn3State;
		if (btn3State == HIGH) {
			ui.RightPush();
			updateDisplay = true;
		}
	}
	

	if (updateDisplay) {
		ui.UpdateDisplay(lcd);
	}

	MidiMsg msg;
	if (midi1.read()) {
		msg.type = midi1.getType();
		msg.channel = midi1.getChannel();
		msg.byte1 = midi1.getData1();
		msg.byte2 = midi1.getData2();

		handleMidi(msg);
	}

	unsigned long currentMillis = millis();
	if (currentMillis - prevMillis > interval) {
		prevMillis = currentMillis;
		proc.runClock();
		while (proc.popMessage(&msg)) {
			Serial.print("Sending ");
			Serial.println(itoa(msg.byte1, buf, 10));
			midi1.send(msg.type, msg.byte1, msg.byte2, msg.channel);
		}
	}

}

void handleMidi(MidiMsg msg)
{
	char buf[10];
	if (proc.pushMessage(&msg)) {
		midi1.send(msg.type, msg.byte1, msg.byte2, msg.channel);
		Serial.println(itoa(msg.byte1, buf, 10));
	}
	while (proc.popMessage(&msg)) {
		midi1.send(msg.type, msg.byte1, msg.byte2, msg.channel);
		Serial.println(itoa(msg.byte1, buf, 10));
	}
}
