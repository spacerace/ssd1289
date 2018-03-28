#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <inttypes.h>
#include <stdint.h>

extern uint32_t random_xorshift32();
extern uint32_t random_kiss32();

#endif // __RANDOM_H__
