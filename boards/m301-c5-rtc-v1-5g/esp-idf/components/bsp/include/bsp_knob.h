#pragma once
#include "esp_err.h"
#ifdef __cplusplus
extern "C" {
#endif
// One application owner; raw ADC, no application volume mapping or persistence.
esp_err_t bsp_knob_init(void);
esp_err_t bsp_knob_read_raw(int *raw);
#ifdef __cplusplus
}
#endif
