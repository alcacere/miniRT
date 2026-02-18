/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 17:09:23 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 15:57:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
#define RAY_H

#include "types.h"
#include "vector.h"

/**
 * the on thing that all ray tracers have in common
 * is the computation of what color is seen along the ray
 * . Let's think of a ray as a functoin `P(t) = A + tB`
 * Here P is a 3D position along a line in 3D. A si the origin
 * and B is the ray direction, Thre ray parameter t is a real
 * number. Plug in a differnt P(t) moves the point along
 * the ray. Add in negative t values and  we can go
 * anywhere on the 3D line. For positive t, we get only the
 * part in front of A.And this is waht is often called
 * a `half-line` or a ray.
 *
 * t=1				t=0				t=1			t=2
 * <--|--------------|:-------------->|-----------|----->
 * 					 A               B
 * 					linear interpolation
 */
typedef struct s_ray
{
	t_vec3 orig;
	t_vec3 dir;
	real_t tm;
} t_ray;

/* Construct a ray from origin, direction, and time */
static inline t_ray ray_create(t_vec3 origin, t_vec3 direction, real_t time)
{
	t_ray r;

	r.orig = origin;
	r.dir = direction;
	r.tm = time;
	return (r);
}

/* Construct a ray with default time = 0 */
static inline t_ray ray_create_default(t_vec3 origin, t_vec3 direction)
{
	return ray_create(origin, direction, (real_t)0.0);
}

/* Accessors (return pointers to avoid copying) */
static inline const t_vec3 *ray_origin(t_ray *ray)
{
	return (&ray->orig);
}

static inline const t_vec3 *ray_direction(t_ray *ray)
{
	return (&ray->dir);
}

static inline real_t ray_time(const t_ray *ray)
{
	return ray->tm;
}

/* Compute point along ray: P(t) = origin + t * direction */
static inline t_vec3 ray_at(t_ray *ray, real_t t)
{
	t_vec3 scaled;

	scaled = vec3_mul_scalar(&ray->dir, t);
	return (vec3_add(&ray->orig, &scaled));
}

#endif