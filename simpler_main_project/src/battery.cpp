#include "battery.hpp"

#include "utils.hpp"

#include <ble/gatt-service/battery_service_server.h>
#include <btstack.h>
#include <cstdio>
#include <hardware/adc.h>
#include <hardware/gpio.h>

namespace battery
{

namespace
{

constexpr uint adc_gpio          = 27;
constexpr uint voltage_read_gpio = 26;

static btstack_timer_source_t battery_sense_start_timer;
#define BATTERY_SENSE_START_TIMEOUT_MS T_SEC_TO_MS(5)

static btstack_timer_source_t battery_sense_read_timer;
#define BATTERY_SENSE_READ_TIMEOUT_MS T_MS(10)

void battery_sense_read_timer_callback(btstack_timer_source_t* ts)
{
    UNUSED(ts);

    constexpr float conversion_factor = 3.3f / (1 << 12);
    constexpr float voltage_divider   = (330. + 100.) / 330.;
    constexpr float diode_drop        = 0.230;

    constexpr float min_voltage = 3.3f;
    constexpr float max_voltage = 4.2f;

    float batt_raw = 0;
    for (std::size_t i = 0; i < 50; i++)
    {
        batt_raw += static_cast<float>(adc_read());
    }
    batt_raw /= 50.f;
    gpio_put(26, false);

    float batt_voltage = batt_raw * conversion_factor * voltage_divider + diode_drop;
    float batt         = 100 * (batt_voltage - min_voltage) / (max_voltage - min_voltage);
    printf("Battery: %.0f   %.3f   %.3f\r\n", batt_raw, batt_voltage, batt);

    battery_service_server_set_battery_value(static_cast<uint8_t>(batt));

    btstack_run_loop_set_timer(&battery_sense_start_timer, BATTERY_SENSE_START_TIMEOUT_MS);
    btstack_run_loop_add_timer(&battery_sense_start_timer);
}

void battery_sense_start_timer_callback(btstack_timer_source_t* ts)
{
    UNUSED(ts);

    gpio_put(26, true);

    btstack_run_loop_set_timer(&battery_sense_read_timer, BATTERY_SENSE_READ_TIMEOUT_MS);
    btstack_run_loop_add_timer(&battery_sense_read_timer);
}

} // namespace

void init()
{
    adc_init();
    adc_gpio_init(adc_gpio);
    adc_select_input(1);

    gpio_init(voltage_read_gpio);
    gpio_set_dir(voltage_read_gpio, true);

    battery_service_server_init(100);

    btstack_run_loop_set_timer_handler(&battery_sense_start_timer,
                                       battery_sense_start_timer_callback);
    btstack_run_loop_set_timer_handler(&battery_sense_read_timer,
                                       battery_sense_read_timer_callback);

    btstack_run_loop_set_timer(&battery_sense_start_timer, BATTERY_SENSE_START_TIMEOUT_MS);
    btstack_run_loop_add_timer(&battery_sense_start_timer);
}

} // namespace battery
