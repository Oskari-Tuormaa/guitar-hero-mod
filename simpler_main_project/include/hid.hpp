#ifndef HID_H_INCLUDED
#define HID_H_INCLUDED

#include <cstdint>
#include <span>

namespace hid
{

enum HatSwitchDirections : uint8_t
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

struct __attribute__((packed)) GamepadReport
{
    uint16_t                 buttons : 9; // 9 buttons (1 bit each)
    uint8_t                  pad1 : 7;    // 7 bit padding for alignment
    enum HatSwitchDirections hat : 4;     // Hat switch (4 bits)
    uint8_t                  pad2 : 4;    // 4 bits padding for alignment
};

struct GamepadReport*    get_report();
std::span<const uint8_t> get_report_data();
std::span<const uint8_t> get_report_descriptor();

} // namespace hid

#endif // ifndef HID_H_INCLUDED
