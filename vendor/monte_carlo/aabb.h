/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       aabb.h                                                          */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 22:08:19                                             */
/*  Updated:    2026/01/04 22:08:19                                             */
/*                                                                              */
/* ============================================================================ */

#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"
#include "point.h"
#include <stdbool.h>

/* Axis-Aligned Bounding Box */
typedef struct s_aabb
{
	t_interval x;
	t_interval y;
	t_interval z;
} t_aabb;

/* Default empty AABB */
static inline t_aabb aabb_empty(void)
{
	return ((t_aabb){
		.x = interval(INFINITY, -INFINITY),
		.y = interval(INFINITY, -INFINITY),
		.z = interval(INFINITY, -INFINITY)});
}

/* AABB from three intervals */
static inline t_aabb aabb_from_intervals(const t_interval *x, const t_interval *y, const t_interval *z)
{
	return ((t_aabb){.x = *x, .y = *y, .z = *z});
}

/* AABB from two points (extrema) */
static inline t_aabb aabb_from_points(const t_point3 *a, const t_point3 *b)
{
	t_aabb box;
	box.x = (a->x <= b->x) ? interval(a->x, b->x) : interval(b->x, a->x);
	box.y = (a->y <= b->y) ? interval(a->y, b->y) : interval(b->y, a->y);
	box.z = (a->z <= b->z) ? interval(a->z, b->z) : interval(b->z, a->z);
	return box;
}

/* AABB from merging two bounding boxes */
static inline t_aabb aabb_merge(const t_aabb *box0, const t_aabb *box1)
{
	return ((t_aabb){
		.x = interval_merge(&box0->x, &box1->x),
		.y = interval_merge(&box0->y, &box1->y),
		.z = interval_merge(&box0->z, &box1->z)});
}

/* Get axis interval by index (0=x, 1=y, 2=z) */
static inline const t_interval *aabb_axis_interval(const t_aabb *box, int axis)
{
	if (axis == 1)
		return &box->y;
	if (axis == 2)
		return &box->z;
	return &box->x;
}

/* Return the index of the longest axis (0=x, 1=y, 2=z) */
static inline int aabb_longest_axis(const t_aabb *box)
{
	real_t x_size = interval_size(&box->x);
	real_t y_size = interval_size(&box->y);
	real_t z_size = interval_size(&box->z);

	if (x_size > y_size)
		return (x_size > z_size) ? 0 : 2;
	else
		return (y_size > z_size) ? 1 : 2;
}

/* Ray-AABB intersection test */
static inline bool aabb_hit(const t_aabb *box, const t_ray *r, t_interval *ray_t)
{
	if (!box || !r || !ray_t)
		return false;

	real_t inv_x = (real_t)1.0 / r->dir.x;
	real_t inv_y = (real_t)1.0 / r->dir.y;
	real_t inv_z = (real_t)1.0 / r->dir.z;

	real_t t0, t1;

	/* X slab */
	t0 = (box->x.min - r->orig.x) * inv_x;
	t1 = (box->x.max - r->orig.x) * inv_x;
	if (t0 > t1)
	{
		real_t tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	if (t0 > ray_t->min)
		ray_t->min = t0;
	if (t1 < ray_t->max)
		ray_t->max = t1;
	if (ray_t->max <= ray_t->min)
		return false;

	/* Y slab */
	t0 = (box->y.min - r->orig.y) * inv_y;
	t1 = (box->y.max - r->orig.y) * inv_y;
	if (t0 > t1)
	{
		real_t tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	if (t0 > ray_t->min)
		ray_t->min = t0;
	if (t1 < ray_t->max)
		ray_t->max = t1;
	if (ray_t->max <= ray_t->min)
		return false;

	/* Z slab */
	t0 = (box->z.min - r->orig.z) * inv_z;
	t1 = (box->z.max - r->orig.z) * inv_z;
	if (t0 > t1)
	{
		real_t tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	if (t0 > ray_t->min)
		ray_t->min = t0;
	if (t1 < ray_t->max)
		ray_t->max = t1;
	if (ray_t->max <= ray_t->min)
		return false;

	return true;
}

static inline t_aabb aabb_add_vec3(const t_aabb *box, const t_vec3 *offset)
{
	t_interval nx = interval_add(&box->x, offset->x);
	t_interval ny = interval_add(&box->y, offset->y);
	t_interval nz = interval_add(&box->z, offset->z);
	return aabb_from_intervals(&nx, &ny, &nz);
}

#endif