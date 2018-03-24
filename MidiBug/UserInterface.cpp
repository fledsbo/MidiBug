#include "UserInterface.h"
#include <LiquidCrystal.h>

UserInterface::UserInterface(MidiProcessor &_proc)
	: proc(_proc)
{
}
	


void UserInterface::SelectorUp() {
	mode = (Mode)(mode + 1);
	if (mode >= ModeMax) mode = (Mode)0;
	Serial.print("Mode: ");
	Serial.println((int)mode);

}

void UserInterface::SelectorDown() {
	mode = (Mode)(mode -1);
	if (mode > ModeMax) mode = (Mode)(ModeMax - 1);
	Serial.print("Mode: ");
	Serial.println((int)mode);
}

void UserInterface::SelectorPush()
{

}


void UserInterface::UpdateDisplay(LiquidCrystal &lcd) {
	lcd.clear();
	switch (mode)
	{
	case Scale:
		lcd.setCursor(0, 0);
		lcd.print("Scale");
		lcd.setCursor(0, 1);
		lcd.print(proc.ScaleStatus());
		break;
	}
}

void UserInterface::LeftUp() {
	switch (mode)
	{
	case Scale:
		proc.ScaleRootUp();
		break;
	}
}

void UserInterface::LeftDown() {
	switch (mode)
	{
	case Scale:
		proc.ScaleRootDown();
		break;
	}
}

void UserInterface::LeftPush() {
	switch (mode)
	{
	case Scale:
		proc.ScaleToggle();
		break;
	}
}

void UserInterface::RightUp() {
	switch (mode)
	{
	case Scale:
		proc.ScaleMapUp();
		break;
	}
}

void UserInterface::RightDown() {
	switch (mode)
	{
	case Scale:
		proc.ScaleMapDown();
		break;
	}
}

void UserInterface::RightPush() {

}

