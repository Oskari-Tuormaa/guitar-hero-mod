#include "bt.hpp"

#include "hid.hpp"
#include "kitara.h"
#include "shutdown.hpp"

#include <array>
#include <ble/gatt-service/device_information_service_server.h>
#include <ble/gatt-service/hids_device.h>
#include <btstack.h>
#include <cstdio>

namespace bt
{

namespace
{

auto adv_data = std::to_array<uint8_t>({
    // Flags general discoverable, BR/EDR not supported
    0x02,
    BLUETOOTH_DATA_TYPE_FLAGS,
    0x06,
    // Name
    0x07,
    BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME,
    'K',
    'i',
    't',
    'a',
    'r',
    'a',
    // 16-bit Service UUIDs
    0x03,
    BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS,
    ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE & 0xff,
    ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE >> 8,
    // Appearance HID - Mouse (Category 15, Sub-Category 2)
    0x03,
    BLUETOOTH_DATA_TYPE_APPEARANCE,
    0xC4,
    0x03,
});

btstack_packet_callback_registration_t hci_event_callback_registration;
btstack_packet_callback_registration_t sm_event_callback_registration;

hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;

// NOLINTNEXTLINE [bugprone-easily-swappable-parameters]
void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size)
{
    UNUSED(channel);
    UNUSED(size);
    uint16_t conn_interval;

    if (packet_type != HCI_EVENT_PACKET)
        return;

    switch (hci_event_packet_get_type(packet))
    {
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            con_handle = HCI_CON_HANDLE_INVALID;
            printf("Disconnected\n");
            shutdown::restart();
            break;
        case SM_EVENT_JUST_WORKS_REQUEST:
            printf("Just Works requested\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            break;
        case HCI_EVENT_HIDS_META:
            switch (hci_event_hids_meta_get_subevent_code(packet))
            {
                case HIDS_SUBEVENT_INPUT_REPORT_ENABLE:
                    con_handle = hids_subevent_input_report_enable_get_con_handle(packet);
                    printf("Report Characteristic Subscribed %u\n",
                           hids_subevent_input_report_enable_get_enable(packet));
                    gap_request_connection_parameter_update(con_handle, 6, 6, 0, 100);
                    shutdown::restart();
                    break;
                case HIDS_SUBEVENT_CAN_SEND_NOW:
                    // Send report
                    {
                        auto report = hid::get_report_data();
                        hids_device_send_input_report(con_handle, report.data(), report.size());
                        break;
                    }
                default: break;
            }
            break;

        default: break;
    }
}

} // namespace

bool is_connected()
{
    return con_handle != HCI_CON_HANDLE_INVALID;
}

void send_hid_report()
{
    if (is_connected())
    {
        hids_device_request_can_send_now_event(con_handle);
    }
}

void init()
{
    // setup l2cap and
    l2cap_init();

    // setup SM: Display only
    sm_init();
    sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_BONDING);

    // setup ATT server
    att_server_init(profile_data, NULL, NULL);

    // setup device information service
    device_information_service_server_init();

    // setup HID Device service
    auto report_descriptor = hid::get_report_descriptor();
    hids_device_init(0, report_descriptor.data(), report_descriptor.size());

    // setup advertisements
    uint16_t  adv_int_min = 0x0030;
    uint16_t  adv_int_max = 0x0030;
    uint8_t   adv_type    = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data.size(), adv_data.data());
    gap_advertisements_enable(1);

    // register for events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    sm_event_callback_registration.callback = &packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    hids_device_register_packet_handler(packet_handler);

    hci_power_control(HCI_POWER_ON);
}

} // namespace bt
