/* ============================================================================ */
/*                                                                              */
/*  Probability Density Functions for Importance Sampling                       */
/*                                                                              */
/* ============================================================================ */

#ifndef PDF_H
#define PDF_H

#include "types.h"
#include "vector.h"
#include "hittable.h"
#include "settings.h"
#include <stdlib.h>
#include <math.h>

/* Abstract PDF type */
typedef struct s_pdf
{
	void *data;
	real_t (*value)(const struct s_pdf *pdf, const t_vec3 *direction);
	t_vec3 (*generate)(const struct s_pdf *pdf);
	void (*destroy)(struct s_pdf *pdf);
} t_pdf;

/* ============================================================================ */
/*                          COSINE PDF                                          */
/* ============================================================================ */

typedef struct s_cosine_pdf
{
	t_vec3 uvw[3]; /* orthonormal basis (u, v, w) where w = normal */
} t_cosine_pdf;

/* Build orthonormal basis from normal vector */
static inline void build_onb_from_w(t_vec3 *uvw, const t_vec3 *n)
{
	uvw[2] = unit_vector(n); /* w = normalized normal */

	t_vec3 a;
	if (fabsl((long double)uvw[2].x) > (long double)0.9)
		a = vec3_create(0.0, 1.0, 0.0);
	else
		a = vec3_create(1.0, 0.0, 0.0);

	t_vec3 cross_wa = cross(&uvw[2], &a);
	uvw[1] = unit_vector(&cross_wa);  /* v */
	uvw[0] = cross(&uvw[2], &uvw[1]); /* u */
}

static inline t_vec3 onb_local(const t_vec3 *uvw, const t_vec3 *a)
{
	t_vec3 u_scaled = vec3_mul_scalar(&uvw[0], a->x);
	t_vec3 v_scaled = vec3_mul_scalar(&uvw[1], a->y);
	t_vec3 w_scaled = vec3_mul_scalar(&uvw[2], a->z);
	t_vec3 uv = vec3_add(&u_scaled, &v_scaled);
	return vec3_add(&uv, &w_scaled);
}

static inline real_t cosine_pdf_value(const t_pdf *pdf, const t_vec3 *direction)
{
	const t_cosine_pdf *cpdf = (const t_cosine_pdf *)pdf->data;
	if (!cpdf)
		return 0.0;

	t_vec3 unit_dir = unit_vector(direction);
	real_t cosine = dot(&unit_dir, &cpdf->uvw[2]);

	if (cosine <= 0.0)
		return 0.0;
	return cosine / (real_t)PI;
}

static inline t_vec3 cosine_pdf_generate(const t_pdf *pdf)
{
	const t_cosine_pdf *cpdf = (const t_cosine_pdf *)pdf->data;
	if (!cpdf)
		return vec3_create(0.0, 1.0, 0.0);

	/* Generate random direction in cosine-weighted hemisphere */
	real_t r1 = random_real();
	real_t r2 = random_real();
	real_t z = (real_t)sqrt(1.0 - (double)r2);

	real_t phi = 2.0 * PI * r1;
	real_t x = (real_t)cos((double)phi) * (real_t)sqrt((double)r2);
	real_t y = (real_t)sin((double)phi) * (real_t)sqrt((double)r2);

	t_vec3 local_dir = vec3_create(x, y, z);
	return onb_local(cpdf->uvw, &local_dir);
}

static inline void cosine_pdf_destroy(t_pdf *pdf)
{
	if (pdf && pdf->data)
	{
		free(pdf->data);
		pdf->data = NULL;
	}
}

static inline t_pdf *cosine_pdf_create(const t_vec3 *normal)
{
	t_pdf *pdf = (t_pdf *)malloc(sizeof(t_pdf));
	if (!pdf)
		return NULL;

	t_cosine_pdf *cpdf = (t_cosine_pdf *)malloc(sizeof(t_cosine_pdf));
	if (!cpdf)
	{
		free(pdf);
		return NULL;
	}

	build_onb_from_w(cpdf->uvw, normal);

	pdf->data = cpdf;
	pdf->value = cosine_pdf_value;
	pdf->generate = cosine_pdf_generate;
	pdf->destroy = cosine_pdf_destroy;

	return pdf;
}

/* ============================================================================ */
/*                          SPHERE PDF (uniform on sphere)                      */
/* ============================================================================ */

static inline real_t sphere_pdf_value(const t_pdf *pdf, const t_vec3 *direction)
{
	(void)pdf;
	(void)direction;
	return 1.0 / (4.0 * PI);
}

static inline t_vec3 sphere_pdf_generate(const t_pdf *pdf)
{
	(void)pdf;
	return random_unit_vector();
}

static inline void sphere_pdf_destroy(t_pdf *pdf)
{
	/* No data to free */
	(void)pdf;
}

static inline t_pdf *sphere_pdf_create(void)
{
	t_pdf *pdf = (t_pdf *)malloc(sizeof(t_pdf));
	if (!pdf)
		return NULL;

	pdf->data = NULL;
	pdf->value = sphere_pdf_value;
	pdf->generate = sphere_pdf_generate;
	pdf->destroy = sphere_pdf_destroy;

	return pdf;
}

/* ============================================================================ */
/*                          MIXTURE PDF                                         */
/* ============================================================================ */

typedef struct s_mixture_pdf
{
	t_pdf *p0;
	t_pdf *p1;
} t_mixture_pdf;

static inline real_t mixture_pdf_value(const t_pdf *pdf, const t_vec3 *direction)
{
	const t_mixture_pdf *mpdf = (const t_mixture_pdf *)pdf->data;
	if (!mpdf || !mpdf->p0 || !mpdf->p1)
		return 0.0;

	return 0.5 * mpdf->p0->value(mpdf->p0, direction) +
		   0.5 * mpdf->p1->value(mpdf->p1, direction);
}

static inline t_vec3 mixture_pdf_generate(const t_pdf *pdf)
{
	const t_mixture_pdf *mpdf = (const t_mixture_pdf *)pdf->data;
	if (!mpdf)
		return vec3_create(0.0, 1.0, 0.0);

	if (random_real() < 0.5)
		return mpdf->p0->generate(mpdf->p0);
	else
		return mpdf->p1->generate(mpdf->p1);
}

static inline void mixture_pdf_destroy(t_pdf *pdf)
{
	if (!pdf || !pdf->data)
		return;

	t_mixture_pdf *mpdf = (t_mixture_pdf *)pdf->data;
	/* Note: we don't destroy p0 and p1 here - caller owns them */
	free(mpdf);
	pdf->data = NULL;
}

static inline t_pdf *mixture_pdf_create(t_pdf *p0, t_pdf *p1)
{
	if (!p0 || !p1)
		return NULL;

	t_pdf *pdf = (t_pdf *)malloc(sizeof(t_pdf));
	if (!pdf)
		return NULL;

	t_mixture_pdf *mpdf = (t_mixture_pdf *)malloc(sizeof(t_mixture_pdf));
	if (!mpdf)
	{
		free(pdf);
		return NULL;
	}

	mpdf->p0 = p0;
	mpdf->p1 = p1;

	pdf->data = mpdf;
	pdf->value = mixture_pdf_value;
	pdf->generate = mixture_pdf_generate;
	pdf->destroy = mixture_pdf_destroy;

	return pdf;
}

#endif
