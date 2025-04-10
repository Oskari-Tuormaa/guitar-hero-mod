//--------------------------------------------------------------------------------
// Report descriptor data in hex (length 116 bytes)
//--------------------------------------------------------------------------------

#include <array>
#include <cstdint>

// 05010905 A101A100 09351500 26FFFF35 0046FFFF 95017510 8102C005 01093615
// 0026FFFF 95017510 81020501 09371500 26FF0095 01750881 02750895 01810305
// 09190129 0A950A75 01810275 06950181 03050109 39150125 08350046 3B10660E
// 00750495 01814275 04950181 03750895 018103C0

//--------------------------------------------------------------------------------
// Decoded Application Collection
//--------------------------------------------------------------------------------

constexpr auto report_descriptor = std::to_array<uint8_t>({
    // clang-format off
    0x05, 0x01,                //   (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page 
    0x09, 0x05,                //   (LOCAL)  USAGE              0x00010005 Game Pad (Application Collection)  
    0xA1, 0x01,                //   (MAIN)   COLLECTION         0x01 Application (Usage=0x00010005: Page=Generic Desktop Page, Usage=Game Pad, Type=Application Collection)
    0xA1, 0x00,                //     (MAIN)   COLLECTION         0x00 Physical (Usage=0x0: Page=, Usage=, Type=) <-- Error: COLLECTION must be preceded by a known USAGE <-- Warning: USAGE type should be CP (Physical Collection)
    0x09, 0x35,                //       (LOCAL)  USAGE              0x00010035 Rz (Dynamic Value)  
    0x15, 0x00,              //       (GLOBAL) LOGICAL_MINIMUM    0x00 (0)  <-- Info: Consider replacing 15 00 with 14
    0x26, 0xFF, 0xFF,   //       (GLOBAL) LOGICAL_MAXIMUM    0xFFFF (-1)  <-- Info: Consider replacing 26 FFFF with 25 FF
    0x35, 0x00,              //       (GLOBAL) PHYSICAL_MINIMUM   0x00 (0)  <-- Info: Consider replacing 35 00 with 34
    0x46, 0xFF, 0xFF,   //       (GLOBAL) PHYSICAL_MAXIMUM   0xFFFF (-1)  <-- Info: Consider replacing 46 FFFF with 45 FF
    0x95, 0x01,              //       (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
    0x75, 0x10,              //       (GLOBAL) REPORT_SIZE        0x10 (16) Number of bits per field  
    0x81, 0x02,              //       (MAIN)   INPUT              0x00000002 (1 field x 16 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap  <-- Error: LOGICAL_MAXIMUM (-1) is less than LOGICAL_MINIMUM (0) <-- Error: PHYSICAL_MAXIMUM (-1) is less than PHYSICAL_MINIMUM (0)
    0xC0,                         //     (MAIN)   END_COLLECTION     Physical  <-- Warning: Physical units are still in effect PHYSICAL(MIN=0,MAX=-1) UNIT(0x,EXP=0)
    0x05, 0x01,              //     (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page <-- Redundant: USAGE_PAGE is already 0x0001
    0x09, 0x36,              //     (LOCAL)  USAGE              0x00010036 Slider (Dynamic Value)  
    0x15, 0x00,              //     (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
    0x26, 0xFF, 0xFF,   //     (GLOBAL) LOGICAL_MAXIMUM    0xFFFF (-1) <-- Redundant: LOGICAL_MAXIMUM is already -1 <-- Info: Consider replacing 26 FFFF with 25 FF
    0x95, 0x01,              //     (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
    0x75, 0x10,              //     (GLOBAL) REPORT_SIZE        0x10 (16) Number of bits per field <-- Redundant: REPORT_SIZE is already 16 
    0x81, 0x02,              //     (MAIN)   INPUT              0x00000002 (1 field x 16 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap  <-- Error: LOGICAL_MAXIMUM (-1) is less than LOGICAL_MINIMUM (0) <-- Error: PHYSICAL_MAXIMUM (-1) is less than PHYSICAL_MINIMUM (0)
    0x05, 0x01,              //     (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page <-- Redundant: USAGE_PAGE is already 0x0001
    0x09, 0x37,              //     (LOCAL)  USAGE              0x00010037 Dial (Dynamic Value)  
    0x15, 0x00,              //     (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
    0x26, 0xFF, 0x00,   //     (GLOBAL) LOGICAL_MAXIMUM    0x00FF (255)  
    0x95, 0x01,              //     (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
    0x75, 0x08,              //     (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field  
    0x81, 0x02,              //     (MAIN)   INPUT              0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap  <-- Error: PHYSICAL_MAXIMUM (-1) is less than PHYSICAL_MINIMUM (0)
    0x75, 0x08,              //     (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field <-- Redundant: REPORT_SIZE is already 8 
    0x95, 0x01,              //     (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
    0x81, 0x03,              //     (MAIN)   INPUT              0x00000003 (1 field x 8 bits) 1=Constant 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
    0x05, 0x09,              //     (GLOBAL) USAGE_PAGE         0x0009 Button Page 
    0x19, 0x01,              //     (LOCAL)  USAGE_MINIMUM      0x00090001 Button 1 Primary/trigger (Selector, On/Off Control, Momentary Control, or One Shot Control)  
    0x29, 0x0A,              //     (LOCAL)  USAGE_MAXIMUM      0x0009000A Button 10 (Selector, On/Off Control, Momentary Control, or One Shot Control)  
    0x95, 0x0A,              //     (GLOBAL) REPORT_COUNT       0x0A (10) Number of fields  
    0x75, 0x01,              //     (GLOBAL) REPORT_SIZE        0x01 (1) Number of bits per field  
    0x81, 0x02,              //     (MAIN)   INPUT              0x00000002 (10 fields x 1 bit) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap  <-- Error: REPORT_SIZE (1) is too small for LOGICAL_MAXIMUM (255) which needs 8 bits. <-- Error: PHYSICAL_MAXIMUM (-1) is less than PHYSICAL_MINIMUM (0)
    0x75, 0x06,              //     (GLOBAL) REPORT_SIZE        0x06 (6) Number of bits per field  
    0x95, 0x01,              //     (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
    0x81, 0x03,              //     (MAIN)   INPUT              0x00000003 (1 field x 6 bits) 1=Constant 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
    0x05, 0x01,              //     (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page 
    0x09, 0x39,              //     (LOCAL)  USAGE              0x00010039 Hat switch (Dynamic Value)  
    0x15, 0x01,              //     (GLOBAL) LOGICAL_MINIMUM    0x01 (1)  
    0x25, 0x08,              //     (GLOBAL) LOGICAL_MAXIMUM    0x08 (8)  
    0x35, 0x00,              //     (GLOBAL) PHYSICAL_MINIMUM   0x00 (0) <-- Redundant: PHYSICAL_MINIMUM is already 0 <-- Info: Consider replacing 35 00 with 34
    0x46, 0x3B, 0x10,   //     (GLOBAL) PHYSICAL_MAXIMUM   0x103B (4155)  
    0x66, 0x0E, 0x00,   //     (GLOBAL) UNIT               0x000E  (E=Reserved <-- Error: Measurement system type (E) is reserved)  <-- Info: Consider replacing 66 0E00 with 65 0E
    0x75, 0x04,              //     (GLOBAL) REPORT_SIZE        0x04 (4) Number of bits per field  
    0x95, 0x01,             //     (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
    0x81, 0x42,            //     (MAIN)   INPUT              0x00000042 (1 field x 4 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 1=Null 0=NonVolatile 0=Bitmap 
    0x75, 0x04,            //     (GLOBAL) REPORT_SIZE        0x04 (4) Number of bits per field <-- Redundant: REPORT_SIZE is already 4 
    0x95, 0x01,            //     (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
    0x81, 0x03,            //     (MAIN)   INPUT              0x00000003 (1 field x 4 bits) 1=Constant 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
    0x75, 0x08,            //     (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field  
    0x95, 0x01,            //     (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
    0x81, 0x03,            //     (MAIN)   INPUT              0x00000003 (1 field x 8 bits) 1=Constant 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
    0xC0,                        //   (MAIN)   END_COLLECTION     Application  <-- Warning: Physical units are still in effect PHYSICAL(MIN=0,MAX=4155) UNIT(0x0000000E,EXP=0)
    // clang-format on
});

