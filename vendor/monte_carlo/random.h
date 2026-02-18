/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       random.h                                                        */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 22:54:09                                             */
/*  Updated:    2026/01/04 22:54:09                                             */
/*                                                                              */
/* ============================================================================ */

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "settings.h"
#include "types.h"

/* Fast xorshift64* RNG */
static inline uint64_t random_seed(uint64_t seed)
{
	if (seed == 0)
		seed = (uint64_t)time(NULL) ^ 0x9e3779b97f4a7c15ULL;
	return seed;
}

static inline uint64_t random_u64(void)
{
	/* thread-local state for thread-safe RNG */
	static __thread uint64_t state = 0;
	if (state == 0)
	{
		uint64_t t = (uint64_t)time(NULL);
		uint64_t addr = (uint64_t)(uintptr_t)&state;
#ifdef _OPENMP
		uint64_t tid = (uint64_t)omp_get_thread_num();
#else
		uint64_t tid = 0;
#endif
		state = random_seed(t ^ addr ^ (tid * 0x9e3779b97f4a7c15ULL));
	}

	/* xorshift64* */
	state ^= state >> 12;
	state ^= state << 25;
	state ^= state >> 27;
	return state * 0x2545F4914F6CDD1DULL;
}

/* return a random real in [0,1) using 53-bit mantissa scaling */
static inline real_t random_real(void)
{
	const uint64_t rnd = random_u64();
	const uint64_t mantissa = rnd >> 11;					/* keep top 53 bits */
	return (real_t)(mantissa * (1.0 / 9007199254740992.0)); /* 1 / 2^53 */
}

/* return a random real in [min,max) */
static inline real_t random_real_interval(real_t min, real_t max)
{
	return min + (max - min) * random_real();
}

/* Alias for random_real() */
static inline real_t random_double(void)
{
	return random_real();
}

/* return a random integer in [min,max] (inclusive) */
static inline int random_int(int min, int max)
{
	return min + (int)(random_real() * (real_t)(max - min + 1));
}

#endif