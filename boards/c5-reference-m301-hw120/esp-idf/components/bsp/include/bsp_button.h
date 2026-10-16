#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BSP_BUTTON_WIFI,
    BSP_BUTTON_VOICE,
    BSP_BUTTON_COUNT,
} bsp_button_t;

typedef enum {
    BSP_BUTTON_PRESSED,
    BSP_BUTTON_RELEASED,
    BSP_BUTTON_CLICKED,
    BSP_BUTTON_LONG_PRESSED,
} bsp_button_event_t;

// Callback runs in the button component task; it must not block.
typedef void (*bsp_button_callback_t)(bsp_button_t button, bsp_button_event_t event,
                                      void *user_data);

esp_err_t bsp_button_init(bsp_button_callback_t callback, void *user_data);
bool bsp_button_is_pressed(bsp_button_t button);

#ifdef __cplusplus
}
#endif
