/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 14:18:47 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 17:14:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "hittable.h"
#include "texture.h"
#include <stdlib.h>

/* Forward declaration to handle circular dependency */
typedef struct s_hit_record t_hit_record;

/* Abstract material type: encapsulates scatter behavior */
typedef struct s_material
{
	/* Private data for the specific material */
	void *data;

	/* Virtual scatter function: returns true if ray scattered, false if absorbed */
	bool (*scatter)(const struct s_material *mat, const t_ray *r_in,
					const t_hit_record *rec, t_color *attenuation, t_ray *scattered);

	/* Optional destructor for cleanup */
	void (*destroy)(struct s_material *mat);
} t_material;

/* Lambertian (diffuse) material type: now holds a texture */
typedef struct s_lambertian
{
	t_texture *tex;
} t_lambertian;

/* Metal material type */
typedef struct s_metal
{
	t_color albedo;
	real_t fuzz;
} t_metal;

/* Dielectric material type */
typedef struct s_dielectric
{
	real_t refraction_index;
} t_dielectric;

/* Schlick's approximation for reflectance:
   r0 = ((1 - n) / (1 + n))²
   reflectance = r0 + (1-r0) * (1 - cos_theta)⁵ */
static inline real_t reflectance(real_t cosine, real_t refraction_index)
{
	real_t r0 = (1.0 - refraction_index) / (1.0 + refraction_index);
	r0 = r0 * r0;
	real_t one_minus_cos = 1.0 - cosine;
	real_t one_minus_cos5 = one_minus_cos * one_minus_cos * one_minus_cos * one_minus_cos * one_minus_cos;
	return r0 + (1.0 - r0) * one_minus_cos5;
}

/* Lambertian scatter: sample texture color and preserve ray time */
static inline bool lambertian_scatter(const t_material *mat, const t_ray *r_in,
									  const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_lambertian *lamb = (const t_lambertian *)mat->data;
	if (!lamb || !lamb->tex)
		return false;

	/* scatter_direction = rec.normal + random_unit_vector() */
	t_vec3 rnd = random_unit_vector();
	t_vec3 scatter_direction = vec3_add(&rec->normal, &rnd);

	/* Catch degenerate scatter direction */
	if (vec3_near_zero(&scatter_direction))
		scatter_direction = rec->normal;

	/* construct scattered ray from hit point with new direction, preserving time */
	*scattered = ray_create(rec->p, scatter_direction, r_in->tm);

	/* Sample color from texture using (u, v, p) */
	*attenuation = lamb->tex->value(lamb->tex, rec->u, rec->v, &rec->p);

	return true;
}

/* Metal scatter: preserve ray time */
static inline bool metal_scatter(const t_material *mat, const t_ray *r_in,
								 const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_metal *metal = (const t_metal *)mat->data;
	if (!metal)
		return false;

	/* reflected = reflect(r_in.direction(), rec.normal) */
	t_vec3 reflected = vec3_reflect(&r_in->dir, &rec->normal);

	/* normalize and add fuzz perturbation */
	t_vec3 unit_reflected = unit_vector(&reflected);
	t_vec3 random_vec = random_unit_vector();
	t_vec3 fuzz_offset = vec3_mul_scalar(&random_vec, metal->fuzz);
	t_vec3 fuzzed = vec3_add(&unit_reflected, &fuzz_offset);

	/* construct scattered ray from hit point with fuzzed direction, preserving time */
	*scattered = ray_create(rec->p, fuzzed, r_in->tm);

	/* attenuation by albedo */
	*attenuation = metal->albedo;

	/* return false if scattered ray points into surface */
	return (dot(&fuzzed, &rec->normal) > 0.0);
}

