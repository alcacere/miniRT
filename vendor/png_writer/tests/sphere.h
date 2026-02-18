/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 14:30:52 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 17:22:49 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPHERE_H
#define SPHERE_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "point.h"
#include "hittable.h"
#include "interval.h"
#include "aabb.h"
#include <math.h>
#include <stdbool.h>

/* Forward declaration to avoid circular dependency */
typedef struct s_material t_material;

/* Moving sphere: center1 + time * (center2 - center1) */
typedef struct s_moving_sphere
{
	t_vec3 center1;
	t_vec3 center_velocity; /* center2 - center1 */
} t_moving_sphere;

/* Sphere type: moving center, radius, albedo, material pointer, and bounding box */
typedef struct s_sphere
{
	t_moving_sphere center;
	real_t radius;
	t_vec3 albedo;
	t_aabb bbox;
	t_material *mat; /* pointer to material that determines scattering */
} t_sphere;

/* Compute sphere center at given time: center1 + time * velocity */
static inline t_vec3 sphere_center_at(const t_sphere *s, real_t time)
{
	t_vec3 scaled = vec3_mul_scalar(&s->center.center_velocity, time);
	return vec3_add(&s->center.center1, &scaled);
}

/* Get UV coordinates for a point on a unit sphere centered at origin.
   p: a point on the sphere (assumed normalized or at least on the sphere surface).
   u: returned value [0,1] of angle around Y axis from X=-1.
   v: returned value [0,1] of angle from Y=-1 to Y=+1.

   Examples:
   <1, 0, 0> yields <0.50, 0.50>       <-1, 0, 0> yields <0.00, 0.50>
   <0, 1, 0> yields <0.50, 1.00>       < 0,-1, 0> yields <0.50, 0.00>
   <0, 0, 1> yields <0.25, 0.50>       < 0, 0,-1> yields <0.75, 0.50>
*/
static inline void sphere_get_uv(const t_vec3 *p, real_t *u, real_t *v)
{
	/* theta = acos(-p.y): angle from Y=-1 to Y=+1 */
	real_t theta = (real_t)acos((double)(-p->y));

	/* phi = atan2(-p.z, p.x) + pi: angle around Y axis */
	real_t phi = (real_t)atan2((double)(-p->z), (double)(p->x)) + (real_t)PI;

	/* u in [0, 1] from phi in [0, 2*pi] */
	*u = phi / ((real_t)2.0 * (real_t)PI);

	/* v in [0, 1] from theta in [0, pi] */
	*v = theta / (real_t)PI;
}

/* create_sphere: stationary sphere (center2 = center1, velocity = 0) */
static inline t_sphere create_sphere(const t_point3 *center, real_t radius, t_vec3 albedo, t_material *mat)
{
	t_sphere s;
	s.center.center1 = vec3_create(center->x, center->y, center->z);
	s.center.center_velocity = vec3_zero();
	s.radius = (radius > 0.0) ? radius : 0.0;
	s.albedo = albedo;
	s.mat = mat;

	/* Compute bounding box for stationary sphere */
	t_vec3 rvec = vec3_create(s.radius, s.radius, s.radius);
	t_point3 center_low = point3_create(center->x - s.radius, center->y - s.radius, center->z - s.radius);
	t_point3 center_high = point3_create(center->x + s.radius, center->y + s.radius, center->z + s.radius);
	s.bbox = aabb_from_points(&center_low, &center_high);

	return s;
}

