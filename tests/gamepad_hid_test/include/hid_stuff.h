#pragma once

#include <cstdint>

#include <array>

namespace config
{
    constexpr uint8_t report_id = 0x01;
} // namespace config

#define BTN_A        0x01
#define BTN_B        0x02
#define BTN_C        0x03
#define BTN_X        0x04
#define BTN_Y        0x05
#define BTN_Z        0x06
#define BTN_TL       0x07
#define BTN_TR       0x08
#define BTN_TL2      0x09
#define BTN_TR2      0x0a
#define BTN_SELECT   0x0b
#define BTN_START    0x0c
#define BTN_GUIDE    0x0d
#define BTN_THUMBL   0x0e
#define BTN_THUMBR   0x0f
#define BTN_END      0x0f
#define BTN_USAGE(x) 0x09, x

#define HID_BUTTON_USAGES                                                      \
    BTN_USAGE(BTN_A), BTN_USAGE(BTN_B), BTN_USAGE(BTN_Y), BTN_USAGE(BTN_X),    \
        BTN_USAGE(BTN_TL), BTN_USAGE(BTN_TR), BTN_USAGE(BTN_SELECT),           \
        BTN_USAGE(BTN_START), BTN_USAGE(BTN_GUIDE), BTN_USAGE(BTN_C),          \
        BTN_USAGE(BTN_Z), BTN_USAGE(BTN_TL2), BTN_USAGE(BTN_TR2)

#define HID_BUTTON_COUNT 13
#define HID_BUTTON_PADDING                                                     \
    ((HID_BUTTON_COUNT % 8) ? (8 - (HID_BUTTON_COUNT % 8)) : 0)

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
    0x15, 0x81,                  //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                  //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                  //     REPORT_SIZE (8)
    0x95, 0x02,                  //     REPORT_COUNT (2)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Rx & Ry -> 2x8 = 16 bits
    0x05, 0x01,                  //     USAGE_PAGE (Generic Desktop)
    0x09, 0x33,                  //     USAGE (Rx)
    0x09, 0x34,                  //     USAGE (Ry)
    0x15, 0x81,                  //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                  //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                  //     REPORT_SIZE (8)
    0x95, 0x02,                  //     REPORT_COUNT (2)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Z = 8 bits
    0x05, 0x01,                  //     USAGE_PAGE (Generic Desktop)
    0x09, 0x32,                  //     USAGE (Z)
    0x15, 0x81,                  //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                  //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                  //     REPORT_SIZE (8)
    0x95, 0x01,                  //     REPORT_COUNT (1)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Rz = 8 bits
    0x05, 0x01,                  //     USAGE_PAGE (Generic Desktop)
    0x09, 0x35,                  //     USAGE (Rz)
    0x15, 0x81,                  //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                  //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                  //     REPORT_SIZE (8)
    0x95, 0x01,                  //     REPORT_COUNT (1)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    0x05, 0x09,              //   Usage Page (Button)
    HID_BUTTON_USAGES,
    0x15, 0x00,              //   Logical Minimum (0)
    0x25, 0x01,              //   Logical Maximum (1)
    0x35, 0x00,              //   Physical Minimum (0)
    0x45, 0x01,              //   Physical Maximum (1)
    0x75, 0x01,              //   Report Size (1)
    0x95, HID_BUTTON_COUNT,  //   Report Count (HID_BUTTON_COUNT)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

    // Buttons = 17 bits
//     0x05, 0x09,                  //     USAGE_PAGE (Button)
//     0x19, 0x01,                  //     USAGE_MINIMUM (1)
//     0x29, 0x11,                  //     USAGE_MAXIMUM (17)
//     0x75, 0x01,                  //     REPORT_SIZE (1)
//     0x95, 0x11,                  //     REPORT_COUNT (17)
//     0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Padding to byte align the hat switch data = 7 bits
    0x75, 0x01,                  //     REPORT_SIZE (1)
    0x95, HID_BUTTON_PADDING,    //     REPORT_COUNT (HID_BUTTON_PADDING)
    0x81, 0x03,                  //     INPUT (Cnst,Var,Abs)

    // Hat switch = 4 bits
    0x05, 0x01,                  //     USAGE_PAGE (Generic Desktop)
    0x09, 0x39,                  //     USAGE (Hat switch)
    0x15, 0x00,                  //     LOGICAL_MINIMUM (0)
    0x25, 0x07,                  //     LOGICAL_MAXIMUM (7)
    0x35, 0x00,
    0x46, 0x3B, 0x01,
    0x65, 0x14,
    0x75, 0x04,                  //     REPORT_SIZE (4)
    0x95, 0x01,                  //     REPORT_COUNT (1)
    0x81, 0x42,                 //     INPUT (Data,Var,Abs)

    // Padding to complete the byte for the hat switch
    0x95, 0x04,                  //     REPORT_SIZE (4)
    0x75, 0x01,                  //     REPORT_COUNT (1)
    0x81, 0x03,                  //     INPUT (Cnst,Var,Abs)

    0xc0,                             //     END_COLLECTION
    0xc0,                             // END_COLLECTION
    // clang-format on
});

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

using report_t = struct
{
    uint8_t x;
    uint8_t y;

    uint8_t rx;
    uint8_t ry;

    uint8_t z;
    uint8_t rz;

    bool b0 : 1;
    bool b1 : 1;
    bool b2 : 1;
    bool b3 : 1;
    bool b4 : 1;
    bool b5 : 1;
    bool b8 : 1;
    bool b9 : 1;
    bool b16 : 1;
    bool b17 : 1;
    bool b18 : 1;
    bool b6 : 1;
    bool b7 : 1;

    uint8_t : HID_BUTTON_PADDING;

    uint8_t hat : 4;

    uint8_t : 4;
};
