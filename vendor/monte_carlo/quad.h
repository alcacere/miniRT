/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:54:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 23:22:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUAD_H
#define QUAD_H

#include "common.h"

typedef struct s_quad
{
	t_point3 q;
	t_vec3 u;
	t_vec3 v;
	t_vec3 w;
	t_material *mat;
	t_aabb bbox;
	t_vec3 normal;
	real_t d;
} t_quad;

/* Compute bounding box from quad vertices: Q, Q+u, Q+v, Q+u+v
   Also compute plane normal = unit(cross(u, v)), w = cross(u,v), and d = dot(normal, Q) */
static inline void set_bounding_box(t_quad *quad)
{
	if (!quad)
		return;

	/* compute plane normal, w vector, and D */
	{
		t_vec3 n = cross(&quad->u, &quad->v);
		quad->w = n; /* store raw cross product for plane coordinate calculations */
		t_vec3 n_unit = unit_vector(&n);
		/* If u and v are colinear, unit_vector returns zero; handle gracefully */
		quad->normal = n_unit;
		quad->d = (real_t)dot(&quad->normal, &quad->q);
	}

	/* build four corner points */
	t_point3 p0 = quad->q;
	t_point3 p1 = point3_create(quad->q.x + quad->u.x, quad->q.y + quad->u.y, quad->q.z + quad->u.z);
	t_point3 p2 = point3_create(quad->q.x + quad->v.x, quad->q.y + quad->v.y, quad->q.z + quad->v.z);
	t_point3 p3 = point3_create(quad->q.x + quad->u.x + quad->v.x,
								quad->q.y + quad->u.y + quad->v.y,
								quad->q.z + quad->u.z + quad->v.z);

	/* compute min/max across the four points */
	real_t minx = fmin(fmin(p0.x, p1.x), fmin(p2.x, p3.x));
	real_t miny = fmin(fmin(p0.y, p1.y), fmin(p2.y, p3.y));
	real_t minz = fmin(fmin(p0.z, p1.z), fmin(p2.z, p3.z));

	real_t maxx = fmax(fmax(p0.x, p1.x), fmax(p2.x, p3.x));
	real_t maxy = fmax(fmax(p0.y, p1.y), fmax(p2.y, p3.y));
	real_t maxz = fmax(fmax(p0.z, p1.z), fmax(p2.z, p3.z));

	t_point3 low = point3_create(minx, miny, minz);
	t_point3 high = point3_create(maxx, maxy, maxz);

	quad->bbox = aabb_from_points(&low, &high);
}

/* Create a quad by value (no heap allocation). Caller may store/copy as needed. */
static inline t_quad quad_create(const t_point3 *Q, const t_vec3 *u, const t_vec3 *v, t_material *mat)
{
	t_quad quad;
	if (!Q || !u || !v)
	{
		/* zero initialize on invalid input */
		quad.q = point3_create(0.0, 0.0, 0.0);
		quad.u = vec3_zero();
		quad.v = vec3_zero();
		quad.w = vec3_zero();
		quad.mat = NULL;
		quad.bbox = aabb_empty();
		quad.normal = vec3_zero();
		quad.d = (real_t)0.0;
		return quad;
	}

	quad.q = *Q;
	quad.u = *u;
	quad.v = *v;
	quad.mat = mat;

	set_bounding_box(&quad);
	return quad;
}

/* Accessor for bounding box */
static inline t_aabb quad_bounding_box(const t_quad *quad)
{
	if (!quad)
		return aabb_empty();
	return quad->bbox;
}

/* Check if plane coordinates (alpha, beta) are interior to the quad and set rec UV */
static inline bool quad_is_interior(real_t alpha, real_t beta, t_hit_record *rec)
{
	/* Check if both coordinates are in [0, 1] */
	if (!contains((real_t)0.0, (real_t)1.0, alpha) || !contains((real_t)0.0, (real_t)1.0, beta))
		return false;

	/* Set UV coordinates in hit record */
	rec->u = alpha;
	rec->v = beta;
	return true;
}

