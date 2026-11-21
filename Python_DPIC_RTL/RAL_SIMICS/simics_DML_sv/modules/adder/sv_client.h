#ifndef SV_CLIENT_H
#define SV_CLIENT_H

#include <stdint.h>

void sv_write_a(uint32_t val);
void sv_write_b(uint32_t val);
uint32_t sv_read_sum(void);
void sv_reset(void);
void sv_disconnect(void);

#endif
