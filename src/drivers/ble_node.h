#ifndef SRC_DRIVERS_BLE_NODE_H
#define SRC_DRIVERS_BLE_NODE_H

#ifdef BLE_NODE
#include <inttypes.h>

namespace bluetooth_node {
    void init();

    template <typename TDataType>
    void transmit(BlePacket<TDataType>& packet);
}

#endif // BLE_NODE




#endif // SRC_DRIVERS_BLE_NODE_H
