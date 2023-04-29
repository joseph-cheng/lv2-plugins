#ifndef __MIDI_H__
#define __MIDI_H__

#include <lv2/atom/atom.h>
#include <stdint.h>

typedef struct {
  uint8_t status;
  uint8_t pitch;
  uint8_t velocity;
} Midi_t;

typedef struct {
  LV2_Atom_Event event;
  Midi_t midi;
} MidiNoteEvent_t;

#endif // __MIDI_H__
