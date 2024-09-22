#include "pubsub.hpp"

#include <cstdint>

namespace Topic
{

    struct BluetoothReady : public PubSub<BluetoothReady>
    { };
    struct BluetoothConnected : public PubSub<BluetoothConnected>
    {
        bool connected;
    };
    struct BluetoothDisconnected : public PubSub<BluetoothDisconnected>
    { };

    struct ButtonStateChange : public PubSub<ButtonStateChange>
    {
        enum class ButtonState
        {
            PRESSED,
            RELEASED
        };
        ButtonState new_state;
        uint32_t    button_gpio;
    };

} // namespace Topic
