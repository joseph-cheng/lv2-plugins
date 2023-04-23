#include <lv2/core/lv2.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define AMP_URI "http://rhubarbdreams.dev/plugins/tone_gen"

typedef enum {
  TONE_GEN_FREQ = 0,
  TONE_GEN_LEVEL = 1,
  TONE_GEN_OUTPUT = 2,
} PortIndex_e;

typedef struct {
  const float *freq;
  const float *level;
  float *output;
  double position;
  double rate;
} ToneGen_t;

static LV2_Handle instantiate(const LV2_Descriptor *descriptor, double rate,
                              const char *bundle_path,
                              const LV2_Feature *const *features) {
  ToneGen_t *tone_gen = (ToneGen_t *)calloc(1, sizeof(ToneGen_t));
  tone_gen->rate = rate;
  return (LV2_Handle)tone_gen;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  ToneGen_t *tone_gen = (ToneGen_t *)instance;

  switch ((PortIndex_e)port) {
  case TONE_GEN_FREQ:
    tone_gen->freq = (const float *)data;
    break;
  case TONE_GEN_LEVEL:
    tone_gen->level = (const float *)data;
    break;
  case TONE_GEN_OUTPUT:
    tone_gen->output = (float *)data;
    break;
  }
}

static void activate(LV2_Handle instance) {
  ToneGen_t *tone_gen = (ToneGen_t *)instance;
  tone_gen->position = 0.0;
}

static void run(LV2_Handle instance, uint32_t num_samples) {
  ToneGen_t *tone_gen = (ToneGen_t *)instance;
  const float freq = *(tone_gen->freq);
  const float level = *(tone_gen->level);
  float *const output = tone_gen->output;

  for (uint32_t ii = 0; ii < num_samples; ii++) {
    output[ii] = sin(2.0 * M_PI * tone_gen->position) * level;
    tone_gen->position += freq / tone_gen->rate;
  }
}

static void cleanup(LV2_Handle instance) { free(instance); }

static void deactivate(LV2_Handle instance) {}

static const void *extension_data(const char *uri) { return NULL; }

static const LV2_Descriptor descriptor = {
    AMP_URI, instantiate, connect_port, activate,
    run,     deactivate,  cleanup,      extension_data,
};

LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index) {
  return index == 0 ? &descriptor : NULL;
}
