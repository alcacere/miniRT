/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       material.h                                                      */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 21:47:29                                             */
/*  Updated:    2026/01/04 21:47:29                                             */
/*                                                                              */
/* ============================================================================ */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "hittable.h"
#include <stdlib.h>
#include "texture.h" // for t_texture and solid_color_create
/* Forward declarations to handle circular dependencies */
typedef struct s_hit_record t_hit_record;
/* t_texture now comes from texture.h; keep the typedef for safety */
typedef struct s_texture t_texture;

/* Abstract material type: encapsulates scatter behavior and emission */
typedef struct s_material
{
	/* Private data for the specific material */
	void *data;

	/* Virtual emitted function: returns color emitted by this material */
	t_color (*emitted)(const struct s_material *mat, real_t u, real_t v, const t_point3 *p);

	/* Virtual scatter function: returns true if ray scattered, false if absorbed */
	bool (*scatter)(const struct s_material *mat, const t_ray *r_in,
					const t_hit_record *rec, t_color *attenuation, t_ray *scattered);

	/* Virtual scattering_pdf function: returns PDF value for scattered ray direction */
	real_t (*scattering_pdf)(const struct s_material *mat, const t_ray *r_in,
							 const t_hit_record *rec, const t_ray *scattered);

	/* Optional destructor for cleanup */
	void (*destroy)(struct s_material *mat);
} t_material;

/* ============================================================================ */
/*                          HELPER FUNCTIONS (MOVED EARLY)                      */
/* ============================================================================ */

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

/* Default emitted: returns black (no emission) */
static inline t_color default_emitted(const t_material *mat, real_t u, real_t v, const t_point3 *p)
{
	(void)mat;
	(void)u;
	(void)v;
	(void)p;
	return vec3_create(0.0, 0.0, 0.0);
}

/* Default scattering_pdf: returns 0 (no scattering) */
static inline real_t default_scattering_pdf(const t_material *mat, const t_ray *r_in,
											const t_hit_record *rec, const t_ray *scattered)
{
	(void)mat;
	(void)r_in;
	(void)rec;
	(void)scattered;
	return (real_t)0.0;
}

/* ============================================================================ */
/*                          MATERIAL TYPE DEFINITIONS                           */
/* ============================================================================ */

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

/* Diffuse light material: emits light from a texture */
typedef struct s_diffuse_light
{
	t_texture *tex;
	real_t scale; /* NEW: emission multiplier (1.0 = default) */
} t_diffuse_light;

/* Isotropic material type: scatters uniformly in all directions */
typedef struct s_isotropic
{
	t_texture *tex;
} t_isotropic;

/* Tinted glass material type: dielectric with color absorption */
typedef struct s_tinted_glass
{
	real_t refraction_index;
	t_color tint; /* color tint (absorbed per unit distance) */
} t_tinted_glass;

/* Glossy material: combination of diffuse and specular reflection */
typedef struct s_glossy
{
	t_color albedo;
	real_t roughness; /* 0 = mirror, 1 = fully diffuse */
	real_t metalness; /* 0 = dielectric, 1 = metal */
} t_glossy;

/* ============================================================================ */
/*                          SCATTER & PDF FUNCTIONS                             */
/* ============================================================================ */

/* Lambertian scattering_pdf: cos_theta / pi for cosine-weighted hemisphere */
static inline real_t lambertian_scattering_pdf(const t_material *mat, const t_ray *r_in,
											   const t_hit_record *rec, const t_ray *scattered)
{
	(void)mat;
	(void)r_in;
	real_t cos_theta = dot(&rec->normal, &scattered->dir);
	if (cos_theta < (real_t)0.0)
		return (real_t)0.0;
	return cos_theta / (real_t)PI;
}

