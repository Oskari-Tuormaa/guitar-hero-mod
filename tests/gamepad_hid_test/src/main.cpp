#include <btstack.h>

#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>

#include <array>

#include <cinttypes>

namespace config
{
    constexpr uint8_t report_id = 0x01;

    constexpr uint16_t class_of_device = 0x508;

    constexpr auto device_name = "Custom gamepad";

    constexpr uint16_t product_id = 1;
    constexpr uint16_t version    = 1;

    constexpr uint32_t dummy_report_timeout_ms = 50;

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

    static bool connected = false;

    static btstack_packet_callback_registration_t packet_callbacks;

    static uint16_t hid_cid;

    static uint16_t buttons = 1;
    static uint8_t  x       = 0;
    static uint8_t  y       = 0;
    static uint8_t  rx      = 0;
    static uint8_t  ry      = 0;

    namespace timers
    {
        static btstack_timer_source_t dummy_report;
    } // namespace timers
} // namespace globals

static auto hid_descriptor = std::to_array<uint8_t>({
    // clang-format off
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //     COLLECTION (Physical)

    // ReportID - 8 bits
    0x85, config::report_id,       //     REPORT_ID (1)

    // X & Y -> 2x8 = 16 bits
    0x05, 0x01,                  //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                  //     USAGE (X)
    0x09, 0x31,                  //     USAGE (Y)
    0x09, 0x33,                  //     USAGE (Rx)
    0x09, 0x34,                  //     USAGE (Ry)
    0x15, 0x81,                  //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                  //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                  //     REPORT_SIZE (8)
    0x95, 0x04,                  //     REPORT_COUNT (4)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Buttons - 8 bits
    0x05, 0x09,                  //     USAGE_PAGE (Button)
    0x19, 0x01,                  //     USAGE_MINIMUM (Button 1)
    0x29, 0x08,                  //     USAGE_MAXIMUM (Button 8)
    0x15, 0x00,                  //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                  //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                  //     REPORT_SIZE (1)
    0x95, 0x08,                  //     REPORT_COUNT (8)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Buttons - 8 bits
    0x05, 0x09,                  //     USAGE_PAGE (Button)
    0x19, 0x09,                  //     USAGE_MINIMUM (Button 9)
    0x29, 0x11,                  //     USAGE_MAXIMUM (Button 16)
    0x15, 0x00,                  //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                  //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                  //     REPORT_SIZE (1)
    0x95, 0x08,                  //     REPORT_COUNT (8)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    0xc0,                             //     END_COLLECTION
    0xc0,                             // END_COLLECTION
    // clang-format on
});

void send_report(uint8_t x, uint8_t y, uint8_t rx, uint8_t ry, uint16_t button)
{
    auto message = std::to_array<uint8_t>(
        { 0xa1, config::report_id, x, y, rx, ry,
          static_cast<uint8_t>(button >> 8), static_cast<uint8_t>(button) });

    hid_device_send_interrupt_message(globals::hid_cid, message.begin(),
                                      message.size());
}

void dummy_report_handler(btstack_timer_source_t* ts)
{
    if (globals::connected)
    {
        globals::x += 10;
        globals::y += 5;
        globals::rx += 20;
        globals::ry += 10;
        globals::buttons
            = globals::buttons == 0x8000 ? 1 : globals::buttons << 1;
        hid_device_request_can_send_now_event(globals::hid_cid);
    }

    btstack_run_loop_set_timer_handler(ts, dummy_report_handler);
    btstack_run_loop_set_timer(ts, config::dummy_report_timeout_ms);
    btstack_run_loop_add_timer(ts);
}

void hid_event_handler(uint8_t* packet)
{
    uint8_t status;
    uint8_t subevent_type = hci_event_hid_meta_get_subevent_code(packet);
    switch (subevent_type)
    {
    case HID_SUBEVENT_CONNECTION_OPENED:
        printf("Connection opened\r\n");
        status = hid_subevent_connection_opened_get_status(packet);
        if (status != ERROR_CODE_SUCCESS)
        {
            printf("Connection failed, status 0x%x\r\n", status);
            globals::hid_cid   = 0;
            globals::connected = false;
            return;
        }
        globals::hid_cid   = hid_subevent_connection_opened_get_hid_cid(packet);
        globals::connected = true;
        break;
    case HID_SUBEVENT_CONNECTION_CLOSED:
        printf("Connection closed\r\n");
        globals::hid_cid   = 0;
        globals::connected = false;
        break;
    case HID_SUBEVENT_CAN_SEND_NOW:
        send_report(globals::x, globals::y, globals::rx, globals::ry,
                    globals::buttons);
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
