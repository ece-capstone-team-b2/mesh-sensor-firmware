#ifndef SRC_DRIVERS_CRC_H_
#define SRC_DRIVERS_CRC_H_

#include <inttypes.h>

uint16_t crc16(const uint8_t* data, 
                const uint16_t size);

#endif  // SRC_DRIVERS_CRC_H_