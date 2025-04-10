#include "hid.hpp"

#include <array>

namespace hid
{

namespace
{

GamepadReport report = {};

// from USB HID Specification 1.1, Appendix B.2
const auto gamepad_hid_report_descriptor = std::to_array<uint8_t>({
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
});

} // namespace

GamepadReport* get_report()
{
    return &report;
}

std::span<const uint8_t> get_report_data()
{
    return { reinterpret_cast<uint8_t*>(&report), sizeof(GamepadReport) };
}

std::span<const uint8_t> get_report_descriptor()
{
    return gamepad_hid_report_descriptor;
}

} // namespace hid
