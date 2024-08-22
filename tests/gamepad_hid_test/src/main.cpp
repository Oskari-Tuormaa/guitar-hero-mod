#include <btstack.h>

#include <cstring>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>

#include <array>

#include <cinttypes>

#include "hid_stuff.h"

enum class AppInitState
{
    TRYING,
    DONE,
};

enum class DummyOutputState
{
    PRESS_START,
    PRESS_DOWN,
    PRESS_UP,
};

namespace config
{
    constexpr uint16_t class_of_device = 0x508;

    constexpr auto device_name = "Custom gamepad";

    constexpr uint16_t product_id = 1;
    constexpr uint16_t version    = 1;

    constexpr uint32_t dummy_report_timeout_ms = 1;

    constexpr uint32_t dummy_start_timeout_ms = 2'000;
    constexpr uint32_t dummy_stop_timeout_ms  = 20'000;

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

namespace globals
{
    static std::array<uint8_t, 300> hid_service_buffer;
    static std::array<uint8_t, 100> device_id_sdp_service_buffer;

    static bool connected     = false;
    static bool dummy_running = false;

    static btstack_packet_callback_registration_t packet_callbacks;

    static uint16_t hid_cid;

    static report_t report;

    static int dummy_output_state = 0;

    static AppInitState                app_init_state { AppInitState::TRYING };
    static btstack_link_key_iterator_t link_key_it;

