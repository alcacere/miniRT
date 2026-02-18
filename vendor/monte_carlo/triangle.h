/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       triangle.h                                                      */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/05                                                      */
/*                                                                              */
/* ============================================================================ */

#ifndef TRIANGLE_H
#define TRIANGLE_H

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

/* Triangle: three vertices, precomputed edges and normal */
typedef struct s_triangle
{
	t_point3 v0;   /* first vertex */
	t_point3 v1;   /* second vertex */
	t_point3 v2;   /* third vertex */
	t_vec3 e1;	   /* edge v1 - v0 */
	t_vec3 e2;	   /* edge v2 - v0 */
	t_vec3 normal; /* unit normal */
	t_material *mat;
	t_aabb bbox;
} t_triangle;

/* Create triangle from three vertices */
static inline t_triangle triangle_create(const t_point3 *v0, const t_point3 *v1,
										 const t_point3 *v2, t_material *mat)
{
	t_triangle tri;
	tri.v0 = *v0;
	tri.v1 = *v1;
	tri.v2 = *v2;
	tri.mat = mat;

	/* Precompute edges */
	tri.e1 = vec3_sub(v1, v0);
	tri.e2 = vec3_sub(v2, v0);

	/* Compute normal: cross(e1, e2) normalized */
	t_vec3 n = cross(&tri.e1, &tri.e2);
	tri.normal = unit_vector(&n);

	/* Compute bounding box */
	real_t minx = fmin(fmin(v0->x, v1->x), v2->x);
	real_t miny = fmin(fmin(v0->y, v1->y), v2->y);
	real_t minz = fmin(fmin(v0->z, v1->z), v2->z);
	real_t maxx = fmax(fmax(v0->x, v1->x), v2->x);
	real_t maxy = fmax(fmax(v0->y, v1->y), v2->y);
	real_t maxz = fmax(fmax(v0->z, v1->z), v2->z);

	/* Pad thin dimensions to avoid degenerate boxes */
	const real_t delta = (real_t)0.0001;
	if (maxx - minx < delta)
	{
		minx -= delta;
		maxx += delta;
	}
	if (maxy - miny < delta)
	{
		miny -= delta;
		maxy += delta;
	}
	if (maxz - minz < delta)
	{
		minz -= delta;
		maxz += delta;
	}

	t_point3 low = point3_create(minx, miny, minz);
	t_point3 high = point3_create(maxx, maxy, maxz);
	tri.bbox = aabb_from_points(&low, &high);

	return tri;
}

/* Möller-Trumbore ray-triangle intersection algorithm */
static inline bool triangle_hit(const t_triangle *tri, const t_ray *r,
								t_interval rayt, t_hit_record *rec)
{
	if (!tri || !r || !rec)
		return false;

	const real_t EPSILON = (real_t)1e-8;

	/* h = cross(ray.dir, e2) */
	t_vec3 h = cross(&r->dir, &tri->e2);

	/* a = dot(e1, h) */
	real_t a = dot(&tri->e1, &h);

	/* Ray is parallel to triangle */
	if (fabsl((long double)a) < (long double)EPSILON)
		return false;

	real_t f = (real_t)1.0 / a;

	/* s = ray.origin - v0 */
	t_vec3 s = vec3_sub(&r->orig, &tri->v0);

	/* u = f * dot(s, h) */
	real_t u = f * dot(&s, &h);

	/* Check u bounds [0, 1] */
	if (u < (real_t)0.0 || u > (real_t)1.0)
		return false;

	/* q = cross(s, e1) */
	t_vec3 q = cross(&s, &tri->e1);

	/* v = f * dot(ray.dir, q) */
	real_t v = f * dot(&r->dir, &q);

	/* Check v bounds and u+v <= 1 */
	if (v < (real_t)0.0 || u + v > (real_t)1.0)
		return false;

	/* t = f * dot(e2, q) */
	real_t t = f * dot(&tri->e2, &q);

	/* Check t is in valid range */
	if (!contains(rayt.min, rayt.max, t))
		return false;

	/* Hit! Fill record */
	rec->t = t;
	rec->p = ray_at((t_ray *)r, t);
	rec->u = u;
	rec->v = v;
	rec->mat = tri->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, r, &tri->normal);

	return true;
}

