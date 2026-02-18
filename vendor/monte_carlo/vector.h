/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       vector.h                                                        */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 22:17:01                                             */
/*  Updated:    2026/01/04 22:17:01                                             */
/*                                                                              */
/* ============================================================================ */

#ifndef VECTOR_H
#define VECTOR_H

#include "types.h"
#include "settings.h"
#include <math.h>
#include <stdbool.h>

/* 3D vector type */
typedef struct s_vec3
{
	real_t x;
	real_t y;
	real_t z;
} t_vec3;

/* Constructor */
static inline t_vec3 vec3_create(real_t x, real_t y, real_t z)
{
	return ((t_vec3){.x = x, .y = y, .z = z});
}

/* Zero vector */
static inline t_vec3 vec3_zero(void)
{
	return vec3_create((real_t)0.0, (real_t)0.0, (real_t)0.0);
}

/* Negation */
static inline t_vec3 vec3_neg(const t_vec3 *v)
{
	return vec3_create(-v->x, -v->y, -v->z);
}

/* Addition */
static inline t_vec3 vec3_add(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->x + b->x, a->y + b->y, a->z + b->z);
}

/* Subtraction */
static inline t_vec3 vec3_sub(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->x - b->x, a->y - b->y, a->z - b->z);
}

/* Element-wise multiplication */
static inline t_vec3 vec3_mul_elem(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->x * b->x, a->y * b->y, a->z * b->z);
}

/* Scalar multiplication */
static inline t_vec3 vec3_mul_scalar(const t_vec3 *v, real_t t)
{
	return vec3_create(v->x * t, v->y * t, v->z * t);
}

/* Scalar division */
static inline t_vec3 vec3_div_scalar(const t_vec3 *v, real_t t)
{
	return vec3_mul_scalar(v, (real_t)1.0 / t);
}

