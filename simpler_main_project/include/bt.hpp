#ifndef BLUETOOTH_H_INCLUDED
#define BLUETOOTH_H_INCLUDED

namespace bt
{

bool is_connected();
void send_hid_report();
void init();

} // namespace bt

#endif // ifndef BLUETOOTH_H_INCLUDED
