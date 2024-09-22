#include <array>
#include <cinttypes>
#include <cstring>
#include <utility>

#include <btstack.h>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>

#include "bt_init.hpp"
#include "topics.hpp"

namespace globals
{
    static std::array<uint8_t, 300> hid_service_buffer;
    static std::array<uint8_t, 100> device_id_sdp_service_buffer;

    static bool connected = false;

    static stdext::inplace_function<void()> on_connected_callback;

    static btstack_packet_callback_registration_t packet_callbacks;

    static uint16_t hid_cid;

    static report_t report;

} // namespace globals

void transmit_report(const report_t& report)
{
    std::array<uint8_t, 2 + sizeof(report)> message { 0xa1, config::report_id };
    packed_report_t                         packed_report = report.to_packed();
    std::memcpy(&message[2], &packed_report, sizeof(packed_report));
    hid_device_send_interrupt_message(globals::hid_cid, message.begin(),
                                      message.size());
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
        }
        else
        {
            printf("Connection opened\r\n");
            globals::on_connected_callback();
            globals::hid_cid
                = hid_subevent_connection_opened_get_hid_cid(packet);
            globals::connected = true;
        }
        Topic::BluetoothConnected::publish({ .connected = globals::connected });
        break;
    case HID_SUBEVENT_CONNECTION_CLOSED:
        printf("Connection closed\r\n");
        globals::hid_cid   = 0;
        globals::connected = false;
        Topic::BluetoothDisconnected::publish({});
        break;
    case HID_SUBEVENT_CAN_SEND_NOW:
        transmit_report(globals::report);
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
        switch (state)
        {
        case HCI_STATE_WORKING:
            printf("Bluetooth running!\n");
            Topic::BluetoothReady::publish({});
            break;
        default:
            printf("New BT state: 0x%02x\r\n", state);
            break;
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

void init_bt()
{
    std::memset(&globals::report, 0, sizeof(globals::report));

    printf("Setting up BT...\n");

    gap_discoverable_control(1);
    gap_set_class_of_device(config::class_of_device);
    gap_set_local_name(config::device_name);
    gap_set_default_link_policy_settings(LM_LINK_POLICY_ENABLE_ROLE_SWITCH
                                         | LM_LINK_POLICY_ENABLE_SNIFF_MODE);
    gap_set_allow_role_switch(true);

    l2cap_init();

    sdp_init();

    printf("Done setting up BT.\n");

    setup_hid();

    hci_power_control(HCI_POWER_ON);
}

bool is_connected()
{
    return globals::connected;
}

void send_report(const report_t& report)
{
    if (is_connected())
    {
        globals::report = report;
        hid_device_request_can_send_now_event(globals::hid_cid);
    }
}

uint16_t* get_hid_cid()
{
    return &globals::hid_cid;
}