/* Hit test: plane intersection + plane-coordinate check */
static inline bool quad_hit(const t_quad *quad, const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	/* basic validation */
	if (!quad || !r || !rec)
		return false;

	/* Ray-plane intersection: denom = dot(n, dir) */
	real_t denom = (real_t)dot(&quad->normal, &r->dir);
	const real_t EPS = (real_t)1e-8;
	if (fabsl((long double)denom) < (long double)EPS)
		return false; /* parallel */

	/* t = (d - dot(n, orig)) / denom */
	real_t numer = (real_t)dot(&quad->normal, &r->orig);
	real_t t = (quad->d - numer) / denom;

	/* check interval */
	if (!contains(rayt.min, rayt.max, t))
		return false;

	/* compute intersection point */
	t_vec3 p = ray_at((t_ray *)r, t);

	/* Compute plane coordinates using cross products and dot products
	   alpha = dot(w, cross(planar_hitpt_vector, v)) / |w|^2
	   beta  = dot(w, cross(u, planar_hitpt_vector)) / |w|^2 */
	t_vec3 planar_hitpt_vector = vec3_sub(&p, &quad->q);
	real_t w_len_sq = vec3_length_squared(&quad->w);
	if (w_len_sq <= (real_t)0.0)
		return false; /* degenerate quad */

	t_vec3 cross_phv_v = cross(&planar_hitpt_vector, &quad->v);
	real_t alpha = (real_t)dot(&quad->w, &cross_phv_v) / w_len_sq;

	t_vec3 cross_u_phv = cross(&quad->u, &planar_hitpt_vector);
	real_t beta = (real_t)dot(&quad->w, &cross_u_phv) / w_len_sq;

	/* Check if point is interior and set UV; reject if exterior */
	if (!quad_is_interior(alpha, beta, rec))
		return false;

	/* Ray hits the 2D shape; set the rest of the hit record and return true */
	rec->t = t;
	rec->p = p;
	rec->mat = quad->mat;
	set_face_normal(rec, r, &quad->normal);

	return true;
}

/* Bind current quad for hit_noobj indirection (mirrors sphere pattern) */
static __thread const t_quad *g_current_quad = NULL;
static inline void set_current_quad(const void *obj)
{
	g_current_quad = (const t_quad *)obj;
}

/* hit_noobj wrapper used by hittable_list */
static inline bool quad_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_quad)
		return false;
	return quad_hit(g_current_quad, r, rayt, rec);
}

/* Create a 3D axis-aligned box from two opposite corner points.
   Returns a hittable_list containing the six quad faces of the box. */
static inline void box(t_hittable_list *world, const t_point3 *a, const t_point3 *b, t_material *mat)
{
	if (!world || !a || !b || !mat)
		return;

	/* Construct the two opposite vertices with min and max coordinates */
	real_t minx = (a->x < b->x) ? a->x : b->x;
	real_t miny = (a->y < b->y) ? a->y : b->y;
	real_t minz = (a->z < b->z) ? a->z : b->z;

	real_t maxx = (a->x > b->x) ? a->x : b->x;
	real_t maxy = (a->y > b->y) ? a->y : b->y;
	real_t maxz = (a->z > b->z) ? a->z : b->z;

	t_point3 min = point3_create(minx, miny, minz);
	t_point3 max = point3_create(maxx, maxy, maxz);

	/* Edge vectors for the box dimensions */
	t_vec3 dx = vec3_create(max.x - min.x, 0.0, 0.0);
	t_vec3 dy = vec3_create(0.0, max.y - min.y, 0.0);
	t_vec3 dz = vec3_create(0.0, 0.0, max.z - min.z);

	/* Front face: at max z */
	t_point3 pos_front = point3_create(min.x, min.y, max.z);
	t_quad quad_front = quad_create(&pos_front, &dx, &dy, mat);
	t_quad *quad_front_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_front_copy)
	{
		*quad_front_copy = quad_front;
		hittable_list_add_nonowned(world, quad_front_copy, set_current_quad,
								   quad_hit_noobj, &quad_front.bbox);
	}

	/* Right face: at max x */
	t_point3 pos_right = point3_create(max.x, min.y, max.z);
	t_vec3 neg_dz = vec3_neg(&dz);
	t_quad quad_right = quad_create(&pos_right, &neg_dz, &dy, mat);
	t_quad *quad_right_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_right_copy)
	{
		*quad_right_copy = quad_right;
		hittable_list_add_nonowned(world, quad_right_copy, set_current_quad,
								   quad_hit_noobj, &quad_right.bbox);
	}

	/* Back face: at min z */
	t_point3 pos_back = point3_create(max.x, min.y, min.z);
	t_vec3 neg_dx = vec3_neg(&dx);
	t_quad quad_back = quad_create(&pos_back, &neg_dx, &dy, mat);
	t_quad *quad_back_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_back_copy)
	{
		*quad_back_copy = quad_back;
		hittable_list_add_nonowned(world, quad_back_copy, set_current_quad,
								   quad_hit_noobj, &quad_back.bbox);
	}

	/* Left face: at min x */
	t_point3 pos_left = point3_create(min.x, min.y, min.z);
	t_quad quad_left = quad_create(&pos_left, &dz, &dy, mat);
	t_quad *quad_left_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_left_copy)
	{
		*quad_left_copy = quad_left;
		hittable_list_add_nonowned(world, quad_left_copy, set_current_quad,
								   quad_hit_noobj, &quad_left.bbox);
	}

	/* Top face: at max y */
	t_point3 pos_top = point3_create(min.x, max.y, max.z);
	t_vec3 neg_dz_top = vec3_neg(&dz);
	t_quad quad_top = quad_create(&pos_top, &dx, &neg_dz_top, mat);
	t_quad *quad_top_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_top_copy)
	{
		*quad_top_copy = quad_top;
		hittable_list_add_nonowned(world, quad_top_copy, set_current_quad,
								   quad_hit_noobj, &quad_top.bbox);
	}

	/* Bottom face: at min y */
	t_point3 pos_bottom = point3_create(min.x, min.y, min.z);
	t_quad quad_bottom = quad_create(&pos_bottom, &dx, &dz, mat);
	t_quad *quad_bottom_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_bottom_copy)
	{
		*quad_bottom_copy = quad_bottom;
		hittable_list_add_nonowned(world, quad_bottom_copy, set_current_quad,
								   quad_hit_noobj, &quad_bottom.bbox);
	}
}

