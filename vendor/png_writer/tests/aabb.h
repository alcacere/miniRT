/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 16:17:15 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 16:55:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

	for (int axis = 0; axis < 3; ++axis)
	{
		const t_interval *ax = aabb_axis_interval(box, axis);
		real_t ray_orig_axis;
		real_t ray_dir_axis;

		/* Get axis component from ray origin and direction */
		if (axis == 0)
		{
			ray_orig_axis = r->orig.x;
			ray_dir_axis = r->dir.x;
		}
		else if (axis == 1)
		{
			ray_orig_axis = r->orig.y;
			ray_dir_axis = r->dir.y;
		}
		else
		{
			ray_orig_axis = r->orig.z;
			ray_dir_axis = r->dir.z;
		}

		/* Compute t values for this axis */
		real_t adinv = (real_t)1.0 / ray_dir_axis;
		real_t t0 = (ax->min - ray_orig_axis) * adinv;
		real_t t1 = (ax->max - ray_orig_axis) * adinv;

		/* Swap if needed to ensure t0 <= t1 */
		if (t0 < t1)
		{
			if (t0 > ray_t->min)
				ray_t->min = t0;
			if (t1 < ray_t->max)
				ray_t->max = t1;
		}
		else
		{
			if (t1 > ray_t->min)
				ray_t->min = t1;
			if (t0 < ray_t->max)
				ray_t->max = t0;
		}

		/* Early exit: no intersection */
		if (ray_t->max <= ray_t->min)
			return false;
	}

	return true;
}

#endif