/* Lambertian scatter: sample texture color and preserve ray time */
static inline bool lambertian_scatter(const t_material *mat, const t_ray *r_in,
									  const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_lambertian *lamb = (const t_lambertian *)mat->data;
	if (!lamb || !lamb->tex)
		return false;

	/* Use cosine-weighted hemisphere sampling instead of random unit vector */
	/* This concentrates rays toward the normal, improving convergence */
	t_vec3 scatter_direction = random_cosine_direction(&rec->normal);

	/* Catch degenerate scatter direction (extremely rare with cosine sampling) */
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

/* Tinted glass scatter: similar to dielectric but applies color tint */
static inline bool tinted_glass_scatter(const t_material *mat, const t_ray *r_in,
										const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_tinted_glass *glass = (const t_tinted_glass *)mat->data;
	if (!glass)
		return false;

	/* Apply tint as attenuation */
	*attenuation = glass->tint;

	real_t ri = rec->front_face ? (1.0 / glass->refraction_index) : glass->refraction_index;
	t_vec3 unit_direction = unit_vector(&r_in->dir);

	t_vec3 neg_unit_dir = vec3_neg(&unit_direction);
	real_t cos_theta = dot(&neg_unit_dir, &rec->normal);
	if (cos_theta > 1.0)
		cos_theta = 1.0;

	real_t sin_theta = (real_t)sqrt(1.0 - (double)(cos_theta * cos_theta));

	bool cannot_refract = (ri * sin_theta) > 1.0;
	real_t refl = reflectance(cos_theta, ri);
	bool should_reflect = cannot_refract || (refl > random_double());
	t_vec3 direction;

	if (should_reflect)
		direction = vec3_reflect(&unit_direction, &rec->normal);
	else
		direction = vec3_refract(&unit_direction, &rec->normal, ri);

	*scattered = ray_create(rec->p, direction, r_in->tm);
	return true;
}

/* Glossy scatter: blend between specular and diffuse */
static inline bool glossy_scatter(const t_material *mat, const t_ray *r_in,
								  const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_glossy *glossy = (const t_glossy *)mat->data;
	if (!glossy)
		return false;

	*attenuation = glossy->albedo;

	/* Blend between specular reflection and diffuse based on roughness */
	if (random_double() > glossy->roughness)
	{
		/* Specular reflection with slight perturbation */
		t_vec3 reflected = vec3_reflect(&r_in->dir, &rec->normal);
		t_vec3 unit_reflected = unit_vector(&reflected);
		t_vec3 fuzz_vec = random_unit_vector();
		t_vec3 fuzz_offset = vec3_mul_scalar(&fuzz_vec, glossy->roughness * 0.5);
		t_vec3 direction = vec3_add(&unit_reflected, &fuzz_offset);

		if (dot(&direction, &rec->normal) <= 0)
			direction = rec->normal;

		*scattered = ray_create(rec->p, direction, r_in->tm);
	}
	else
	{
		/* Diffuse scatter */
		t_vec3 scatter_direction = random_cosine_direction(&rec->normal);
		if (vec3_near_zero(&scatter_direction))
			scatter_direction = rec->normal;
		*scattered = ray_create(rec->p, scatter_direction, r_in->tm);
	}

	return true;
}

/* Diffuse light emitted: sample texture color */
static inline t_color diffuse_light_emitted(const t_material *mat, real_t u, real_t v, const t_point3 *p)
{
	const t_diffuse_light *light = (const t_diffuse_light *)mat->data;
	if (!light || !light->tex)
		return vec3_create(0.0, 0.0, 0.0);
	/* NEW: apply emission scaling */
	t_color c = light->tex->value(light->tex, u, v, p);
	return vec3_mul_scalar(&c, (light->scale > (real_t)0.0) ? light->scale : (real_t)1.0);
}

/* Diffuse light scatter: does not scatter, always returns false */
static inline bool diffuse_light_scatter(const t_material *mat, const t_ray *r_in,
										 const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	(void)mat;
	(void)r_in;
	(void)rec;
	(void)attenuation;
	(void)scattered;
	return false;
}

/* Isotropic scatter: uniform random direction */
static inline bool isotropic_scatter(const t_material *mat, const t_ray *r_in,
									 const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_isotropic *iso = (const t_isotropic *)mat->data;
	if (!iso || !iso->tex)
		return false;

	/* Sample random unit vector (isotropic scattering) */
	t_vec3 scattered_dir = random_unit_vector();

	/* Construct scattered ray preserving time */
	*scattered = ray_create(rec->p, scattered_dir, r_in->tm);

	/* Sample color from texture */
	*attenuation = iso->tex->value(iso->tex, rec->u, rec->v, &rec->p);

	return true;
}

/* ============================================================================ */
/*                          DESTRUCTOR FUNCTIONS                                */
/* ============================================================================ */

/* Lambertian destructor: only destroy texture if created via lambertian_create (not _texture variant) */
static inline void lambertian_destroy(t_material *mat)
{
	if (!mat || !mat->data)
		return;

	t_lambertian *lamb = (t_lambertian *)mat->data;

	/* Note: We do NOT destroy the texture here because:
	   - If created via lambertian_create(), the texture IS owned and should be destroyed
	   - If created via lambertian_create_texture(), the texture is NOT owned (shared)

	   Since we can't distinguish, callers must manage texture lifetime explicitly.
	   The texture pointer is stored but not owned by this destructor. */

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

/* Tinted glass destructor */
static inline void tinted_glass_destroy(t_material *mat)
{
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}

/* Glossy destructor */
static inline void glossy_destroy(t_material *mat)
{
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}

/* Diffuse light destructor: destroy owned texture */
static inline void diffuse_light_destroy(t_material *mat)
{
	if (!mat || !mat->data)
		return;

	t_diffuse_light *light = (t_diffuse_light *)mat->data;

	if (light->tex && light->tex->destroy)
	{
		light->tex->destroy(light->tex);
		free(light->tex);
		light->tex = NULL;
	}

	free(mat->data);
	mat->data = NULL;
}

/* Isotropic destructor */
static inline void isotropic_destroy(t_material *mat)
{
	if (!mat || !mat->data)
		return;

	t_isotropic *iso = (t_isotropic *)mat->data;

	/* Texture is not owned by isotropic (shared ownership) */
	free(mat->data);
	mat->data = NULL;
}

/* ============================================================================ */
/*                          CONSTRUCTOR FUNCTIONS                               */
/* ============================================================================ */

/* Lambertian constructor from texture: use pre-built texture (shared ownership) */
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
	mat->emitted = default_emitted;
	mat->scatter = lambertian_scatter;
	mat->scattering_pdf = lambertian_scattering_pdf;
	mat->destroy = lambertian_destroy;

	return mat;
}

/* Lambertian constructor from color: creates a solid_color texture (owned) */
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

	lamb->tex = solid_color_create(albedo);
	if (!lamb->tex)
	{
		free(lamb);
		free(mat);
		return NULL;
	}

	mat->data = lamb;
	mat->emitted = default_emitted;
	mat->scatter = lambertian_scatter;
	mat->scattering_pdf = lambertian_scattering_pdf;
	mat->destroy = lambertian_destroy; /* solid color texture is owned */

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

	if (fuzz < (real_t)0.0)
		fuzz = (real_t)0.0;
	else if (fuzz > (real_t)1.0)
		fuzz = (real_t)1.0;

	metal->albedo = albedo;
	metal->fuzz = fuzz;

	mat->data = metal;
	mat->emitted = default_emitted;
	mat->scatter = metal_scatter;
	mat->scattering_pdf = default_scattering_pdf;
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
	mat->emitted = default_emitted;
	mat->scatter = dielectric_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = dielectric_destroy;

	return mat;
}