    namespace timers
    {
        static btstack_timer_source_t dummy_report;
        static btstack_timer_source_t dummy_start_stop;
    } // namespace timers
} // namespace globals

void link_key_iterator_init()
{
    gap_link_key_iterator_init(&globals::link_key_it);
}

void link_key_iterator_done()
{
    gap_link_key_iterator_done(&globals::link_key_it);
}

void link_key_iterator_try_next()
{
    bd_addr_t       addr;
    link_key_t      link_key;
    link_key_type_t type;

    if (gap_link_key_iterator_get_next(&globals::link_key_it, addr, link_key,
                                       &type))
    {
        printf("Trying to connect to %s - type %u...\n", bd_addr_to_str(addr),
               (int)type);
        hid_device_connect(addr, &globals::hid_cid);
    }
    else
    {
        printf("Done trying...\n");
        link_key_iterator_done();
        globals::app_init_state = AppInitState::DONE;
    }
}

void send_report(report_t report)
{
    std::array<uint8_t, 2 + sizeof(report)> message {
        0xa1,
        config::report_id,
    };

    std::memcpy(&message[2], &report, sizeof(report));

    hid_device_send_interrupt_message(globals::hid_cid, message.begin(),
                                      message.size());
}

void dummy_report_handler(btstack_timer_source_t* ts)
{
    if (globals::connected)
    {
#if 1
        if (globals::dummy_running)
        {
            if (globals::dummy_output_state % 10 == 5)
            {
                globals::report.hat = HatSwitchDirections::HATSWITCH_DOWN;
            }
            else if ((globals::dummy_output_state % 10) == 6)
            {
                globals::report.hat = HatSwitchDirections::HATSWITCH_NONE;
            }

            switch ((globals::dummy_output_state / 10) % 5)
            {
            case 0:
                globals::report.b4 = false;
                globals::report.b0 = true;
                break;
            case 1:
                globals::report.b0 = false;
                globals::report.b1 = true;
                break;
            case 2:
                globals::report.b1 = false;
                globals::report.b2 = true;
                break;
            case 3:
                globals::report.b2 = false;
                globals::report.b3 = true;
                break;
            case 4:
                globals::report.b3 = false;
                globals::report.b4 = true;
                break;
            }

            globals::dummy_output_state++;
        }
        else
        {
            globals::report.hat = HatSwitchDirections::HATSWITCH_NONE;
            globals::report.b0  = false;
            globals::report.b1  = false;
            globals::report.b2  = false;
            globals::report.b3  = false;
            globals::report.b4  = false;
        }
#else
        globals::report.x  += 1;
        globals::report.y  += 2;
        globals::report.rx += 3;
        globals::report.ry += 4;
        globals::report.z  += 5;
        globals::report.rz += 6;

        uint8_t tmp         = globals::report.b18;
        globals::report.b18 = globals::report.b17;
        globals::report.b17 = globals::report.b16;
        globals::report.b16 = globals::report.b9;
        globals::report.b9  = globals::report.b8;
        globals::report.b8  = globals::report.b7;
        globals::report.b7  = globals::report.b6;
        globals::report.b6  = globals::report.b5;
        globals::report.b5  = globals::report.b4;
        globals::report.b4  = globals::report.b3;
        globals::report.b3  = globals::report.b2;
        globals::report.b2  = globals::report.b1;
        globals::report.b1  = globals::report.b0;
        globals::report.b0  = tmp;

        globals::report.hat += 1;
        if (globals::report.hat > 7)
            globals::report.hat = 0;
#endif

        hid_device_request_can_send_now_event(globals::hid_cid);
    }

    btstack_run_loop_set_timer_handler(ts, dummy_report_handler);
    btstack_run_loop_set_timer(ts, config::dummy_report_timeout_ms);
    btstack_run_loop_add_timer(ts);
}

void dummy_start_stop_handler(btstack_timer_source_t* ts)
{
    if (globals::dummy_running)
    {
        printf("Stopping dummy output\n");
        globals::dummy_running = false;
    }
    else
    {
        printf("Starting dummy output for %d ms\n",
               config::dummy_stop_timeout_ms);
        globals::dummy_running = true;

        btstack_run_loop_set_timer(ts, config::dummy_stop_timeout_ms);
        btstack_run_loop_add_timer(ts);
    }
}

void hid_event_handler(uint8_t* packet)
{
    uint8_t status;
    uint8_t subevent_type = hci_event_hid_meta_get_subevent_code(packet);
    switch (subevent_type)
    {
    case HID_SUBEVENT_CONNECTION_OPENED:
        status = hid_subevent_connection_opened_get_status(packet);
        if (status != ERROR_CODE_SUCCESS)
        {
            printf("Connection failed, status 0x%x\r\n", status);
            globals::hid_cid   = 0;
            globals::connected = false;

            if (globals::app_init_state == AppInitState::TRYING)
            {
                link_key_iterator_try_next();
            }

            return;
        }
        printf("Connection opened\r\n");
        globals::hid_cid   = hid_subevent_connection_opened_get_hid_cid(packet);
        globals::connected = true;
        globals::app_init_state = AppInitState::DONE;

        globals::dummy_output_state = 0;
        btstack_run_loop_set_timer_handler(&globals::timers::dummy_start_stop,
                                           dummy_start_stop_handler);
        btstack_run_loop_set_timer(&globals::timers::dummy_start_stop,
                                   config::dummy_start_timeout_ms);
        btstack_run_loop_add_timer(&globals::timers::dummy_start_stop);
        break;
    case HID_SUBEVENT_CONNECTION_CLOSED:
        printf("Connection closed\r\n");
        globals::hid_cid   = 0;
        globals::connected = false;
        break;
    case HID_SUBEVENT_CAN_SEND_NOW:
        send_report(globals::report);
        break;
    default:
        printf("Unknown HID subevent: 0x%02x\r\n", subevent_type);
        break;
    }
}

void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet,
                    uint16_t size)
{
    UNUSED(channel);
    UNUSED(size);

    uint8_t state;

    if (packet_type != HCI_EVENT_PACKET)
        return;

    uint8_t event_type = hci_event_packet_get_type(packet);
    switch (event_type)
    {
    case BTSTACK_EVENT_STATE:
        state = btstack_event_state_get_state(packet);
        printf("New BT state: 0x%02x\r\n", state);

        if (state == HCI_STATE_WORKING
            && globals::app_init_state == AppInitState::TRYING)
        {
            link_key_iterator_init();
            link_key_iterator_try_next();
        }

        break;

    case HCI_EVENT_USER_CONFIRMATION_REQUEST:
        printf("SSP User Confirmation Request with numeric values '%06" PRIu32
               "'\n",
               hci_event_user_confirmation_request_get_numeric_value(packet));
        printf("SSP User Confirmation Auto accept\n");
        break;

    case HCI_EVENT_HID_META:
        hid_event_handler(packet);
        break;
    }
}

