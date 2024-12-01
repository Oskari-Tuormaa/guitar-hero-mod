/*
 * Copyright (C) 2017 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at
 * contact@bluekitchen-gmbh.com
 *
 */

#include <pico/time.h>
#define BTSTACK_FILE__ "hog_mouse_demo.c"

// *****************************************************************************
/* EXAMPLE_START(hog_mouse_demo): HID Mouse LE
 */
// *****************************************************************************

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hog_mouse_demo.h"

#include "btstack.h"

#include "ble/gatt-service/battery_service_server.h"
#include "ble/gatt-service/device_information_service_server.h"
#include "ble/gatt-service/hids_device.h"

#include <hardware/gpio.h>

#define BIT(n)  (1 << n)
#define IBIT(n) (~(1 << n))

struct __attribute__((packed)) GamepadReport
{
    uint16_t buttons : 9; // 9 buttons (1 bit each)
    uint8_t  pad1 : 7; // 7 bit padding for alignment
    uint8_t  hat : 4; // Hat switch (4 bits)
    uint8_t  pad2 : 4; // 4 bits padding for alignment
};

enum HatSwitchDirections
{
    HATSWITCH_UP        = 0x00,
    HATSWITCH_UPRIGHT   = 0x01,
    HATSWITCH_RIGHT     = 0x02,
    HATSWITCH_DOWNRIGHT = 0x03,
    HATSWITCH_DOWN      = 0x04,
    HATSWITCH_DOWNLEFT  = 0x05,
    HATSWITCH_LEFT      = 0x06,
    HATSWITCH_UPLEFT    = 0x07,
    HATSWITCH_NONE      = 0x0F,
};

// from USB HID Specification 1.1, Appendix B.2
static const uint8_t gamepad_hid_report_descriptor[] = {
    // clang-format off
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)

    0x85, 0x01,        // Report Id (1)
    
    // Buttons (9 buttons)
    0x05, 0x09,        //   Usage Page (Button)
    /*HID_BUTTON_USAGES,*/
    0x19, 0x01,        //   Usage Minimum (Button 1)
    0x29, 0x09,        //   Usage Maximum (Button 9)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x09,        //   Report Count (9)
    0x81, 0x02,        //   Input (Data, Var, Abs)
    
    // Padding for alignment (7 bit)
    0x75, 0x07,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x03,        //   Input (Cnst, Var, Abs)
    
    // Hat switch
    0x05, 0x01,        //   Usage Page (Generic Desktop)
    0x09, 0x39,        //   Usage (Hat switch)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x07,        //   Logical Maximum (7)
    0x35, 0x00,        //   Physical Minimum (0)
    0x46, 0x3B, 0x01,  //   Physical Maximum (315)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Var, Abs)
    
    // Padding for alignment (4 bits)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x03,        //   Input (Cnst, Var, Abs)
    
    0xC0               // End Collection
    // clang-format on
};

static btstack_packet_callback_registration_t hci_event_callback_registration;
static btstack_packet_callback_registration_t l2cap_event_callback_registration;
static btstack_packet_callback_registration_t sm_event_callback_registration;
static uint8_t                                battery = 100;
static hci_con_handle_t con_handle                    = HCI_CON_HANDLE_INVALID;
static uint8_t          protocol_mode                 = 1;

static void packet_handler(uint8_t packet_type, uint16_t channel,
                           uint8_t* packet, uint16_t size);

const uint8_t adv_data[] = {
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
};
const uint8_t adv_data_len = sizeof(adv_data);

static void hog_mouse_setup(void)
{

    // setup l2cap and
    l2cap_init();

    // setup SM: Display only
    sm_init();
    sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION
                                       | SM_AUTHREQ_BONDING);

    // setup ATT server
    att_server_init(profile_data, NULL, NULL);

    // setup battery service
    battery_service_server_init(battery);

    // setup device information service
    device_information_service_server_init();

    // setup HID Device service
    hids_device_init(0, gamepad_hid_report_descriptor,
                     sizeof(gamepad_hid_report_descriptor));

    // setup advertisements
    uint16_t  adv_int_min = 0x0030;
    uint16_t  adv_int_max = 0x0030;
    uint8_t   adv_type    = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0,
                                  null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t*)adv_data);
    gap_advertisements_enable(1);

    // register for events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // register for connection parameter updates
    l2cap_event_callback_registration.callback = &packet_handler;
    l2cap_add_event_handler(&l2cap_event_callback_registration);

    sm_event_callback_registration.callback = &packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    hids_device_register_packet_handler(packet_handler);
}