/* Thread-local current triangle for hit_noobj */
static __thread const t_triangle *g_current_triangle = NULL;

static inline void set_current_triangle(const void *obj)
{
	g_current_triangle = (const t_triangle *)obj;
}

static inline bool triangle_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_triangle)
		return false;
	return triangle_hit(g_current_triangle, r, rayt, rec);
}

/* Add triangle to hittable list (copies the triangle) */
static inline bool hittable_list_add_triangle(t_hittable_list *list, const t_triangle *tri)
{
	if (!list || !tri)
		return false;

	t_triangle *copy = (t_triangle *)malloc(sizeof(t_triangle));
	if (!copy)
		return false;

	*copy = *tri;
	t_hittable_wrapper wrap = {
		.object = copy,
		.owned = true,
		.set_current = set_current_triangle,
		.hit_noobj = triangle_hit_noobj,
		.bbox = tri->bbox};
	return hittable_list_add_wrapper(list, &wrap);
}

/* ============================================================================ */
/*                          SIMPLE MESH LOADING                                 */
/* ============================================================================ */

/* Triangle mesh: collection of triangles sharing material */
typedef struct s_mesh
{
	t_triangle *triangles;
	size_t count;
	size_t capacity;
	t_aabb bbox;
} t_mesh;

static inline void mesh_init(t_mesh *mesh)
{
	if (!mesh)
		return;
	mesh->triangles = NULL;
	mesh->count = 0;
	mesh->capacity = 0;
	mesh->bbox = aabb_empty();
}

static inline bool mesh_add_triangle(t_mesh *mesh, const t_triangle *tri)
{
	if (!mesh || !tri)
		return false;

	if (mesh->count >= mesh->capacity)
	{
		size_t newcap = (mesh->capacity == 0) ? 16 : mesh->capacity * 2;
		t_triangle *newarr = (t_triangle *)realloc(mesh->triangles, newcap * sizeof(t_triangle));
		if (!newarr)
			return false;
		mesh->triangles = newarr;
		mesh->capacity = newcap;
	}

	mesh->triangles[mesh->count++] = *tri;
	mesh->bbox = aabb_merge(&mesh->bbox, &tri->bbox);
	return true;
}

static inline void mesh_clear(t_mesh *mesh)
{
	if (!mesh)
		return;
	free(mesh->triangles);
	mesh->triangles = NULL;
	mesh->count = 0;
	mesh->capacity = 0;
	mesh->bbox = aabb_empty();
}

/* Add all triangles from mesh to hittable list */
static inline bool mesh_add_to_list(const t_mesh *mesh, t_hittable_list *list)
{
	if (!mesh || !list)
		return false;

	for (size_t i = 0; i < mesh->count; ++i)
	{
		if (!hittable_list_add_triangle(list, &mesh->triangles[i]))
			return false;
	}
	return true;
}

/* ============================================================================ */
/*                          PROCEDURAL MESH GENERATION                          */
/* ============================================================================ */

/* Create a quad from two triangles */
static inline void mesh_add_quad(t_mesh *mesh, const t_point3 *a, const t_point3 *b,
								 const t_point3 *c, const t_point3 *d, t_material *mat)
{
	/* Quad ABCD split into triangles ABC and ACD */
	t_triangle t1 = triangle_create(a, b, c, mat);
	t_triangle t2 = triangle_create(a, c, d, mat);
	mesh_add_triangle(mesh, &t1);
	mesh_add_triangle(mesh, &t2);
}

