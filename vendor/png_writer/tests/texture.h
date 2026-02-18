/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 17:05:38 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 20:08:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "point.h"
#include "lode_image.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

/* Forward declaration */
typedef struct s_texture t_texture;

/* Abstract texture type with virtual value function */
typedef struct s_texture
{
	void *data;
	t_color (*value)(const struct s_texture *tex, real_t u, real_t v, const t_point3 *p);
	void (*destroy)(struct s_texture *tex);
} t_texture;

/* ---------- Solid color ---------- */
typedef struct s_solid_color
{
	t_color albedo;
} t_solid_color;

static inline t_color solid_color_value(const t_texture *tex, real_t u, real_t v, const t_point3 *p)
{
	(void)u;
	(void)v;
	(void)p;
	const t_solid_color *sc = (const t_solid_color *)tex->data;
	return sc ? sc->albedo : vec3_create(0.0, 0.0, 0.0);
}

static inline void solid_color_destroy(t_texture *tex)
{
	if (tex && tex->data)
	{
		free(tex->data);
		tex->data = NULL;
	}
}

static inline t_texture *solid_color_create(t_color albedo)
{
	t_texture *tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return NULL;
	t_solid_color *sc = (t_solid_color *)malloc(sizeof(t_solid_color));
	if (!sc)
	{
		free(tex);
		return NULL;
	}
	sc->albedo = albedo;
	tex->data = sc;
	tex->value = solid_color_value;
	tex->destroy = solid_color_destroy;
	return tex;
}

/* ---------- Checker ---------- */
typedef struct s_checker_texture
{
	real_t inv_scale;
	t_texture *even;
	t_texture *odd;
} t_checker_texture;

static inline t_color checker_texture_value(const t_texture *tex, real_t u, real_t v, const t_point3 *p)
{
	(void)u;
	(void)v;
	const t_checker_texture *checker = (const t_checker_texture *)tex->data;
	if (!checker || !checker->even || !checker->odd)
		return vec3_create(0.0, 0.0, 0.0);

	int xi = (int)floor((double)(checker->inv_scale * p->x));
	int yi = (int)floor((double)(checker->inv_scale * p->y));
	int zi = (int)floor((double)(checker->inv_scale * p->z));

	bool is_even = ((xi + yi + zi) % 2) == 0;
	t_texture *sample = is_even ? checker->even : checker->odd;
	return sample->value(sample, u, v, p);
}

static inline void checker_texture_destroy(t_texture *tex)
{
	if (!tex || !tex->data)
		return;
	t_checker_texture *checker = (t_checker_texture *)tex->data;
	if (checker->even)
	{
		checker->even->destroy(checker->even);
		free(checker->even);
	}
	if (checker->odd)
	{
		checker->odd->destroy(checker->odd);
		free(checker->odd);
	}
	free(tex->data);
	tex->data = NULL;
}

static inline t_texture *checker_texture_create(real_t scale, t_texture *even, t_texture *odd)
{
	t_texture *tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return NULL;
	t_checker_texture *checker = (t_checker_texture *)malloc(sizeof(t_checker_texture));
	if (!checker)
	{
		free(tex);
		return NULL;
	}
	checker->inv_scale = (scale > 0.0) ? (1.0 / scale) : 1.0;
	checker->even = even;
	checker->odd = odd;
	tex->data = checker;
	tex->value = checker_texture_value;
	tex->destroy = checker_texture_destroy;
	return tex;
}

/* ---------- Image texture (PNG) ---------- */
typedef struct s_image_texture
{
	t_lode_image image;
} t_image_texture;

static inline real_t srgb_to_linear(unsigned char c)
{
	double v = (double)c / 255.0;
	/* More accurate sRGB to linear conversion per IEC 61966-2-1 standard */
	return (real_t)((v <= 0.04045) ? (v / 12.92) : pow((v + 0.055) / 1.055, 2.4));
}