/* Tinted glass constructor */
static inline t_material *tinted_glass_create(real_t refraction_index, t_color tint)
{
	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_tinted_glass *glass = (t_tinted_glass *)malloc(sizeof(t_tinted_glass));
	if (!glass)
	{
		free(mat);
		return NULL;
	}

	glass->refraction_index = refraction_index;
	glass->tint = tint;

	mat->data = glass;
	mat->emitted = default_emitted;
	mat->scatter = tinted_glass_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = tinted_glass_destroy;

	return mat;
}

/* Glossy constructor */
static inline t_material *glossy_create(t_color albedo, real_t roughness, real_t metalness)
{
	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_glossy *glossy = (t_glossy *)malloc(sizeof(t_glossy));
	if (!glossy)
	{
		free(mat);
		return NULL;
	}

	glossy->albedo = albedo;
	glossy->roughness = clamp(roughness, 0.0, 1.0);
	glossy->metalness = clamp(metalness, 0.0, 1.0);

	mat->data = glossy;
	mat->emitted = default_emitted;
	mat->scatter = glossy_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = glossy_destroy;

	return mat;
}

/* Diffuse light constructor from color: creates a solid_color texture */
static inline t_material *diffuse_light_create(t_color emit)
{
	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_diffuse_light *light = (t_diffuse_light *)malloc(sizeof(t_diffuse_light));
	if (!light)
	{
		free(mat);
		return NULL;
	}

	light->tex = solid_color_create(emit);
	if (!light->tex)
	{
		free(light);
		free(mat);
		return NULL;
	}

	light->scale = (real_t)1.0; /* NEW */

	mat->data = light;
	mat->emitted = diffuse_light_emitted;
	mat->scatter = diffuse_light_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = diffuse_light_destroy;

	return mat;
}

