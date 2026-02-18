/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   noise_texture.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:15:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 01:19:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H

#include "texture.h"
#include "perlin.h"
#include "color.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Noise texture type: wraps a perlin noise generator with frequency scale */
typedef struct s_noise_texture
{
	t_perlin perlin;
	real_t scale;
	bool use_turb;
	int turb_depth;
} t_noise_texture;

/* helper clamp */
static inline real_t rt_clamp(real_t x, real_t lo, real_t hi)
{
	if (x < lo)
		return lo;
	if (x > hi)
		return hi;
	return x;
}

/* Noise texture value function: supports optional turbulence */
static inline t_color noise_texture_value(const t_texture *tex, real_t u, real_t v, const t_point3 *p)
{
	(void)u;
	(void)v;
	const t_noise_texture *nt = (const t_noise_texture *)tex->data;
	if (!nt || !p)
		return vec3_create(0.0, 0.0, 0.0);

	/* use turb depth from struct or default to 7 */
	int depth = (nt->turb_depth > 0) ? nt->turb_depth : 7;

	/* compute turbulence (uses original point) */
	real_t t = perlin_turb(&nt->perlin, (const t_vec3 *)p, depth);

	/* Improved formula: smoother variation with sin-based pattern
	   formula: 0.5*(1 + sin(scale*p.z + 10*turb)) gives better gradation */
	double angle = (double)(nt->scale * p->z) + 10.0 * (double)t;
	real_t s = (real_t)sin(angle);

	/* Create grey value with smooth variation (avoids harsh transitions) */
	real_t base = (real_t)0.5 * ((real_t)1.0 + s);
	t_color out = vec3_create(base, base, base);

	return out;
}

/* destroy unchanged */
static inline void noise_texture_destroy(t_texture *tex)
{
	if (tex && tex->data)
	{
		free(tex->data);
		tex->data = NULL;
	}
}

/* constructor: no turbulence */
static inline t_texture *noise_texture_create(real_t scale)
{
	t_texture *tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return NULL;

	t_noise_texture *nt = (t_noise_texture *)malloc(sizeof(t_noise_texture));
	if (!nt)
	{
		free(tex);
		return NULL;
	}

	perlin_init(&nt->perlin);
	nt->scale = (scale > (real_t)0.0) ? scale : (real_t)1.0;
	nt->use_turb = false;
	nt->turb_depth = 0;

	tex->data = nt;
	tex->value = noise_texture_value;
	tex->destroy = noise_texture_destroy;
	return tex;
}

/* constructor: enable turbulence with depth */
static inline t_texture *noise_texture_create_turb(real_t scale, int turb_depth)
{
	t_texture *tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return NULL;

	t_noise_texture *nt = (t_noise_texture *)malloc(sizeof(t_noise_texture));
	if (!nt)
	{
		free(tex);
		return NULL;
	}

	perlin_init(&nt->perlin);
	nt->scale = (scale > (real_t)0.0) ? scale : (real_t)1.0;
	nt->use_turb = true;
	nt->turb_depth = (turb_depth > 0) ? turb_depth : 7;

	tex->data = nt;
	tex->value = noise_texture_value;
	tex->destroy = noise_texture_destroy;
	return tex;
}

#endif
