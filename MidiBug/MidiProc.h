#pragma once

#include <MIDI.h>

struct MidiMsg
{
	midi::MidiType type;
	midi::Channel channel;
	midi::DataByte byte1;
	midi::DataByte byte2;
};

class MidiProcessor
{
private:
	struct Scale
	{
		int root;
		int scalemap;
	};
	Scale scale_value;
	Scale *scale;

	struct Arp
	{
		int interval;
		int gate;
	};

	Arp arp_value;
	Arp *arp;
	int arpCurrent;
	MidiMsg arpPlaying;

	MidiMsg currentNotes[20];
	int numCurrentNotes;



	MidiMsg queue[10];
	int queueDepth;

	char statusBuffer[100];

	midi::DataByte scaleRemap(midi::DataByte in);
public:
	MidiProcessor();

	bool pushMessage(MidiMsg *msg);
	bool popMessage(MidiMsg *msg);
	void runClock();

	// Controls:

	void ScaleToggle();
	void ScaleRootUp();
	void ScaleRootDown();
	void ScaleMapUp();
	void ScaleMapDown();
	char *ScaleStatus();

	void ArpToggle();
	char *ArpStatus();
};