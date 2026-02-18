/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       cylinder.h                                                      */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/05                                                      */
/*                                                                              */
/* ============================================================================ */

#ifndef CYLINDER_H
#define CYLINDER_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "point.h"
#include "hittable.h"
#include "interval.h"
#include "aabb.h"
#include <math.h>
#include <stdbool.h>

/* Forward declaration */
typedef struct s_material t_material;

/* Finite cylinder: center at base, axis direction, radius, height */
typedef struct s_cylinder
{
	t_point3 base; /* center of bottom cap */
	t_vec3 axis;   /* unit direction from base to top */
	real_t radius;
	real_t height;
	t_material *mat;
	t_aabb bbox;
} t_cylinder;

/* Create cylinder */
static inline t_cylinder cylinder_create(const t_point3 *base, const t_vec3 *axis,
										 real_t radius, real_t height, t_material *mat)
{
	t_cylinder cyl;
	cyl.base = *base;
	cyl.axis = unit_vector(axis);
	cyl.radius = (radius > 0) ? radius : (real_t)0.1;
	cyl.height = (height > 0) ? height : (real_t)1.0;
	cyl.mat = mat;

	/* Compute bounding box by finding extrema of cylinder */
	/* Top center */
	t_vec3 scaled_axis = vec3_mul_scalar(&cyl.axis, cyl.height);
	t_point3 top = vec3_add(base, &scaled_axis);

	/* For an arbitrary axis, we need to find the extent in each direction */
	/* Compute perpendicular radius contribution to each axis */
	real_t ax = cyl.axis.x;
	real_t ay = cyl.axis.y;
	real_t az = cyl.axis.z;

	/* Maximum extent perpendicular to axis in x, y, z directions */
	real_t ex = cyl.radius * (real_t)sqrt(1.0 - (double)(ax * ax));
	real_t ey = cyl.radius * (real_t)sqrt(1.0 - (double)(ay * ay));
	real_t ez = cyl.radius * (real_t)sqrt(1.0 - (double)(az * az));

	real_t minx = fmin(base->x, top.x) - ex;
	real_t miny = fmin(base->y, top.y) - ey;
	real_t minz = fmin(base->z, top.z) - ez;
	real_t maxx = fmax(base->x, top.x) + ex;
	real_t maxy = fmax(base->y, top.y) + ey;
	real_t maxz = fmax(base->z, top.z) + ez;

	t_point3 low = point3_create(minx, miny, minz);
	t_point3 high = point3_create(maxx, maxy, maxz);
	cyl.bbox = aabb_from_points(&low, &high);

	return cyl;
}

/* Create Y-axis aligned cylinder (common case) */
static inline t_cylinder cylinder_create_y(const t_point3 *base, real_t radius,
										   real_t height, t_material *mat)
{
	t_vec3 y_axis = vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0);
	return cylinder_create(base, &y_axis, radius, height, mat);
}

/* Helper: get UV for cylinder side (theta around axis, v along height) */
static inline void cylinder_get_uv(const t_cylinder *cyl, const t_vec3 *p,
								   real_t *u, real_t *v)
{
	/* Project point onto base plane */
	t_vec3 to_p = vec3_sub(p, &cyl->base);
	real_t height_proj = dot(&to_p, &cyl->axis);

	/* v = height along cylinder [0, 1] */
	*v = height_proj / cyl->height;

	/* Get radial component */
	t_vec3 axis_comp = vec3_mul_scalar(&cyl->axis, height_proj);
	t_vec3 radial = vec3_sub(&to_p, &axis_comp);

	/* Build local coordinate system perpendicular to axis */
	t_vec3 ref;
	if (fabsl((long double)cyl->axis.y) > (long double)0.9)
		ref = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	else
		ref = vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0);

	t_vec3 u_axis = cross(&cyl->axis, &ref);
	u_axis = unit_vector(&u_axis);
	t_vec3 v_axis = cross(&cyl->axis, &u_axis);

	/* Compute angle */
	real_t x_proj = dot(&radial, &u_axis);
	real_t z_proj = dot(&radial, &v_axis);
	real_t theta = (real_t)atan2((double)z_proj, (double)x_proj);

	/* u = angle around cylinder [0, 1] */
	*u = (theta + (real_t)PI) / ((real_t)2.0 * (real_t)PI);
}