//--------------------------------------------------------------------------------
// Generic Desktop Page inputReport (Device --> Host)
//--------------------------------------------------------------------------------

using inputReport_t = struct
{
    // No REPORT ID byte
    // Collection: CA:GamePad CP:
    uint16_t GD_GamePadRz; // Usage 0x00010035: Rz, Value = 0 to -1, Physical =
                           // Value x --1 Collection: CA:GamePad
    uint16_t GD_GamePadSlider; // Usage 0x00010036: Slider, Value = 0 to -1,
                               // Physical = Value x --1
    uint8_t GD_GamePadDial; // Usage 0x00010037: Dial, Value = 0 to 255,
                            // Physical = Value x -1 / 255
    uint8_t pad_4; // Pad
    uint8_t
        BTN_GamePadButton1 : 1; // Usage 0x00090001: Button 1 Primary/trigger,
                                // Value = 0 to 255, Physical = Value x -1 / 255
    uint8_t
        BTN_GamePadButton2 : 1; // Usage 0x00090002: Button 2 Secondary, Value =
                                // 0 to 255, Physical = Value x -1 / 255
    uint8_t
        BTN_GamePadButton3 : 1; // Usage 0x00090003: Button 3 Tertiary, Value =
                                // 0 to 255, Physical = Value x -1 / 255
    uint8_t BTN_GamePadButton4 : 1; // Usage 0x00090004: Button 4, Value = 0 to
                                    // 255, Physical = Value x -1 / 255
    uint8_t BTN_GamePadButton5 : 1; // Usage 0x00090005: Button 5, Value = 0 to
                                    // 255, Physical = Value x -1 / 255
    uint8_t BTN_GamePadButton6 : 1; // Usage 0x00090006: Button 6, Value = 0 to
                                    // 255, Physical = Value x -1 / 255
    uint8_t BTN_GamePadButton7 : 1; // Usage 0x00090007: Button 7, Value = 0 to
                                    // 255, Physical = Value x -1 / 255
    uint8_t BTN_GamePadButton8 : 1; // Usage 0x00090008: Button 8, Value = 0 to
                                    // 255, Physical = Value x -1 / 255
    uint8_t BTN_GamePadButton9 : 1; // Usage 0x00090009: Button 9, Value = 0 to
                                    // 255, Physical = Value x -1 / 255
    uint8_t BTN_GamePadButton10 : 1; // Usage 0x0009000A: Button 10, Value = 0
                                     // to 255, Physical = Value x -1 / 255
    uint8_t : 6; // Pad
    uint8_t
        GD_GamePadHatSwitch : 4; // Usage 0x00010039: Hat switch, Value = 1 to
                                 // 8, Physical = (Value - 1) x 4155 / 7 in
    uint8_t : 4; // Pad
    uint8_t pad_9; // Pad
};
