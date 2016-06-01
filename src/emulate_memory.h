#ifndef EMULATE_MEMORY_H
#define EMULATE_MEMORY_H

#include <stdlib.h>
#include <stdint.h>

struct pi_state;

int init_pi_memory(struct pi_state *pstate);
uint8_t *get_memory(struct pi_state *pstate, size_t address);

#endif