/* Ray-cylinder intersection */
static inline bool cylinder_hit(const t_cylinder *cyl, const t_ray *r,
								t_interval rayt, t_hit_record *rec)
{
	if (!cyl || !r || !rec)
		return false;

	const real_t EPSILON = (real_t)1e-8;

	/* Transform ray to cylinder's local space */
	t_vec3 oc = vec3_sub(&r->orig, &cyl->base);

	/* Components of ray perpendicular to axis */
	real_t d_dot_axis = dot(&r->dir, &cyl->axis);
	real_t oc_dot_axis = dot(&oc, &cyl->axis);

	/* Store intermediate results before taking address */
	t_vec3 d_axis_comp = vec3_mul_scalar(&cyl->axis, d_dot_axis);
	t_vec3 oc_axis_comp = vec3_mul_scalar(&cyl->axis, oc_dot_axis);

	t_vec3 d_perp = vec3_sub(&r->dir, &d_axis_comp);
	t_vec3 oc_perp = vec3_sub(&oc, &oc_axis_comp);

	/* Quadratic coefficients for side surface */
	real_t a = vec3_length_squared(&d_perp);
	real_t half_b = dot(&oc_perp, &d_perp);
	real_t c = vec3_length_squared(&oc_perp) - cyl->radius * cyl->radius;

	bool hit_anything = false;
	real_t closest_t = rayt.max;
	t_vec3 closest_normal;
	real_t closest_u = 0, closest_v = 0;
	bool is_cap_hit = false;

	/* Check side surface */
	if (a > EPSILON)
	{
		real_t discriminant = half_b * half_b - a * c;
		if (discriminant >= 0)
		{
			real_t sqrtd = (real_t)sqrt((double)discriminant);

			/* Try both roots */
			real_t roots[2] = {(-half_b - sqrtd) / a, (-half_b + sqrtd) / a};
			for (int i = 0; i < 2; ++i)
			{
				real_t t = roots[i];
				if (t < rayt.min || t >= closest_t)
					continue;

				/* Check if hit point is within cylinder height */
				t_vec3 hit_pt = ray_at((t_ray *)r, t);
				t_vec3 to_hit = vec3_sub(&hit_pt, &cyl->base);
				real_t h = dot(&to_hit, &cyl->axis);

				if (h >= 0 && h <= cyl->height)
				{
					hit_anything = true;
					closest_t = t;
					is_cap_hit = false;

					/* Outward normal: radial direction */
					t_vec3 axis_comp = vec3_mul_scalar(&cyl->axis, h);
					t_vec3 center_at_h = vec3_add(&cyl->base, &axis_comp);
					t_vec3 outward = vec3_sub(&hit_pt, &center_at_h);
					closest_normal = unit_vector(&outward);

					cylinder_get_uv(cyl, &hit_pt, &closest_u, &closest_v);
				}
			}
		}
	}

	/* Check bottom cap (at base) */
	if (fabsl((long double)d_dot_axis) > (long double)EPSILON)
	{
		real_t t = -oc_dot_axis / d_dot_axis;
		if (t >= rayt.min && t < closest_t)
		{
			t_vec3 hit_pt = ray_at((t_ray *)r, t);
			t_vec3 to_hit = vec3_sub(&hit_pt, &cyl->base);
			/* Compute perpendicular distance */
			t_vec3 axis_proj = vec3_mul_scalar(&cyl->axis, dot(&to_hit, &cyl->axis));
			t_vec3 perp = vec3_sub(&to_hit, &axis_proj);
			real_t dist_sq = vec3_length_squared(&perp);

			if (dist_sq <= cyl->radius * cyl->radius)
			{
				hit_anything = true;
				closest_t = t;
				is_cap_hit = true;
				closest_normal = vec3_neg(&cyl->axis);
				closest_u = (perp.x / cyl->radius + (real_t)1.0) * (real_t)0.5;
				closest_v = (perp.z / cyl->radius + (real_t)1.0) * (real_t)0.5;
			}
		}

		/* Check top cap */
		t = (cyl->height - oc_dot_axis) / d_dot_axis;
		if (t >= rayt.min && t < closest_t)
		{
			t_vec3 hit_pt = ray_at((t_ray *)r, t);
			t_vec3 top_axis_comp = vec3_mul_scalar(&cyl->axis, cyl->height);
			t_vec3 top_center = vec3_add(&cyl->base, &top_axis_comp);
			t_vec3 to_hit = vec3_sub(&hit_pt, &top_center);
			real_t dist_sq = vec3_length_squared(&to_hit);

			if (dist_sq <= cyl->radius * cyl->radius)
			{
				hit_anything = true;
				closest_t = t;
				is_cap_hit = true;
				closest_normal = cyl->axis;
				closest_u = (to_hit.x / cyl->radius + (real_t)1.0) * (real_t)0.5;
				closest_v = (to_hit.z / cyl->radius + (real_t)1.0) * (real_t)0.5;
			}
		}
	}

	if (!hit_anything)
		return false;

	rec->t = closest_t;
	rec->p = ray_at((t_ray *)r, closest_t);
	rec->u = closest_u;
	rec->v = closest_v;
	rec->mat = cyl->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, r, &closest_normal);

	return true;
}

