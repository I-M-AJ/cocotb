#ifndef SV_CLIENT_H
#define SV_CLIENT_H

#include <stdint.h>

int sv_buffer_push(uint32_t word);
int sv_buffer_result(uint32_t *word);
uint32_t sv_buffer_status(void);
int sv_buffer_shutdown(void);

#endif
