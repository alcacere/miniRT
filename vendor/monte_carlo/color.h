/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:52:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 00:41:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include "types.h"
#include "vector.h"
#include "ray.h"
#include "sphere.h"
#include "hittable_list.h"
#include "interval.h"
#include "material.h"

/* Return a color (t_vec3) by value, not a pointer */
static inline t_vec3 color_create(real_t x, real_t y, real_t z)
{
	return vec3_create(x, y, z);
}

/* helper: return max component of a color/vector */
static inline real_t vec3_max_component(const t_vec3 *v)
{
	real_t m = v->x;
	if (v->y > m)
		m = v->y;
	if (v->z > m)
		m = v->z;
	return m;
}

/* linear -> gamma (gamma = 2.0) with safe handling of negative inputs */
static inline real_t linear_to_gamma(real_t v)
{
	if (v > (real_t)0.0)
		return (real_t)sqrt((double)v);
	return (real_t)0.0;
}

/* Convert a [0,1] component to byte [0,255] with clamping (no gamma here) */
static inline int component_to_byte(real_t v, const t_interval *intensity)
{
	v = clamp(v, intensity->min, intensity->max);
	return (int)(256.0 * v);
}

/* Write pixel color: apply gamma, then clamp, then convert to byte */
static inline void write_color(FILE *out, const t_vec3 *pixel)
{
	real_t r = pixel->x;
	real_t g = pixel->y;
	real_t b = pixel->z;

	/* Apply linear->gamma transform (gamma=2) */
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	/* Clamp and convert to byte range [0,255] */
	static const t_interval intensity = {0.000, 0.999, true};
	int rbyte = component_to_byte(r, &intensity);
	int gbyte = component_to_byte(g, &intensity);
	int bbyte = component_to_byte(b, &intensity);

	fprintf(out, "%d %d %d\n", rbyte, gbyte, bbyte);
}

/* simple ray_color that uses hit_sphere for legacy tests (kept) */
static inline t_vec3 ray_color_legacy(const t_ray *r)
{
	t_vec3 center = vec3_create((real_t)0, (real_t)0, (real_t)-1);
	real_t t = hit_sphere(&center, (real_t)0.5, r);
	if (t > 0.0)
	{
		t_vec3 p = ray_at((t_ray *)r, (real_t)t);
		t_vec3 diff = vec3_sub(&p, &center);
		t_vec3 n = unit_vector(&diff);
		t_vec3 one = vec3_create((real_t)1, (real_t)1, (real_t)1);
		t_vec3 n_plus = vec3_add(&n, &one);
		return vec3_mul_scalar(&n_plus, (real_t)0.5);
	}
	t_vec3 unit_dir = unit_vector(&r->dir);
	real_t tt = 0.5 * (unit_dir.y + (real_t)1.0);
	t_vec3 white = vec3_create(1.0, 1.0, 1.0);
	t_vec3 blue = vec3_create(0.5, 0.7, 1.0);
	return vec3_lerp(&white, &blue, tt);
}

/* depth-limited ray-color implemented recursively.
   If material exists and scatters, use it; otherwise return black. */
static inline t_vec3 ray_color_depth(const t_ray *r, const t_hittable_list *world, int depth)
{
	if (depth <= 0)
		return vec3_zero();

	t_hit_record rec;
	if (hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
	{
		t_ray scattered;
		t_color attenuation;

		/* if material exists and scatters, use it */
		if (rec.mat && rec.mat->scatter(rec.mat, r, &rec, &attenuation, &scattered))
		{
			t_vec3 scattered_col = ray_color_depth(&scattered, world, depth - 1);
			return vec3_mul_elem(&attenuation, &scattered_col);
		}

		/* fallback: if no material or scatter returns false, return black */
		return vec3_zero();
	}

	/* Miss (background gradient) */
	t_vec3 unit_dir = unit_vector(&r->dir);
	real_t a = (real_t)0.5 * (unit_dir.y + (real_t)1.0);
	t_vec3 white = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	t_vec3 blue = vec3_create((real_t)0.3, (real_t)0.5, (real_t)1.0);
	return vec3_lerp(&white, &blue, a);
}

/* Public API: keep previous signature; use moderate recursion depth (25). */
static inline t_vec3 ray_color_world(const t_ray *r, const t_hittable_list *world)
{
	return ray_color_depth(r, world, 25);
}

#endif