/* Thread-local current cylinder for hit_noobj */
static __thread const t_cylinder *g_current_cylinder = NULL;

static inline void set_current_cylinder(const void *obj)
{
	g_current_cylinder = (const t_cylinder *)obj;
}

static inline bool cylinder_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_cylinder)
		return false;
	return cylinder_hit(g_current_cylinder, r, rayt, rec);
}

/* Add cylinder to hittable list */
static inline bool hittable_list_add_cylinder(t_hittable_list *list, const t_cylinder *cyl)
{
	if (!list || !cyl)
		return false;

	t_cylinder *copy = (t_cylinder *)malloc(sizeof(t_cylinder));
	if (!copy)
		return false;

	*copy = *cyl;
	t_hittable_wrapper wrap = {
		.object = copy,
		.owned = true,
		.set_current = set_current_cylinder,
		.hit_noobj = cylinder_hit_noobj,
		.bbox = cyl->bbox};
	return hittable_list_add_wrapper(list, &wrap);
}

/* ============================================================================ */
/*                          CONE (bonus primitive)                              */
/* ============================================================================ */

typedef struct s_cone
{
	t_point3 apex; /* tip of cone */
	t_vec3 axis;   /* unit direction from apex to base */
	real_t angle;  /* half-angle in radians */
	real_t height; /* distance from apex to base */
	t_material *mat;
	t_aabb bbox;
} t_cone;

static inline t_cone cone_create(const t_point3 *apex, const t_vec3 *axis,
								 real_t angle_deg, real_t height, t_material *mat)
{
	t_cone cone;
	cone.apex = *apex;
	cone.axis = unit_vector(axis);
	cone.angle = degrees_to_radians(angle_deg);
	cone.height = (height > 0) ? height : (real_t)1.0;
	cone.mat = mat;

	/* Compute bounding box */
	real_t base_radius = cone.height * (real_t)tan((double)cone.angle);
	t_vec3 to_base = vec3_mul_scalar(&cone.axis, cone.height);
	t_point3 base_center = vec3_add(apex, &to_base);

	/* Similar to cylinder bbox computation */
	real_t ax = cone.axis.x;
	real_t ay = cone.axis.y;
	real_t az = cone.axis.z;
	real_t ex = base_radius * (real_t)sqrt(1.0 - (double)(ax * ax));
	real_t ey = base_radius * (real_t)sqrt(1.0 - (double)(ay * ay));
	real_t ez = base_radius * (real_t)sqrt(1.0 - (double)(az * az));

	real_t minx = fmin(apex->x, base_center.x - ex);
	real_t miny = fmin(apex->y, base_center.y - ey);
	real_t minz = fmin(apex->z, base_center.z - ez);
	real_t maxx = fmax(apex->x, base_center.x + ex);
	real_t maxy = fmax(apex->y, base_center.y + ey);
	real_t maxz = fmax(apex->z, base_center.z + ez);

	t_point3 low = point3_create(minx, miny, minz);
	t_point3 high = point3_create(maxx, maxy, maxz);
	cone.bbox = aabb_from_points(&low, &high);

	return cone;
}

