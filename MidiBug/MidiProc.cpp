#include "MidiProc.h"

const int num_roots = 12;

char* root_names[] = {
	"C",
	"Db",
	"D",
	"Eb",
	"E",
	"F",
	"Gb",
	"G",
	"Ab",
	"A"
	"Bb",
	"B"
};

const int num_scales = 3;

char* scale_names[] = {
	"Chromatic",
	"Major",
	"Minor",
};

int scale_maps[][12] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
{ 0, 0, 2, 2, 4, 5, 5, 7, 7, 9, 9, 11 },
{ 0, 0, 2, 3, 3, 5, 5, 7, 8, 8, 10, 10 },
};



MidiProcessor::MidiProcessor()
{
	scale = NULL;
	arp = NULL;
	arp_value.interval = 6;
	arp_value.gate = 5;
	queueDepth = 0;
	numCurrentNotes = 0;
}

bool MidiProcessor::pushMessage(MidiMsg *msg) {
	if (msg->type == midi::NoteOn || msg->type == midi::NoteOff) {
		if (scale != NULL) {
			msg->byte1 = scaleRemap(msg->byte1);
		}
	}

	if (msg->type == midi::NoteOn) {
		currentNotes[numCurrentNotes++] = *msg;
	}

	if (msg->type == midi::NoteOff) {
		for (int i = 0; i < numCurrentNotes; i++) {
			if (currentNotes[i].byte1 == msg->byte1) {
				for (; i < numCurrentNotes - 1; i++) {
					currentNotes[i] = currentNotes[i + 1];
				}
				numCurrentNotes--;
				break;
			}
		}
	}
	if (arp != NULL) {
		if (numCurrentNotes == 0) {
			*msg = arpPlaying;
			msg->type = midi::NoteOff;
			return true;
		}
		return false;
	}
	return true;
}

bool MidiProcessor::popMessage(MidiMsg *msg) {
	if (queueDepth > 0) {
		*msg = queue[queueDepth - 1];
		queueDepth--;
		return true;
	}
	return false;
}

midi::DataByte MidiProcessor::scaleRemap(midi::DataByte in) {
	if (scale == NULL) {
		return in;
	}
	int oldPos = in % 12;
	int newPos = scale_maps[scale->scalemap][oldPos];
	in = in - oldPos + newPos;

	in += scale->root;
	return in;
}

void MidiProcessor::ScaleToggle()
{
	if (scale == NULL) {
		scale = &scale_value;
	}
	else {
		scale = NULL;
	}
}

void MidiProcessor::ScaleRootUp()
{
	scale_value.root = (scale_value.root + 1) % num_roots;
}

void MidiProcessor::ScaleRootDown()
{
	scale_value.root = (scale_value.root - 1) % num_roots;
	if (scale_value.root < 0) scale_value.root += num_roots;
}

void MidiProcessor::ScaleMapUp()
{
	scale_value.scalemap = (scale_value.scalemap + 1) % num_scales;
}

void MidiProcessor::ScaleMapDown()
{
	scale_value.scalemap = (scale_value.scalemap - 1) % num_scales;
	if (scale_value.scalemap < 0) scale_value.scalemap += num_scales;
}

char *MidiProcessor::ScaleStatus() {
	if (scale == NULL) {
		sprintf(statusBuffer, "Off");
	}
	else {
		sprintf(statusBuffer, "%s %s", root_names[scale->root % 12], scale_names[scale->scalemap]);
	}
	return statusBuffer;
}


void MidiProcessor::ArpToggle()
{
	if (arp == NULL) {
		arp = &arp_value;
		arpCurrent = 0;
	}
	else {
		arp = NULL;
	}
}

char *MidiProcessor::ArpStatus()
{
	return "";
}


void MidiProcessor::runClock()
{
	char buf[20];
	if (arp != NULL && numCurrentNotes > 0) {
		if (arpCurrent % arp->interval == 0) {
			int curNote = (arpCurrent / arp->interval) % numCurrentNotes;
			arpPlaying = currentNotes[curNote];
			arpPlaying.type = midi::NoteOn;
			queue[queueDepth] = arpPlaying;
			Serial.println(itoa(queue[queueDepth].byte1, buf, 10));
			queueDepth++;
		}
		else if (arpCurrent % arp->interval == arp->gate) {
			queue[queueDepth] = arpPlaying;
			queue[queueDepth].type = midi::NoteOff;
			Serial.println(itoa(queue[queueDepth].byte1, buf, 10));
			queueDepth++;
		}
		arpCurrent++;
	}
}