/* Create a box from triangles (12 triangles, 2 per face) */
static inline void mesh_add_box(t_mesh *mesh, const t_point3 *min, const t_point3 *max, t_material *mat)
{
	if (!mesh || !min || !max || !mat)
		return;

	/* 8 vertices of the box */
	t_point3 v[8] = {
		point3_create(min->x, min->y, min->z), /* 0: front-bottom-left */
		point3_create(max->x, min->y, min->z), /* 1: front-bottom-right */
		point3_create(max->x, max->y, min->z), /* 2: front-top-right */
		point3_create(min->x, max->y, min->z), /* 3: front-top-left */
		point3_create(min->x, min->y, max->z), /* 4: back-bottom-left */
		point3_create(max->x, min->y, max->z), /* 5: back-bottom-right */
		point3_create(max->x, max->y, max->z), /* 6: back-top-right */
		point3_create(min->x, max->y, max->z)  /* 7: back-top-left */
	};

	/* Front face (z = min->z): 0-1-2, 0-2-3 */
	mesh_add_quad(mesh, &v[0], &v[1], &v[2], &v[3], mat);

	/* Back face (z = max->z): 5-4-7, 5-7-6 */
	mesh_add_quad(mesh, &v[5], &v[4], &v[7], &v[6], mat);

	/* Left face (x = min->x): 4-0-3, 4-3-7 */
	mesh_add_quad(mesh, &v[4], &v[0], &v[3], &v[7], mat);

	/* Right face (x = max->x): 1-5-6, 1-6-2 */
	mesh_add_quad(mesh, &v[1], &v[5], &v[6], &v[2], mat);

	/* Top face (y = max->y): 3-2-6, 3-6-7 */
	mesh_add_quad(mesh, &v[3], &v[2], &v[6], &v[7], mat);

	/* Bottom face (y = min->y): 4-5-1, 4-1-0 */
	mesh_add_quad(mesh, &v[4], &v[5], &v[1], &v[0], mat);
}

/* Create a pyramid from triangles */
static inline void mesh_add_pyramid(t_mesh *mesh, const t_point3 *base_center,
									real_t base_size, real_t height, t_material *mat)
{
	if (!mesh || !base_center || !mat)
		return;

	real_t half = base_size / (real_t)2.0;

	/* Base vertices (square base) */
	t_point3 b0 = point3_create(base_center->x - half, base_center->y, base_center->z - half);
	t_point3 b1 = point3_create(base_center->x + half, base_center->y, base_center->z - half);
	t_point3 b2 = point3_create(base_center->x + half, base_center->y, base_center->z + half);
	t_point3 b3 = point3_create(base_center->x - half, base_center->y, base_center->z + half);

	/* Apex */
	t_point3 apex = point3_create(base_center->x, base_center->y + height, base_center->z);

	/* Base (two triangles) */
	mesh_add_quad(mesh, &b0, &b1, &b2, &b3, mat);

	/* Four side faces */
	t_triangle s0 = triangle_create(&b0, &b1, &apex, mat);
	t_triangle s1 = triangle_create(&b1, &b2, &apex, mat);
	t_triangle s2 = triangle_create(&b2, &b3, &apex, mat);
	t_triangle s3 = triangle_create(&b3, &b0, &apex, mat);

	mesh_add_triangle(mesh, &s0);
	mesh_add_triangle(mesh, &s1);
	mesh_add_triangle(mesh, &s2);
	mesh_add_triangle(mesh, &s3);
}

/* Create icosahedron (20-triangle sphere approximation) */
static inline void mesh_add_icosahedron(t_mesh *mesh, const t_point3 *center,
										real_t radius, t_material *mat)
{
	if (!mesh || !center || !mat || radius <= 0)
		return;

	/* Golden ratio */
	const real_t phi = (real_t)((1.0 + sqrt(5.0)) / 2.0);

	/* Normalize to unit sphere then scale */
	real_t len = (real_t)sqrt(1.0 + (double)(phi * phi));
	real_t a = radius / len;
	real_t b = radius * phi / len;

	/* 12 vertices of icosahedron */
	t_point3 v[12] = {
		point3_create(center->x - a, center->y + b, center->z),
		point3_create(center->x + a, center->y + b, center->z),
		point3_create(center->x - a, center->y - b, center->z),
		point3_create(center->x + a, center->y - b, center->z),
		point3_create(center->x, center->y - a, center->z + b),
		point3_create(center->x, center->y + a, center->z + b),
		point3_create(center->x, center->y - a, center->z - b),
		point3_create(center->x, center->y + a, center->z - b),
		point3_create(center->x + b, center->y, center->z - a),
		point3_create(center->x + b, center->y, center->z + a),
		point3_create(center->x - b, center->y, center->z - a),
		point3_create(center->x - b, center->y, center->z + a)};

	/* 20 faces (vertex indices) */
	int faces[20][3] = {
		{0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8}, {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9}, {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}};

	for (int i = 0; i < 20; ++i)
	{
		t_triangle tri = triangle_create(&v[faces[i][0]], &v[faces[i][1]], &v[faces[i][2]], mat);
		mesh_add_triangle(mesh, &tri);
	}
}

#endif