/* Ray-cone intersection (truncated cone with base cap) */
static inline bool cone_hit(const t_cone *cone, const t_ray *r,
							t_interval rayt, t_hit_record *rec)
{
	if (!cone || !r || !rec)
		return false;

	const real_t EPSILON = (real_t)1e-8;

	t_vec3 co = vec3_sub(&r->orig, &cone->apex);

	real_t cos_a = (real_t)cos((double)cone->angle);
	real_t sin_a = (real_t)sin((double)cone->angle);
	real_t cos2 = cos_a * cos_a;
	real_t sin2 = sin_a * sin_a;

	real_t d_dot_v = dot(&r->dir, &cone->axis);
	real_t co_dot_v = dot(&co, &cone->axis);

	/* Quadratic coefficients */
	real_t tan2 = sin2 / cos2;

	real_t a = dot(&r->dir, &r->dir) - (1 + tan2) * d_dot_v * d_dot_v;
	real_t half_b = dot(&r->dir, &co) - (1 + tan2) * d_dot_v * co_dot_v;
	real_t c_coef = dot(&co, &co) - (1 + tan2) * co_dot_v * co_dot_v;

	bool hit_anything = false;
	real_t closest_t = rayt.max;
	t_vec3 closest_normal;

	if (fabsl((long double)a) > (long double)EPSILON)
	{
		real_t discriminant = half_b * half_b - a * c_coef;
		if (discriminant >= 0)
		{
			real_t sqrtd = (real_t)sqrt((double)discriminant);
			real_t roots[2] = {(-half_b - sqrtd) / a, (-half_b + sqrtd) / a};

			for (int i = 0; i < 2; ++i)
			{
				real_t t = roots[i];
				if (t < rayt.min || t >= closest_t)
					continue;

				t_vec3 hit_pt = ray_at((t_ray *)r, t);
				t_vec3 to_hit = vec3_sub(&hit_pt, &cone->apex);
				real_t h = dot(&to_hit, &cone->axis);

				/* Must be between apex and base */
				if (h >= 0 && h <= cone->height)
				{
					hit_anything = true;
					closest_t = t;

					/* Normal computation for cone surface */
					t_vec3 axis_pt = vec3_mul_scalar(&cone->axis, h);
					t_vec3 radial = vec3_sub(&to_hit, &axis_pt);
					t_vec3 radial_unit = unit_vector(&radial);

					/* Normal = radial * cos(angle) - axis * sin(angle) */
					t_vec3 n1 = vec3_mul_scalar(&radial_unit, cos_a);
					t_vec3 n2 = vec3_mul_scalar(&cone->axis, -sin_a);
					closest_normal = vec3_add(&n1, &n2);
					closest_normal = unit_vector(&closest_normal);
				}
			}
		}
	}

	/* Check base cap */
	if (fabsl((long double)d_dot_v) > (long double)EPSILON)
	{
		real_t t = (cone->height - co_dot_v) / d_dot_v;
		if (t >= rayt.min && t < closest_t)
		{
			t_vec3 hit_pt = ray_at((t_ray *)r, t);
			t_vec3 base_axis_comp = vec3_mul_scalar(&cone->axis, cone->height);
			t_vec3 base_center = vec3_add(&cone->apex, &base_axis_comp);
			t_vec3 to_hit = vec3_sub(&hit_pt, &base_center);
			real_t dist_sq = vec3_length_squared(&to_hit);
			real_t base_radius = cone->height * (real_t)tan((double)cone->angle);

			if (dist_sq <= base_radius * base_radius)
			{
				hit_anything = true;
				closest_t = t;
				closest_normal = cone->axis;
			}
		}
	}

	if (!hit_anything)
		return false;

	rec->t = closest_t;
	rec->p = ray_at((t_ray *)r, closest_t);
	rec->u = 0;
	rec->v = 0;
	rec->mat = cone->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, r, &closest_normal);

	return true;
}

static __thread const t_cone *g_current_cone = NULL;

static inline void set_current_cone(const void *obj)
{
	g_current_cone = (const t_cone *)obj;
}

static inline bool cone_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_cone)
		return false;
	return cone_hit(g_current_cone, r, rayt, rec);
}

static inline bool hittable_list_add_cone(t_hittable_list *list, const t_cone *cone)
{
	if (!list || !cone)
		return false;

	t_cone *copy = (t_cone *)malloc(sizeof(t_cone));
	if (!copy)
		return false;

	*copy = *cone;
	t_hittable_wrapper wrap = {
		.object = copy,
		.owned = true,
		.set_current = set_current_cone,
		.hit_noobj = cone_hit_noobj,
		.bbox = cone->bbox};
	return hittable_list_add_wrapper(list, &wrap);
}

#endif
