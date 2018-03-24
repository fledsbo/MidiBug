#pragma once
#include "MidiProc.h"
#include <LiquidCrystal.h>

class UserInterface {
public:
	UserInterface(MidiProcessor &proc);

	void SelectorUp();
	void SelectorDown();
	void SelectorPush();
	void LeftUp();
	void LeftDown();
	void LeftPush();
	void RightUp();
	void RightDown();
	void RightPush();

	void UpdateDisplay(LiquidCrystal &lcd);
private:
	enum Mode {
		Scale,
		ModeMax
	};

	Mode mode;
	MidiProcessor &proc;


};


