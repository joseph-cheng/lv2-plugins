#include "midi.h"
#include "uris.h"

#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/core/lv2.h>
#include <lv2/core/lv2_util.h>
#include <lv2/log/log.h>
#include <lv2/log/logger.h>
#include <lv2/midi/midi.h>
#include <lv2/urid/urid.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  FIFTHS_IN = 0,
  FIFTHS_OUT = 1,
} PortIndex_e;

typedef struct {
  LV2_URID_Map *map;
  LV2_Log_Logger logger;

  const LV2_Atom_Sequence *in_port;
  LV2_Atom_Sequence *out_port;

  FifthsURIs_t uris;
} Fifths_t;

void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  Fifths_t *fifths = (Fifths_t *)instance;
  switch (port) {
  case FIFTHS_IN:
    fifths->in_port = data;
    break;

  case FIFTHS_OUT:
    fifths->out_port = data;
    break;
  }
}

LV2_Handle instantiate(const LV2_Descriptor *descriptor, double sample_rate,
                       const char *path, const LV2_Feature *const *features) {
  Fifths_t *fifths = calloc(1, sizeof(Fifths_t));
  if (!fifths)
    return NULL;

  const char *missing_features =
      lv2_features_query(features, LV2_LOG__log, &fifths->logger.log, false,
                         LV2_URID__map, &fifths->map, true, NULL);

  lv2_log_logger_set_map(&fifths->logger, fifths->map);
  if (missing_features) {
    lv2_log_error(&fifths->logger, "Missing feature: <%s>\n", missing_features);
    free(fifths);
    return NULL;
  }

  map_fifths_uris(fifths->map, &fifths->uris);

  return fifths;
}

void run(LV2_Handle instance, uint32_t num_samples) {
  Fifths_t *fifths = instance;
  FifthsURIs_t *uris = &fifths->uris;

  // get capacity
  const uint32_t capacity = fifths->out_port->atom.size;

  // write empty sequence header to output
  lv2_atom_sequence_clear(fifths->out_port);
  fifths->out_port->atom.type = fifths->in_port->atom.type;

  LV2_ATOM_SEQUENCE_FOREACH(fifths->in_port, ev) {
    if (ev->body.type == uris->midi_Event) {
      const Midi_t *midi = (const Midi_t *)(ev + 1);
      switch (midi->status) {
      case LV2_MIDI_MSG_NOTE_ON:
      case LV2_MIDI_MSG_NOTE_OFF:
        lv2_atom_sequence_append_event(fifths->out_port, capacity, ev);
        if (midi->pitch <= 127 - 11) {
          MidiNoteEvent_t fifth_note;
          fifth_note.event.time.frames = ev->time.frames;
          fifth_note.event.body.type = ev->body.type;
          fifth_note.event.body.size = ev->body.size;

          fifth_note.midi.status = midi->status;
          fifth_note.midi.pitch = midi->pitch + 4;
          fifth_note.midi.velocity = midi->velocity;

          lv2_log_note(&fifths->logger, "time: %ld\n", fifth_note.event.time.frames);
          fifth_note.event.time.frames += 100000;

          lv2_atom_sequence_append_event(fifths->out_port, capacity,
                                         &fifth_note.event);
          fifth_note.event.time.frames += 100000;
          fifth_note.midi.pitch = midi->pitch + 7;
          lv2_atom_sequence_append_event(fifths->out_port, capacity,
                                         &fifth_note.event);
          fifth_note.event.time.frames += 100000;
          fifth_note.midi.pitch = midi->pitch + 11;
          lv2_atom_sequence_append_event(fifths->out_port, capacity,
                                         &fifth_note.event);
        }
        break;
      default:
        lv2_atom_sequence_append_event(fifths->out_port, capacity, ev);
        break;
      }
    }
  }
}

void cleanup(LV2_Handle instance) { free(instance); }

const void *extension_data(const char *uri) { return NULL; }

const LV2_Descriptor descriptor = {
    FIFTHS_URI, instantiate, connect_port, NULL,
    run,        NULL,        cleanup,      extension_data,
};

LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index) {
  return index == 0 ? &descriptor : NULL;
}