static inline real_t linear_to_srgb(real_t linear)
{
	/* Linear to sRGB conversion for tone mapping */
	return (linear <= 0.0031308) ? (12.92 * linear) : (1.055 * pow(linear, 1.0 / 2.4) - 0.055);
}

static inline t_color image_texture_value(const t_texture *tex, real_t u, real_t v, const t_point3 *p)
{
	(void)p;
	const t_image_texture *it = (const t_image_texture *)tex->data;
	if (!it || it->image.w == 0 || it->image.h == 0)
		return vec3_create(0.0, 1.0, 1.0); /* cyan debug */

	/* Wrap UVs for seamless tiling */
	u = u - floor(u);
	v = v - floor(v);
	v = 1.0 - v;

	/* High-quality bilinear filtering with proper wrapping */
	real_t x = u * (real_t)it->image.w;
	real_t y = v * (real_t)it->image.h;

	int x0 = (int)floor(x) % (int)it->image.w;
	int y0 = (int)floor(y) % (int)it->image.h;
	int x1 = (x0 + 1) % (int)it->image.w;
	int y1 = (y0 + 1) % (int)it->image.h;

	real_t fx = x - floor(x);
	real_t fy = y - floor(y);

	/* Fetch 4 corner pixels */
	const unsigned char *p00 = lode_image_pixel_rgb(&it->image, x0, y0);
	const unsigned char *p10 = lode_image_pixel_rgb(&it->image, x1, y0);
	const unsigned char *p01 = lode_image_pixel_rgb(&it->image, x0, y1);
	const unsigned char *p11 = lode_image_pixel_rgb(&it->image, x1, y1);

	/* Convert to linear space for proper blending */
	t_color c00 = vec3_create(srgb_to_linear(p00[0]), srgb_to_linear(p00[1]), srgb_to_linear(p00[2]));
	t_color c10 = vec3_create(srgb_to_linear(p10[0]), srgb_to_linear(p10[1]), srgb_to_linear(p10[2]));
	t_color c01 = vec3_create(srgb_to_linear(p01[0]), srgb_to_linear(p01[1]), srgb_to_linear(p01[2]));
	t_color c11 = vec3_create(srgb_to_linear(p11[0]), srgb_to_linear(p11[1]), srgb_to_linear(p11[2]));

	/* Bilinear interpolation in linear space */
	t_color dx0 = vec3_sub(&c10, &c00);
	t_color dx1 = vec3_sub(&c11, &c01);
	t_color c0_scalar = vec3_mul_scalar(&dx0, fx);
	t_color c1_scalar = vec3_mul_scalar(&dx1, fx);
	t_color c0 = vec3_add(&c00, &c0_scalar);
	t_color c1 = vec3_add(&c01, &c1_scalar);
	t_color dy = vec3_sub(&c1, &c0);
	t_color dy_scalar = vec3_mul_scalar(&dy, fy);

	return vec3_add(&c0, &dy_scalar);
}

static inline void image_texture_destroy(t_texture *tex)
{
	if (!tex || !tex->data)
		return;
	t_image_texture *it = (t_image_texture *)tex->data;
	lode_image_cleanup(&it->image);
	free(it);
	tex->data = NULL;
}

static inline t_texture *image_texture_create_png(const char *filename)
{
	t_texture *tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return NULL;

	t_image_texture *it = (t_image_texture *)malloc(sizeof(t_image_texture));
	if (!it)
	{
		free(tex);
		return NULL;
	}

	lode_image_init(&it->image);
	/* load and report failure */
	{
		unsigned err = lode_image_load_png(&it->image, filename);
		if (err)
		{
			fprintf(stderr, "lodepng error %u: %s (%s)\n", err, lodepng_error_text(err), filename);
			lode_image_cleanup(&it->image);
			free(it);
			free(tex);
			return NULL;
		}
	}

	tex->data = it;
	tex->value = image_texture_value;
	tex->destroy = image_texture_destroy;
	return tex;
}

#endif