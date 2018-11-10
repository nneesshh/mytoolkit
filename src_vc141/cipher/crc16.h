#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern uint16_t crc16(const char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif/*CRC16_H*/
