/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       constant_medium.h                                               */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 23:22:42                                             */
/*  Updated:    2026/01/04 23:22:42                                             */
/*                                                                              */
/* ============================================================================ */

#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"
#include "ray.h"
#include "interval.h"
#include "random.h"
#include <stdlib.h>
#include <math.h>

/* Constant medium (participating media) structure */
typedef struct s_constant_medium
{
	t_hittable_wrapper boundary; /* stored by value (copied), not owned */
	real_t neg_inv_density;		 /* -1/density */
	t_material *phase_function;	 /* isotropic scattering material (owned) */
	t_aabb bbox;				 /* cached bounding box */
} t_constant_medium;

/* Set current medium for hit_noobj callback */
static __thread const t_constant_medium *g_current_medium = NULL;
static inline void set_current_medium(const void *obj)
{
	g_current_medium = (const t_constant_medium *)obj;
}

/* Hit function for constant medium */
static inline bool constant_medium_hit(const t_constant_medium *medium, const t_ray *r,
									   t_interval rayt, t_hit_record *rec)
{
	if (!medium || !rec)
		return false;

	/* Get first intersection with boundary */
	t_hit_record rec1;
	if (!medium->boundary.set_current || !medium->boundary.hit_noobj)
		return false;
	medium->boundary.set_current(medium->boundary.object);
	if (!medium->boundary.hit_noobj(r, interval(-INFINITY, INFINITY), &rec1))
		return false;

	/* Get second intersection with boundary (exit point) */
	t_hit_record rec2;
	medium->boundary.set_current(medium->boundary.object);
	if (!medium->boundary.hit_noobj(r, interval(rec1.t + (real_t)0.0001, INFINITY), &rec2))
		return false;

	/* Clamp intersection points to requested ray interval */
	if (rec1.t < rayt.min)
		rec1.t = rayt.min;
	if (rec2.t > rayt.max)
		rec2.t = rayt.max;

	/* No valid path through medium */
	if (rec1.t >= rec2.t)
		return false;

	/* Clamp entry to ray start */
	if (rec1.t < (real_t)0.0)
		rec1.t = (real_t)0.0;

	/* Compute ray length and distance inside boundary */
	real_t ray_length = vec3_length(&r->dir);
	real_t distance_inside_boundary = (rec2.t - rec1.t) * ray_length;

	/* Sample random hit distance (exponential distribution) */
	real_t random_val = random_double();
	if (random_val <= (real_t)0.0)
		random_val = (real_t)1e-10; /* avoid log(0) */
	real_t hit_distance = medium->neg_inv_density * (real_t)log((double)random_val);

	/* Check if scattering happens inside the medium */
	if (hit_distance > distance_inside_boundary)
		return false;

	/* Compute hit point inside medium */
	rec->t = rec1.t + hit_distance / ray_length;
	rec->p = ray_at((t_ray *)r, rec->t);

	/* Arbitrary normal and material */
	rec->normal = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	rec->front_face = true;
	rec->mat = medium->phase_function;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	rec->u = (real_t)0.0;
	rec->v = (real_t)0.0;

	return true;
}

/* Wrapper for hit_noobj callback */
static inline bool constant_medium_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_medium)
		return false;
	return constant_medium_hit(g_current_medium, r, rayt, rec);
}

/* Constructor: constant medium with texture */
static inline t_constant_medium *constant_medium_create_texture(
	const t_hittable_wrapper *boundary, real_t density, t_texture *tex)
{
	if (!boundary || density <= (real_t)0.0 || !tex)
		return NULL;
	t_constant_medium *medium = (t_constant_medium *)malloc(sizeof(t_constant_medium));
	if (!medium)
		return NULL;
	medium->boundary = *boundary; /* copy wrapper by value */
	medium->neg_inv_density = (real_t)-1.0 / density;
	medium->phase_function = isotropic_create_texture(tex);
	if (!medium->phase_function)
	{
		free(medium);
		return NULL;
	}
	medium->bbox = boundary->bbox;
	return medium;
}

/* Constructor: constant medium with color albedo */
static inline t_constant_medium *constant_medium_create_color(
	const t_hittable_wrapper *boundary, real_t density, t_color albedo)
{
	if (!boundary || density <= (real_t)0.0)
		return NULL;
	t_constant_medium *medium = (t_constant_medium *)malloc(sizeof(t_constant_medium));
	if (!medium)
		return NULL;
	medium->boundary = *boundary; /* copy wrapper by value */
	medium->neg_inv_density = (real_t)-1.0 / density;
	medium->phase_function = isotropic_create(albedo);
	if (!medium->phase_function)
	{
		free(medium);
		return NULL;
	}
	medium->bbox = boundary->bbox;
	return medium;
}

/* Get bounding box */
static inline t_aabb constant_medium_bounding_box(const t_constant_medium *medium)
{
	if (!medium)
		return aabb_empty();
	return medium->bbox;
}

/* Destructor: cleanup owned material (boundary is not owned) */
static inline void constant_medium_destroy(t_constant_medium *medium)
{
	if (!medium)
		return;

	if (medium->phase_function)
	{
		medium->phase_function->destroy(medium->phase_function);
		free(medium->phase_function);
		medium->phase_function = NULL;
	}

	free(medium);
}

#endif
