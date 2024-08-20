#pragma once

#include <cstdint>

#include <array>

namespace config
{
    constexpr uint8_t report_id = 0x01;
} // namespace config

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
    0x09, 0x35,                  //     USAGE (Z)
    0x15, 0x81,                  //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                  //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                  //     REPORT_SIZE (8)
    0x95, 0x01,                  //     REPORT_COUNT (1)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Buttons
    0x05, 0x09,                  //     USAGE_PAGE (Button)
    0x19, 0x01,                  //     USAGE_MINIMUM (1)
    0x29, 0x11,                  //     USAGE_MAXIMUM (17)
    0x75, 0x01,                  //     REPORT_SIZE (1)
    0x95, 0x11,                  //     REPORT_COUNT (17)
    0x81, 0x02,                  //     INPUT (Data,Var,Abs)

    // Hat switch
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

    0xc0,                             //     END_COLLECTION
    0xc0,                             // END_COLLECTION
    // clang-format on
});

using report_t = struct
{
    uint8_t x;
    uint8_t y;

    uint8_t rx;
    uint8_t ry;

    uint8_t z;
    uint8_t rz;

    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
    uint8_t b8 : 1;
    uint8_t b9 : 1;
    uint8_t b10 : 1;
    uint8_t b11 : 1;
    uint8_t b12 : 1;
    uint8_t b13 : 1;
    uint8_t b14 : 1;
    uint8_t b15 : 1;
    uint8_t b16 : 1;
    uint8_t b17 : 1;

    uint8_t hat : 4;
};
