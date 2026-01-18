#ifndef CPUID_H
#define CPUID_H

#include <stdint.h>

// Wrapper for the CPUID instruction
static inline void cpuid(int code, uint32_t *a, uint32_t *d, uint32_t *c, uint32_t *b) {
    __asm__ volatile("cpuid"
        : "=a"(*a), "=d"(*d), "=c"(*c), "=b"(*b)
        : "a"(code)
    );
}

#endif