/* Dielectric scatter: preserve ray time */
static inline bool dielectric_scatter(const t_material *mat, const t_ray *r_in,
									  const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_dielectric *dielec = (const t_dielectric *)mat->data;
	if (!dielec)
		return false;

	/* attenuation = white */
	*attenuation = vec3_create(1.0, 1.0, 1.0);

	/* ri = front_face ? (1.0/refraction_index) : refraction_index */
	real_t ri = rec->front_face ? (1.0 / dielec->refraction_index) : dielec->refraction_index;

	/* unit_direction = unit_vector(r_in.direction()) */
	t_vec3 unit_direction = unit_vector(&r_in->dir);

	/* cos_theta = min(dot(-unit_direction, rec.normal), 1.0) */
	t_vec3 neg_unit_dir = vec3_neg(&unit_direction);
	real_t cos_theta = dot(&neg_unit_dir, &rec->normal);
	if (cos_theta > 1.0)
		cos_theta = 1.0;

	/* sin_theta = sqrt(1.0 - cos_theta²) */
	real_t sin_theta = (real_t)sqrt(1.0 - (double)(cos_theta * cos_theta));

	/* Check for total internal reflection or Schlick reflectance */
	bool cannot_refract = (ri * sin_theta) > 1.0;
	real_t refl = reflectance(cos_theta, ri);
	bool should_reflect = cannot_refract || (refl > random_double());
	t_vec3 direction;

	if (should_reflect)
	{
		/* Must reflect */
		direction = vec3_reflect(&unit_direction, &rec->normal);
	}
	else
	{
		/* Can refract */
		direction = vec3_refract(&unit_direction, &rec->normal, ri);
	}

	/* scattered = ray(rec.p, direction, r_in.time()) */
	*scattered = ray_create(rec->p, direction, r_in->tm);

	return true;
}

/* Lambertian destructor: destroy owned texture */
static inline void lambertian_destroy(t_material *mat)
{
	if (!mat || !mat->data)
		return;

	t_lambertian *lamb = (t_lambertian *)mat->data;
	if (lamb->tex)
	{
		lamb->tex->destroy(lamb->tex);
		free(lamb->tex);
		lamb->tex = NULL;
	}

	free(mat->data);
	mat->data = NULL;
}

/* Metal destructor */
static inline void metal_destroy(t_material *mat)
{
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}

/* Dielectric destructor */
static inline void dielectric_destroy(t_material *mat)
{
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}

/* Lambertian constructor from color: creates a solid_color texture */
static inline t_material *lambertian_create(t_color albedo)
{
	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_lambertian *lamb = (t_lambertian *)malloc(sizeof(t_lambertian));
	if (!lamb)
	{
		free(mat);
		return NULL;
	}

	/* Create a solid color texture from the albedo */
	lamb->tex = solid_color_create(albedo);
	if (!lamb->tex)
	{
		free(lamb);
		free(mat);
		return NULL;
	}

	mat->data = lamb;
	mat->scatter = lambertian_scatter;
	mat->destroy = lambertian_destroy;

	return mat;
}

/* Lambertian constructor from texture: use pre-built texture (non-owned) */
static inline t_material *lambertian_create_texture(t_texture *tex)
{
	if (!tex)
		return NULL;

	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_lambertian *lamb = (t_lambertian *)malloc(sizeof(t_lambertian));
	if (!lamb)
	{
		free(mat);
		return NULL;
	}

	lamb->tex = tex;

	mat->data = lamb;
	mat->scatter = lambertian_scatter;
	mat->destroy = lambertian_destroy;

	return mat;
}

/* Metal constructor with fuzz: allocate and initialize */
static inline t_material *metal_create_fuzz(t_color albedo, real_t fuzz)
{
	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_metal *metal = (t_metal *)malloc(sizeof(t_metal));
	if (!metal)
	{
		free(mat);
		return NULL;
	}

	/* clamp fuzz to [0, 1] */
	if (fuzz < (real_t)0.0)
		fuzz = (real_t)0.0;
	else if (fuzz > (real_t)1.0)
		fuzz = (real_t)1.0;

	metal->albedo = albedo;
	metal->fuzz = fuzz;

	mat->data = metal;
	mat->scatter = metal_scatter;
	mat->destroy = metal_destroy;

	return mat;
}

/* Metal constructor without fuzz (default fuzz=0) */
static inline t_material *metal_create(t_color albedo)
{
	return metal_create_fuzz(albedo, (real_t)0.0);
}

/* Dielectric constructor: allocate and initialize */
static inline t_material *dielectric_create(real_t refraction_index)
{
	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_dielectric *dielec = (t_dielectric *)malloc(sizeof(t_dielectric));
	if (!dielec)
	{
		free(mat);
		return NULL;
	}

	dielec->refraction_index = refraction_index;

	mat->data = dielec;
	mat->scatter = dielectric_scatter;
	mat->destroy = dielectric_destroy;

	return mat;
}

#endif