#pragma once

#include "hid_stuff.h"
#include "inplace_function.hpp"

namespace config
{
    constexpr uint16_t class_of_device = 0x508;

    constexpr auto device_name = "Custom gamepad";

    constexpr uint16_t product_id = 1;
    constexpr uint16_t version    = 1;

    namespace hid
    {
        constexpr uint16_t device_subclass = 0x2540;
        constexpr uint8_t  country_code    = 33;

        constexpr bool     virtual_cable        = false;
        constexpr bool     remote_wake          = true;
        constexpr bool     reconnect_initiate   = true;
        constexpr bool     normally_connectable = true;
        constexpr uint8_t  boot_device          = 0;
        constexpr uint16_t host_max_latency     = 1600;
        constexpr uint16_t host_min_timeout     = 3200;
        constexpr uint16_t supervision_timeout  = 3200;

    } // namespace hid
} // namespace config

using OnConnectedCallback = stdext::inplace_function<void()>;

void init_bt();
bool is_connected();
void set_on_connected_callback(OnConnectedCallback callback);
void send_report(const report_t& report);