/* create_sphere_moving: moving sphere (center1 to center2) */
static inline t_sphere create_sphere_moving(const t_point3 *center1, const t_point3 *center2,
											real_t radius, t_vec3 albedo, t_material *mat)
{
	t_sphere s;
	s.center.center1 = vec3_create(center1->x, center1->y, center1->z);
	t_vec3 c2 = vec3_create(center2->x, center2->y, center2->z);
	s.center.center_velocity = vec3_sub(&c2, &s.center.center1);
	s.radius = (radius > 0.0) ? radius : 0.0;
	s.albedo = albedo;
	s.mat = mat;

	/* Compute bounding boxes at t=0 and t=1, then merge them */
	t_point3 c0_low = point3_create(center1->x - s.radius, center1->y - s.radius, center1->z - s.radius);
	t_point3 c0_high = point3_create(center1->x + s.radius, center1->y + s.radius, center1->z + s.radius);
	t_aabb box0 = aabb_from_points(&c0_low, &c0_high);

	t_point3 c1_low = point3_create(center2->x - s.radius, center2->y - s.radius, center2->z - s.radius);
	t_point3 c1_high = point3_create(center2->x + s.radius, center2->y + s.radius, center2->z + s.radius);
	t_aabb box1 = aabb_from_points(&c1_low, &c1_high);

	s.bbox = aabb_merge(&box0, &box1);

	return s;
}

/* create_sphere_default: stationary sphere with white albedo and no material */
static inline t_sphere create_sphere_default(const t_point3 *center, real_t radius)
{
	t_sphere s;
	s.center.center1 = vec3_create(center->x, center->y, center->z);
	s.center.center_velocity = vec3_zero();
	s.radius = (radius > 0.0) ? radius : 0.0;
	s.albedo = vec3_create(1.0, 1.0, 1.0);
	s.mat = NULL;

	/* Compute bounding box for stationary sphere */
	t_point3 center_low = point3_create(center->x - s.radius, center->y - s.radius, center->z - s.radius);
	t_point3 center_high = point3_create(center->x + s.radius, center->y + s.radius, center->z + s.radius);
	s.bbox = aabb_from_points(&center_low, &center_high);

	return s;
}

/* simple t-only test used by earlier code paths */
static inline real_t hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	t_vec3 oc = vec3_sub(center, &r->orig);
	real_t a = vec3_length_squared(&r->dir);
	real_t h = (real_t)dot(&r->dir, &oc);
	real_t c = vec3_length_squared(&oc) - radius * radius;
	real_t discriminant = (real_t)h * (real_t)h - (real_t)a * (real_t)c;
	if (discriminant < 0.0)
		return -1.0;
	real_t root = sqrt(discriminant);
	return ((real_t)h - root) / (real_t)a;
}

/* Module-local current sphere for no-obj 4-arg hit calls */
static const t_sphere *g_current_sphere = NULL;
static inline void set_current_sphere(const void *obj) { g_current_sphere = (const t_sphere *)obj; }

/* 4-arg sphere hit: now uses time-dependent center, computes UV, and assigns material to rec->mat */
static inline bool sphere_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_sphere *s = g_current_sphere;
	if (!s)
		return false;

	/* Get sphere center at ray time */
	t_vec3 current_center = sphere_center_at(s, r->tm);

	t_vec3 oc = vec3_sub(&r->orig, &current_center);
	real_t a = vec3_length_squared(&r->dir);
	real_t half_b = dot(&r->dir, &oc);
	real_t c = (real_t)vec3_length_squared(&oc) - (real_t)s->radius * (real_t)s->radius;
	real_t discriminant = half_b * half_b - (real_t)a * c;
	if (discriminant < 0.0)
		return false;
	real_t sqrtd = sqrt(discriminant);
	/* try the nearer root first; if it is outside the interval try the farther */
	real_t root = (-half_b - sqrtd) / (real_t)a;
	if (!contains(rayt.min, rayt.max, root))
	{
		root = (-half_b + sqrtd) / (real_t)a;
		if (!contains(rayt.min, rayt.max, root))
			return false;
	}
	rec->t = (real_t)root;
	rec->p = ray_at((t_ray *)r, rec->t);
	t_vec3 tmp = vec3_sub(&rec->p, &current_center);
	t_vec3 outward_normal = unit_vector(&tmp);
	set_face_normal(rec, r, &outward_normal);

	/* Compute UV coordinates from the outward normal */
	sphere_get_uv(&outward_normal, &rec->u, &rec->v);

	/* set per-hit albedo and material from sphere */
	rec->albedo = s->albedo;
	rec->mat = s->mat;
	return true;
}

#endif