/* Dot product */
static inline real_t dot(const t_vec3 *a, const t_vec3 *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

/* Cross product */
static inline t_vec3 cross(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(
		a->y * b->z - a->z * b->y,
		a->z * b->x - a->x * b->z,
		a->x * b->y - a->y * b->x);
}

/* Length squared */
static inline real_t vec3_length_squared(const t_vec3 *v)
{
	return v->x * v->x + v->y * v->y + v->z * v->z;
}

/* Length */
static inline real_t vec3_length(const t_vec3 *v)
{
	return (real_t)sqrt((double)vec3_length_squared(v));
}

/* Unit vector (normalize) */
static inline t_vec3 unit_vector(const t_vec3 *v)
{
	real_t len = vec3_length(v);
	if (len < (real_t)1e-10)
		return vec3_zero();
	return vec3_div_scalar(v, len);
}

/* Linear interpolation: lerp(a, b, t) = (1-t)*a + t*b */
static inline t_vec3 vec3_lerp(const t_vec3 *a, const t_vec3 *b, real_t t)
{
	t_vec3 one_minus_t = vec3_mul_scalar(a, (real_t)1.0 - t);
	t_vec3 t_term = vec3_mul_scalar(b, t);
	return vec3_add(&one_minus_t, &t_term);
}

/* Near zero check */
static inline bool vec3_near_zero(const t_vec3 *v)
{
	const real_t s = (real_t)1e-8;
	return (fabsl((long double)v->x) < (long double)s) &&
		   (fabsl((long double)v->y) < (long double)s) &&
		   (fabsl((long double)v->z) < (long double)s);
}

/* Reflect: v - 2*dot(v,n)*n */
static inline t_vec3 vec3_reflect(const t_vec3 *v, const t_vec3 *n)
{
	real_t d = dot(v, n);
	t_vec3 scaled = vec3_mul_scalar(n, (real_t)2.0 * d);
	return vec3_sub(v, &scaled);
}

/* Refract: Snell's law */
static inline t_vec3 vec3_refract(const t_vec3 *uv, const t_vec3 *n, real_t etai_over_etat)
{
	t_vec3 neg_uv = vec3_neg(uv);
	real_t cos_theta = dot(&neg_uv, n);
	if (cos_theta > (real_t)1.0)
		cos_theta = (real_t)1.0;

	t_vec3 n_scaled = vec3_mul_scalar(n, cos_theta);
	t_vec3 r_out_perp_sum = vec3_add(uv, &n_scaled);
	t_vec3 r_out_perp = vec3_mul_scalar(&r_out_perp_sum, etai_over_etat);

	real_t perp_len_sq = vec3_length_squared(&r_out_perp);
	real_t parallel_factor = (real_t)-sqrt((double)fabsl((long double)((real_t)1.0 - perp_len_sq)));
	t_vec3 r_out_parallel = vec3_mul_scalar(n, parallel_factor);

	return vec3_add(&r_out_perp, &r_out_parallel);
}

/* Forward declaration for random functions (defined after random.h is included) */
static inline real_t random_real(void);
static inline real_t random_real_interval(real_t min, real_t max);

/* Random vector in [0,1)^3 */
static inline t_vec3 vec3_random(void)
{
	return vec3_create(random_real(), random_real(), random_real());
}

/* Random vector in [min,max)^3 */
static inline t_vec3 vec3_random_interval(real_t min, real_t max)
{
	return vec3_create(
		random_real_interval(min, max),
		random_real_interval(min, max),
		random_real_interval(min, max));
}

/* Random point in unit sphere (rejection sampling) */
static inline t_vec3 random_in_unit_sphere(void)
{
	while (1)
	{
		t_vec3 p = vec3_random_interval((real_t)-1.0, (real_t)1.0);
		if (vec3_length_squared(&p) < (real_t)1.0)
			return p;
	}
}

/* Random unit vector (on unit sphere surface) */
static inline t_vec3 random_unit_vector(void)
{
	t_vec3 p = random_in_unit_sphere();
	return unit_vector(&p);
}

/* Random vector in hemisphere oriented by normal */
static inline t_vec3 random_on_hemisphere(const t_vec3 *normal)
{
	t_vec3 on_unit_sphere = random_unit_vector();
	if (dot(&on_unit_sphere, normal) > (real_t)0.0)
		return on_unit_sphere;
	return vec3_neg(&on_unit_sphere);
}

/* Random point in unit disk (for defocus blur) */
static inline t_vec3 random_in_unit_disk(void)
{
	while (1)
	{
		t_vec3 p = vec3_create(
			random_real_interval((real_t)-1.0, (real_t)1.0),
			random_real_interval((real_t)-1.0, (real_t)1.0),
			(real_t)0.0);
		if (vec3_length_squared(&p) < (real_t)1.0)
			return p;
	}
}

/* Cosine-weighted hemisphere sampling: generates direction with PDF = cos(theta)/pi
   This is the proper importance sampling for Lambertian (diffuse) surfaces.
   Uses the method: generate random point on unit disk, project up to hemisphere */
static inline t_vec3 random_cosine_direction(const t_vec3 *normal)
{
	/* Generate two random numbers */
	real_t r1 = random_real();
	real_t r2 = random_real();

	/* Cosine-weighted sampling on hemisphere:
	   phi = 2 * pi * r1
	   cos(theta) = sqrt(r2), sin(theta) = sqrt(1 - r2) */
	real_t phi = (real_t)(2.0 * PI) * r1;
	real_t cos_theta = (real_t)sqrt((double)r2);
	real_t sin_theta = (real_t)sqrt((double)((real_t)1.0 - r2));

	/* Create local direction in tangent space (z = normal direction) */
	real_t x = (real_t)cos((double)phi) * sin_theta;
	real_t y = (real_t)sin((double)phi) * sin_theta;
	real_t z = cos_theta;

	/* Build orthonormal basis around normal */
	t_vec3 w = unit_vector(normal);

	/* Choose a vector not parallel to w for cross product */
	t_vec3 a;
	if (fabsl((long double)w.x) > (long double)0.9)
		a = vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0);
	else
		a = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);

	/* Create orthonormal basis: u, v, w */
	t_vec3 v_cross = cross(&w, &a);
	t_vec3 v = unit_vector(&v_cross);
	t_vec3 u = cross(&w, &v);

	/* Transform local direction to world space */
	t_vec3 u_scaled = vec3_mul_scalar(&u, x);
	t_vec3 v_scaled = vec3_mul_scalar(&v, y);
	t_vec3 w_scaled = vec3_mul_scalar(&w, z);
	t_vec3 uv = vec3_add(&u_scaled, &v_scaled);
	return vec3_add(&uv, &w_scaled);
}

#endif