void setup_bt()
{
    printf("Setting up BT...\r\n");
    gap_discoverable_control(1);
    gap_set_class_of_device(config::class_of_device);
    gap_set_local_name(config::device_name);
    gap_set_default_link_policy_settings(LM_LINK_POLICY_ENABLE_ROLE_SWITCH
                                         | LM_LINK_POLICY_ENABLE_SNIFF_MODE);
    gap_set_allow_role_switch(true);

    l2cap_init();

    sdp_init();
    printf("Done setting up BT.\r\n");
}

void setup_hid()
{
    printf("Setting up HID...\r\n");
    globals::hid_service_buffer.fill(0);

    hid_sdp_record_t hid_params {
        .hid_device_subclass      = config::hid::device_subclass,
        .hid_country_code         = config::hid::country_code,
        .hid_virtual_cable        = config::hid::virtual_cable,
        .hid_remote_wake          = config::hid::remote_wake,
        .hid_reconnect_initiate   = config::hid::reconnect_initiate,
        .hid_normally_connectable = config::hid::normally_connectable,
        .hid_boot_device          = config::hid::boot_device,
        .hid_ssr_host_max_latency = config::hid::host_max_latency,
        .hid_ssr_host_min_timeout = config::hid::host_min_timeout,
        .hid_supervision_timeout  = config::hid::supervision_timeout,
        .hid_descriptor           = hid_descriptor.begin(),
        .hid_descriptor_size      = hid_descriptor.size(),
        .device_name              = config::device_name,
    };

    hid_create_sdp_record(globals::hid_service_buffer.begin(),
                          sdp_create_service_record_handle(), &hid_params);
    btstack_assert(de_get_len(globals::hid_service_buffer.begin())
                   <= globals::hid_service_buffer.size());
    sdp_register_service(globals::hid_service_buffer.begin());

    device_id_create_sdp_record(globals::device_id_sdp_service_buffer.begin(),
                                sdp_create_service_record_handle(),
                                DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH,
                                BLUETOOTH_COMPANY_ID_LEGO_SYSTEM_A_S,
                                config::product_id, config::version);
    btstack_assert(de_get_len(globals::device_id_sdp_service_buffer.begin())
                   <= globals::device_id_sdp_service_buffer.size());
    sdp_register_service(globals::device_id_sdp_service_buffer.begin());

    hid_device_init(config::hid::boot_device, hid_descriptor.size(),
                    hid_descriptor.begin());

    globals::packet_callbacks.callback = &packet_handler;
    hci_add_event_handler(&globals::packet_callbacks);

    hid_device_register_packet_handler(&packet_handler);

    printf("Done setting up HID.\r\n");
}

int main()
{
    stdio_init_all();
    printf("Hello!\r\n");

    std::memset(&globals::report, 0, sizeof(globals::report));
    globals::report.b0 = false;

    cyw43_arch_init();

    setup_bt();

    setup_hid();

    hci_power_control(HCI_POWER_ON);

    btstack_run_loop_set_timer_handler(&globals::timers::dummy_report,
                                       dummy_report_handler);
    btstack_run_loop_set_timer(&globals::timers::dummy_report,
                               config::dummy_report_timeout_ms);
    btstack_run_loop_add_timer(&globals::timers::dummy_report);

    btstack_run_loop_execute();
}
