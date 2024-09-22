#include <cstdio>

#include <btstack.h>

#include "bt_init.hpp"
#include "bt_reconnect.hpp"
#include "topics.hpp"

namespace globals
{
    static bool                        trying_to_connect = false;
    static btstack_link_key_iterator_t link_key_it;
} // namespace globals

void link_key_iterator_init()
{
    gap_link_key_iterator_init(&globals::link_key_it);
}

void link_key_iterator_done()
{
    if (globals::trying_to_connect)
    {
        gap_link_key_iterator_done(&globals::link_key_it);
        globals::trying_to_connect = false;
    }
}

void link_key_iterator_try_next()
{
    bd_addr_t       addr;
    link_key_t      link_key;
    link_key_type_t type;

    if (gap_link_key_iterator_get_next(&globals::link_key_it, addr, link_key,
                                       &type))
    {
        printf("Trying to connect to %s - type %u...\n", bd_addr_to_str(addr),
               (int)type);
        hid_device_connect(addr, get_hid_cid());
    }
    else
    {
        printf("Done trying...\n");
        link_key_iterator_done();
    }
}

void init_reconnect()
{
    Topic::BluetoothReady::subscribe([] (auto){
        link_key_iterator_init();
        link_key_iterator_try_next();
    });

    Topic::BluetoothConnected::subscribe([](auto ev) {
        if (ev.connected)
        {
            link_key_iterator_done();
        }
        else
        {
            link_key_iterator_try_next();
        }
    });

    Topic::BluetoothDisconnected::subscribe([](auto) {
        link_key_iterator_init();
        link_key_iterator_try_next();
    });
}