/* NEW: Diffuse light constructor from color with scale */
static inline t_material *diffuse_light_create_scaled(t_color emit, real_t scale)
{
	t_material *mat = diffuse_light_create(emit);
	if (!mat || !mat->data)
		return mat;
	((t_diffuse_light *)mat->data)->scale = (scale > (real_t)0.0) ? scale : (real_t)1.0;
	return mat;
}

/* Diffuse light constructor from texture */
static inline t_material *diffuse_light_create_texture(t_texture *tex)
{
	if (!tex)
		return NULL;

	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_diffuse_light *light = (t_diffuse_light *)malloc(sizeof(t_diffuse_light));
	if (!light)
	{
		free(mat);
		return NULL;
	}

	light->tex = tex;
	light->scale = (real_t)1.0; /* NEW */

	mat->data = light;
	mat->emitted = diffuse_light_emitted;
	mat->scatter = diffuse_light_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = diffuse_light_destroy;

	return mat;
}

/* NEW: Diffuse light constructor from texture with scale */
static inline t_material *diffuse_light_create_texture_scaled(t_texture *tex, real_t scale)
{
	t_material *mat = diffuse_light_create_texture(tex);
	if (!mat || !mat->data)
		return mat;
	((t_diffuse_light *)mat->data)->scale = (scale > (real_t)0.0) ? scale : (real_t)1.0;
	return mat;
}

/* Isotropic constructor from color (creates solid_color texture) */
static inline t_material *isotropic_create(t_color albedo)
{
	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_isotropic *iso = (t_isotropic *)malloc(sizeof(t_isotropic));
	if (!iso)
	{
		free(mat);
		return NULL;
	}

	iso->tex = solid_color_create(albedo);
	if (!iso->tex)
	{
		free(iso);
		free(mat);
		return NULL;
	}

	mat->data = iso;
	mat->emitted = default_emitted;
	mat->scatter = isotropic_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = isotropic_destroy;

	return mat;
}

/* Isotropic constructor from texture (shared ownership) */
static inline t_material *isotropic_create_texture(t_texture *tex)
{
	if (!tex)
		return NULL;

	t_material *mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return NULL;

	t_isotropic *iso = (t_isotropic *)malloc(sizeof(t_isotropic));
	if (!iso)
	{
		free(mat);
		return NULL;
	}

	iso->tex = tex;

	mat->data = iso;
	mat->emitted = default_emitted;
	mat->scatter = isotropic_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = isotropic_destroy;

	return mat;
}

#endif