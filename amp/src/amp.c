#include <lv2/core/lv2.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define AMP_URI "http://lv2plug.in/plugins/eg-amp"

typedef enum {
  AMP_GAIN = 0,
  AMP_INPUT = 1,
  AMP_OUTPUT = 2,
} PortIndex_e;

typedef struct {
  const float *gain;
  const float *input;
  float *output;
} Amp_t;

static LV2_Handle instantiate(const LV2_Descriptor *descriptor, double rate,
                              const char *bundle_path,
                              const LV2_Feature *const *features) {
  Amp_t *amp = (Amp_t *)calloc(1, sizeof(Amp_t));
  return (LV2_Handle)amp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  Amp_t *amp = (Amp_t *)instance;

  switch ((PortIndex_e)port) {
  case AMP_GAIN:
    amp->gain = (const float *)data;
    break;
  case AMP_INPUT:
    amp->input = (const float *)data;
    break;
  case AMP_OUTPUT:
    amp->output = (float *)data;
    break;
  }
}

static void activate(LV2_Handle instance) {}

float db_to_coefficient(float db) {
  return db > -90.0f ? powf(10.0f, db * 0.05f) : 0.0f;
}

static void run(LV2_Handle instance, uint32_t num_samples) {
  const Amp_t *amp = (const Amp_t *)instance;
  const float gain = *(amp->gain);
  const float *const input = amp->input;
  float *const output = amp->output;

  const float coefficient = db_to_coefficient(gain);
  for (uint32_t ii = 0; ii < num_samples; ii++) {
    output[ii] = input[ii] * coefficient;
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
