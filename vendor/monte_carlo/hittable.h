/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 19:37:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 23:22:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
#define HITTABLE_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "aabb.h"
#include <stdbool.h>

/* Forward declaration of material to avoid circular dependency */
typedef struct s_material t_material;
/* Forward declaration of hit record for callback typedefs */
typedef struct s_hit_record t_hit_record;

/* Generic wrapper callbacks */
typedef void (*t_set_current_fn)(const void *obj);
typedef bool (*t_hit_noobj_fn)(const t_ray *r, t_interval rayt, t_hit_record *rec);

/* Generic hittable wrapper (used by lists, BVH, transforms) */
typedef struct s_hittable_wrapper
{
	void *object;
	bool owned;
	t_set_current_fn set_current;
	t_hit_noobj_fn hit_noobj;
	t_aabb bbox;
} t_hittable_wrapper;

/* Hit record: store intersection point, normal, material and t. */
struct s_hit_record
{
	t_vec3 p;
	t_vec3 normal;
	real_t t;
	bool front_face;
	t_vec3 albedo; /* per-hit surface color */
	// in the future we need to compute (u,v) texture coordinates for a given point on each type of hittable
	real_t u;
	real_t v;
	t_material *mat; /* pointer to material that determines scattering behavior */
};

/* set_face_normal: outward_normal is assumed unit length. */
static inline void set_face_normal(t_hit_record *hit, const t_ray *r, const t_vec3 *outward_normal)
{
	bool front = (dot(&r->dir, outward_normal) < 0.0);
	hit->front_face = front;
	if (front)
		hit->normal = *outward_normal;
	else
		hit->normal = vec3_neg(outward_normal);
}

typedef struct s_translate_wrap
{
	t_hittable_wrapper child;
	t_vec3 offset;
	t_aabb bbox;
} t_translate_wrap;

typedef struct s_rotate_y_wrap
{
	t_hittable_wrapper child;
	real_t sin_theta;
	real_t cos_theta;
	t_aabb bbox;
} t_rotate_y_wrap;

static inline t_vec3 rotate_y_vec(const t_vec3 *v, real_t sin_t, real_t cos_t)
{
	return vec3_create(
		cos_t * v->x + sin_t * v->z,
		v->y,
		-sin_t * v->x + cos_t * v->z);
}

/* translate wrapper */
static __thread const t_translate_wrap *g_current_translate = NULL;
static inline void set_current_translate(const void *obj) { g_current_translate = (const t_translate_wrap *)obj; }
static inline bool translate_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_translate_wrap *tr = g_current_translate;
	if (!tr)
		return false;

	t_vec3 neg_off = vec3_neg(&tr->offset);
	t_ray moved = ray_create(vec3_add(&r->orig, &neg_off), r->dir, r->tm);

	if (!tr->child.set_current || !tr->child.hit_noobj)
		return false;

	tr->child.set_current(tr->child.object);
	if (!tr->child.hit_noobj(&moved, rayt, rec))
		return false;

	rec->p = vec3_add(&rec->p, &tr->offset);
	set_face_normal(rec, &moved, &rec->normal);
	return true;
}

static inline t_translate_wrap *translate_create(const t_hittable_wrapper *child, const t_vec3 *offset)
{
	if (!child || !offset)
		return NULL;
	t_translate_wrap *tr = (t_translate_wrap *)malloc(sizeof(t_translate_wrap));
	if (!tr)
		return NULL;
	tr->child = *child;
	tr->offset = *offset;
	tr->bbox = aabb_add_vec3(&child->bbox, offset);
	return tr;
}

/* rotate_y wrapper */
static __thread const t_rotate_y_wrap *g_current_rotate = NULL;
static inline void set_current_rotate(const void *obj) { g_current_rotate = (const t_rotate_y_wrap *)obj; }
static inline bool rotate_y_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_rotate_y_wrap *rot = g_current_rotate;
	if (!rot)
		return false;

	/* inverse rotate ray to object space */
	t_vec3 o = rotate_y_vec(&r->orig, -rot->sin_theta, rot->cos_theta);
	t_vec3 d = rotate_y_vec(&r->dir, -rot->sin_theta, rot->cos_theta);
	t_ray rotated_r = ray_create(o, d, r->tm);

	if (!rot->child.set_current || !rot->child.hit_noobj)
		return false;

	rot->child.set_current(rot->child.object);
	if (!rot->child.hit_noobj(&rotated_r, rayt, rec))
		return false;

	/* rotate hit point and normal back */
	rec->p = rotate_y_vec(&rec->p, rot->sin_theta, rot->cos_theta);
	rec->normal = rotate_y_vec(&rec->normal, rot->sin_theta, rot->cos_theta);
	set_face_normal(rec, r, &rec->normal);
	return true;
}

static inline t_rotate_y_wrap *rotate_y_create(const t_hittable_wrapper *child, real_t angle_deg)
{
	if (!child)
		return NULL;

	t_rotate_y_wrap *rot = (t_rotate_y_wrap *)malloc(sizeof(t_rotate_y_wrap));
	if (!rot)
		return NULL;

	real_t radians = degrees_to_radians(angle_deg);
	rot->sin_theta = (real_t)sin((double)radians);
	rot->cos_theta = (real_t)cos((double)radians);
	rot->child = *child;

	/* compute rotated bbox from child bbox corners */
	t_aabb cb = child->bbox;
	t_point3 minp = point3_create(INFINITY, INFINITY, INFINITY);
	t_point3 maxp = point3_create(-INFINITY, -INFINITY, -INFINITY);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				real_t x = i ? cb.x.max : cb.x.min;
				real_t y = j ? cb.y.max : cb.y.min;
				real_t z = k ? cb.z.max : cb.z.min;
				t_vec3 corner = vec3_create(x, y, z);
				t_vec3 rotated = rotate_y_vec(&corner, rot->sin_theta, rot->cos_theta);

				if (rotated.x < minp.x)
					minp.x = rotated.x;
				if (rotated.y < minp.y)
					minp.y = rotated.y;
				if (rotated.z < minp.z)
					minp.z = rotated.z;
				if (rotated.x > maxp.x)
					maxp.x = rotated.x;
				if (rotated.y > maxp.y)
					maxp.y = rotated.y;
				if (rotated.z > maxp.z)
					maxp.z = rotated.z;
			}
	rot->bbox = aabb_from_points(&minp, &maxp);
	return rot;
}

#endif