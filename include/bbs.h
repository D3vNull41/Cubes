#ifndef __BBS_H
#define __BBS_H

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <x86intrin.h>

// Type Definitions
#include "typedef.h"

// Constants for Blum Blum Shub (BBS) CSPRNG
#define P 4294967311UL  // First prime number
#define Q 1062232319UL  // Second prime number
#define N (P * Q)       // Modulus (N = P * Q)

// Inline Functions and Utilities

// Check for RDRAND support using CPUID
static inline I16 check_rdrand_support() {
    U32 ecx;
    __asm__ __volatile__(
        "mov $1, %%eax\n\t"
        "cpuid\n\t"
        : "=c" (ecx)  // Output in ecx
        :             // No input
        : "eax", "ebx", "edx"  // Clobbered registers
    );
    return (ecx >> 30) & 1;  // Check the 30th bit for RDRAND support
}

// Get a random seed using RDRAND instruction
static inline U32 get_rdrand_seed() {
    U32 seed;
    U8 success;
    __asm__ __volatile__(
        "rdrand %0\n\t"
        "setc %1"
        : "=r" (seed), "=qm" (success)  // Output in seed, success flag in lower 8 bits of rflags
        :                               // No input
        :                               // No clobbered registers
    );
    return success ? seed : 0;  // Return seed if success, 0 otherwise
}

// Get seed, falling back to time if RDRAND fails
static inline U32 get_seed() {
    if (check_rdrand_support()) {
        U32 seed = get_rdrand_seed();
        if (seed != 0) {
            return seed;
        }
    }
    // Fallback: Use current time
    return (U32)time(NULL);
}

// Optimized Blum Blum Shub CSPRNG with inlined modular reduction
static inline U32 bbs(U32 seed) {
    U64 x = (U64)seed * seed;  // (x_n+1)^2
    U32 res;
    __asm__ __volatile__(
        "divq %[modulus]\n\t"           // Divide x by N
        : "=a" (res)                    // Remainder in res (lower 32 bits of quotient)
        : "d" ((U32)(x >> 32)),         // High 32 bits of dividend
          "a" ((U32)x),                 // Low 32 bits of dividend
          [modulus] "r" (N)             // Divisor N
    );
    return res;
}
static inline U32 random_U32(U32 *seed) {
    *seed = bbs(*seed);
    return *seed;
}

// Generate a random float [0,1) using BBS
static inline F32 random_F32(U32 *seed) {
    *seed = bbs(*seed);
    return (F32)(*seed) / (F32)UINT32_MAX;
}

#ifdef BENCHMARK_BBS
// Benchmark comparison between BBS and rand()
void benchmark();
#endif // BENCHMARK_BBS

#endif // __BBS_H