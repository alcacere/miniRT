/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:07:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 23:45:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERLIN_H
#define PERLIN_H

#include "random.h"
#include "types.h"
#include "vector.h"
#include <stdlib.h>
#include <math.h>

#define POINT_COUNT 256

typedef struct s_perlin
{
	t_vec3 rand_vec[POINT_COUNT]; /* gradient vectors (unit) */
	int perm_x[POINT_COUNT];
	int perm_y[POINT_COUNT];
	int perm_z[POINT_COUNT];
} t_perlin;

/* Forward declaration */
static inline void perlin_permute(int *p, int n);

/* Generate a permutation array: fill with indices 0 to POINT_COUNT-1, then shuffle */
static inline void perlin_generate_perm(int *p)
{
	for (int i = 0; i < POINT_COUNT; i++)
		p[i] = i;
	perlin_permute(p, POINT_COUNT);
}

/* Fisher-Yates shuffle: permute array in-place */
static inline void perlin_permute(int *p, int n)
{
	for (int i = n - 1; i > 0; i--)
	{
		int target = random_int(0, i);
		int tmp = p[i];
		p[i] = p[target];
		p[target] = tmp;
	}
}

/* Hermitian smoothing: fade(t) = t*t*(3-2*t)
   This creates smooth interpolation with zero derivatives at endpoints */
static inline real_t perlin_fade(real_t t)
{
	return t * t * ((real_t)3.0 - (real_t)2.0 * t);
}

/* Gradient-based trilinear interpolation (uses dot of gradient and offset) */
static inline real_t perlin_interp(const t_vec3 c[2][2][2], real_t u, real_t v, real_t w)
{
	/* apply fade to coordinates */
	real_t uu = perlin_fade(u);
	real_t vv = perlin_fade(v);
	real_t ww = perlin_fade(w);

	double accum = 0.0;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				/* weight vector: offset from corner to point */
				t_vec3 weight = vec3_create(u - (real_t)i, v - (real_t)j, w - (real_t)k);
				double dot_g = dot(&c[i][j][k], &weight);

				double i_w = (i ? uu : (1.0 - uu));
				double j_w = (j ? vv : (1.0 - vv));
				double k_w = (k ? ww : (1.0 - ww));

				accum += i_w * j_w * k_w * dot_g;
			}
		}
	}

	return (real_t)accum;
}

/* Initialize Perlin: fill gradient vectors and permutations */
static inline void perlin_init(t_perlin *perlin)
{
	if (!perlin)
		return;

	for (int i = 0; i < POINT_COUNT; i++)
		perlin->rand_vec[i] = random_unit_vector();

	perlin_generate_perm(perlin->perm_x);
	perlin_generate_perm(perlin->perm_y);
	perlin_generate_perm(perlin->perm_z);
}

/* Compute Perlin noise (gradient noise) at point p */
static inline real_t perlin_noise(const t_perlin *perlin, const t_vec3 *p)
{
	if (!perlin || !p)
		return 0.0;

	/* fractional part inside unit cube */
	real_t u = p->x - floor((double)p->x);
	real_t v = p->y - floor((double)p->y);
	real_t w = p->z - floor((double)p->z);

	/* integer cube corner */
	int i = (int)floor((double)p->x);
	int j = (int)floor((double)p->y);
	int k = (int)floor((double)p->z);

	/* gather gradients for the 8 corners */
	t_vec3 c[2][2][2];
	for (int di = 0; di < 2; di++)
	{
		for (int dj = 0; dj < 2; dj++)
		{
			for (int dk = 0; dk < 2; dk++)
			{
				int idx = perlin->perm_x[(i + di) & 255] ^
						  perlin->perm_y[(j + dj) & 255] ^
						  perlin->perm_z[(k + dk) & 255];
				c[di][dj][dk] = perlin->rand_vec[idx];
			}
		}
	}

	/* interpolate using gradient-dot-offset */
	return perlin_interp(c, u, v, w);
}

/* Turbulence: accumulate multiple octaves of noise.
   depth: number of octaves to accumulate (typical 7)
   returns absolute value of accumulated signal (non-negative) */
static inline real_t perlin_turb(const t_perlin *perlin, const t_vec3 *p, int depth)
{
	if (!perlin || !p || depth <= 0)
		return 0.0;

	real_t accum = 0.0;
	real_t weight = 1.0;
	t_vec3 temp = *p;

	for (int i = 0; i < depth; ++i)
	{
		accum += weight * perlin_noise(perlin, &temp);
		weight *= 0.5;
		temp = vec3_mul_scalar(&temp, (real_t)2.0);
	}

	/* return absolute value (classic turbulence) */
	return (real_t)fabs((double)accum);
}

#endif