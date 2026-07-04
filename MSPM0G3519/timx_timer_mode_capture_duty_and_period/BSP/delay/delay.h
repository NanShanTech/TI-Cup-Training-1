#ifndef __DELAY_H__
#define __DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);

#ifdef __cplusplus
}
#endif

#endif /* __DELAY_H__ */
