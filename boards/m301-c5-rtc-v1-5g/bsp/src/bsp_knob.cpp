#include "bsp_knob.h"
#include "bsp_pins.h"
#include "esp_adc/adc_oneshot.h"
static adc_oneshot_unit_handle_t unit;
esp_err_t bsp_knob_init(void) {
    if (unit)
        return ESP_OK;
    adc_oneshot_unit_init_cfg_t cfg = {};
    cfg.unit_id = BSP_KNOB_ADC_UNIT;
    esp_err_t err = adc_oneshot_new_unit(&cfg, &unit);
    if (err != ESP_OK)
        return err;
    adc_oneshot_chan_cfg_t channel = {.atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_12};
    err = adc_oneshot_config_channel(unit, BSP_KNOB_ADC_CHANNEL, &channel);
    if (err != ESP_OK) {
        adc_oneshot_del_unit(unit);
        unit = nullptr;
    }
    return err;
}
esp_err_t bsp_knob_read_raw(int *raw) {
    if (!raw)
        return ESP_ERR_INVALID_ARG;
    if (!unit)
        return ESP_ERR_INVALID_STATE;
    return adc_oneshot_read(unit, BSP_KNOB_ADC_CHANNEL, raw);
}