static inline void box_create_list(const t_point3 *a, const t_point3 *b, t_material *mat, t_hittable_list *out)
{
	if (!out || !a || !b || !mat)
		return;
	hittable_list_init(out);

	real_t minx = (a->x < b->x) ? a->x : b->x;
	real_t miny = (a->y < b->y) ? a->y : b->y;
	real_t minz = (a->z < b->z) ? a->z : b->z;
	real_t maxx = (a->x > b->x) ? a->x : b->x;
	real_t maxy = (a->y > b->y) ? a->y : b->y;
	real_t maxz = (a->z > b->z) ? a->z : b->z;

	t_point3 min = point3_create(minx, miny, minz);
	t_point3 max = point3_create(maxx, maxy, maxz);

	t_vec3 dx = vec3_create(max.x - min.x, 0.0, 0.0);
	t_vec3 dy = vec3_create(0.0, max.y - min.y, 0.0);
	t_vec3 dz = vec3_create(0.0, 0.0, max.z - min.z);

	t_point3 p;
	t_quad q;
	t_quad *cpy;

	/* front */
	p = point3_create(min.x, min.y, max.z);
	q = quad_create(&p, &dx, &dy, mat);
	cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		hittable_list_add_nonowned(out, cpy, set_current_quad, quad_hit_noobj, &q.bbox);
	}

	/* right */
	p = point3_create(max.x, min.y, max.z);
	t_vec3 ndz = vec3_neg(&dz);
	q = quad_create(&p, &ndz, &dy, mat);
	cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		hittable_list_add_nonowned(out, cpy, set_current_quad, quad_hit_noobj, &q.bbox);
	}

	/* back */
	p = point3_create(max.x, min.y, min.z);
	t_vec3 ndx = vec3_neg(&dx);
	q = quad_create(&p, &ndx, &dy, mat);
	cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		hittable_list_add_nonowned(out, cpy, set_current_quad, quad_hit_noobj, &q.bbox);
	}

	/* left */
	p = point3_create(min.x, min.y, min.z);
	q = quad_create(&p, &dz, &dy, mat);
	cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		hittable_list_add_nonowned(out, cpy, set_current_quad, quad_hit_noobj, &q.bbox);
	}

	/* top */
	p = point3_create(min.x, max.y, max.z);
	t_vec3 ndz_top = vec3_neg(&dz);
	q = quad_create(&p, &dx, &ndz_top, mat);
	cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		hittable_list_add_nonowned(out, cpy, set_current_quad, quad_hit_noobj, &q.bbox);
	}

	/* bottom */
	p = point3_create(min.x, min.y, min.z);
	q = quad_create(&p, &dx, &dz, mat);
	cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		hittable_list_add_nonowned(out, cpy, set_current_quad, quad_hit_noobj, &q.bbox);
	}
}

#endif