// HID Report sending
static void send_report(uint16_t buttons, uint8_t hat)
{
    struct GamepadReport report = {
        .buttons = buttons,
        .hat     = hat,
    };
    switch (protocol_mode)
    {
    case 0:
        hids_device_send_boot_mouse_input_report(con_handle, (uint8_t*)&report,
                                                 sizeof(report));
        break;
    case 1:
        hids_device_send_input_report_for_id(con_handle, 1, (uint8_t*)&report,
                                             sizeof(report));
        break;
    default:
        break;
    }
    printf("buttons: 0x%04x   hat: 0x%02x\n", report.buttons, report.hat);
}

static uint16_t buttons = 0;
static uint8_t  hat = HATSWITCH_NONE;

static void mousing_can_send_now(void)
{
    send_report(buttons, hat);
}

// Demo Application

// On systems with STDIN, we can directly type on the console

static void packet_handler(uint8_t packet_type, uint16_t channel,
                           uint8_t* packet, uint16_t size)
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
        break;
    case SM_EVENT_JUST_WORKS_REQUEST:
        printf("Just Works requested\n");
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
        printf("Confirming numeric comparison: %" PRIu32 "\n",
               sm_event_numeric_comparison_request_get_passkey(packet));
        sm_numeric_comparison_confirm(
            sm_event_passkey_display_number_get_handle(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
        printf("Display Passkey: %" PRIu32 "\n",
               sm_event_passkey_display_number_get_passkey(packet));
        break;
    case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE:
        printf("L2CAP Connection Parameter Update Complete, response: %x\n",
               l2cap_event_connection_parameter_update_response_get_result(
                   packet));
        break;
    case HCI_EVENT_META_GAP:
        switch (hci_event_gap_meta_get_subevent_code(packet))
        {
        case GAP_SUBEVENT_LE_CONNECTION_COMPLETE:
            // print connection parameters (without using float operations)
            conn_interval
                = gap_subevent_le_connection_complete_get_conn_interval(packet);
            printf("LE Connection Complete:\n");
            printf("- Connection Interval: %u.%02u ms\n",
                   conn_interval * 125 / 100, 25 * (conn_interval & 3));
            printf(
                "- Connection Latency: %u\n",
                gap_subevent_le_connection_complete_get_conn_latency(packet));
            break;
        default:
            break;
        }
        break;
    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
            // print connection parameters (without using float operations)
            conn_interval
                = hci_subevent_le_connection_update_complete_get_conn_interval(
                    packet);
            printf("LE Connection Update:\n");
            printf("- Connection Interval: %u.%02u ms\n",
                   conn_interval * 125 / 100, 25 * (conn_interval & 3));
            printf("- Connection Latency: %u\n",
                   hci_subevent_le_connection_update_complete_get_conn_latency(
                       packet));
            break;
        default:
            break;
        }
        break;
    case HCI_EVENT_HIDS_META:
        switch (hci_event_hids_meta_get_subevent_code(packet))
        {
        case HIDS_SUBEVENT_INPUT_REPORT_ENABLE:
            con_handle
                = hids_subevent_input_report_enable_get_con_handle(packet);
            printf("Report Characteristic Subscribed %u\n",
                   hids_subevent_input_report_enable_get_enable(packet));
            // request connection param update via L2CAP following Apple
            // Bluetooth Design Guidelines
            // gap_request_connection_parameter_update(con_handle, 12, 12, 4,
            // 100);    // 15 ms, 4, 1s

            // directly update connection params via HCI following Apple
            // Bluetooth Design Guidelines
            // gap_update_connection_parameters(con_handle, 12, 12, 4, 100); //
            // 60-75 ms, 4, 1s

            break;
        case HIDS_SUBEVENT_BOOT_KEYBOARD_INPUT_REPORT_ENABLE:
            con_handle
                = hids_subevent_boot_keyboard_input_report_enable_get_con_handle(
                    packet);
            printf("Boot Keyboard Characteristic Subscribed %u\n",
                   hids_subevent_boot_keyboard_input_report_enable_get_enable(
                       packet));
            break;
        case HIDS_SUBEVENT_BOOT_MOUSE_INPUT_REPORT_ENABLE:
            con_handle
                = hids_subevent_boot_mouse_input_report_enable_get_con_handle(
                    packet);
            printf("Boot Mouse Characteristic Subscribed %u\n",
                   hids_subevent_boot_mouse_input_report_enable_get_enable(
                       packet));
            break;
        case HIDS_SUBEVENT_PROTOCOL_MODE:
            protocol_mode
                = hids_subevent_protocol_mode_get_protocol_mode(packet);
            printf("Protocol Mode: %s mode\n",
                   hids_subevent_protocol_mode_get_protocol_mode(packet)
                       ? "Report"
                       : "Boot");
            break;
        case HIDS_SUBEVENT_CAN_SEND_NOW:
            mousing_can_send_now();
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

bool contains(uint8_t val, const uint8_t* arr, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (arr[i] == val)
        {
            return true;
        }
    }
    return false;
}

static const uint8_t btn_gpios[] = {
    1, 2, 3, 4, 10, 11, 12, 13, 14,
};

static const uint8_t dpad_gpios[] = {
    19,
    20,
    21,
    22,
};

static const uint8_t gpio_bit_map[] = {
    0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 1, 2, 3,
};

static const uint8_t dpad_enum_map[] = {
    HATSWITCH_NONE, // 0b0000
    HATSWITCH_UP, // 0b0001
    HATSWITCH_DOWN, // 0b0010
    HATSWITCH_NONE, // 0b0011
    HATSWITCH_LEFT, // 0b0100
    HATSWITCH_NONE, // 0b0101
    HATSWITCH_NONE, // 0b0110
    HATSWITCH_NONE, // 0b0111
    HATSWITCH_RIGHT, // 0b1000
    HATSWITCH_NONE, // 0b1001
    HATSWITCH_NONE, // 0b1010
    HATSWITCH_NONE, // 0b1011
    HATSWITCH_NONE, // 0b1100
    HATSWITCH_NONE, // 0b1101
    HATSWITCH_NONE, // 0b1110
    HATSWITCH_NONE, // 0b1111
};

static uint8_t dpad_gpio_state = 0;

void gpio_callback(uint gpio, uint32_t event_mask)
{
    if (con_handle == HCI_CON_HANDLE_INVALID)
        return;

    busy_wait_ms(2);
    uint8_t bit = gpio_bit_map[gpio];

    if (contains(gpio, btn_gpios, sizeof(btn_gpios)))
    {
        if (!gpio_get(gpio))
            buttons |= BIT(bit);
        if (gpio_get(gpio))
            buttons &= IBIT(bit);
    }
    else
    {
        if (!gpio_get(gpio))
            dpad_gpio_state |= BIT(bit);
        if (gpio_get(gpio))
            dpad_gpio_state &= IBIT(bit);
        hat = dpad_enum_map[dpad_gpio_state];
    }

    hids_device_request_can_send_now_event(con_handle);
}

int btstack_main(void)
{
    hog_mouse_setup();

    // turn on!
    hci_power_control(HCI_POWER_ON);

    for (size_t i = 0; i < sizeof(btn_gpios); i++)
    {
        gpio_pull_up(btn_gpios[i]);
        gpio_set_irq_enabled_with_callback(
            btn_gpios[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
            gpio_callback);
    }

    for (size_t i = 0; i < sizeof(dpad_gpios); i++)
    {
        gpio_pull_up(dpad_gpios[i]);
        gpio_set_irq_enabled_with_callback(
            dpad_gpios[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
            gpio_callback);
    }

    return 0;
}
