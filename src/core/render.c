/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:20:42 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 22:38:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "graphics.h"
#include <stdlib.h>
#include <math.h>

/*
** clamp_01: clamp a value to [0, 1] range.
*/
static double	clamp_01(double v)
{
	if (v < 0.0)
		return (0.0);
	if (v > 1.0)
		return (1.0);
	return (v);
}

/*
** apply_contrast: S-curve contrast centered at midpoint 0.5.
** CONTRAST > 1 increases contrast, < 1 reduces it.
*/
static double	apply_contrast(double v, double contrast)
{
	v = v - 0.5;
	v = v * contrast;
	return (v + 0.5);
}

/*
** rgb_to_int: converts a linear HDR color to 8-bit sRGB integer.
** Pipeline: gamma correction -> contrast -> saturation -> clamp -> pack.
** Tunable via GAMMA, CONTRAST, SATURATION macros in graphics.h.
*/
int	rgb_to_int(t_color color)
{
	double	gamma_inv;
	double	luma;
	int		rgb[3];

	color.x = color.x * EXPOSURE;
	color.y = color.y * EXPOSURE;
	color.z = color.z * EXPOSURE;
	gamma_inv = 1.0 / GAMMA;
	color.x = pow(fmax(color.x, 0.0), gamma_inv);
	color.y = pow(fmax(color.y, 0.0), gamma_inv);
	color.z = pow(fmax(color.z, 0.0), gamma_inv);
	color.x = apply_contrast(color.x, CONTRAST);
	color.y = apply_contrast(color.y, CONTRAST);
	color.z = apply_contrast(color.z, CONTRAST);
	luma = 0.2126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
	color.x = luma + SATURATION * (color.x - luma);
	color.y = luma + SATURATION * (color.y - luma);
	color.z = luma + SATURATION * (color.z - luma);
	rgb[0] = (int)(clamp_01(color.x) * 255.0);
	rgb[1] = (int)(clamp_01(color.y) * 255.0);
	rgb[2] = (int)(clamp_01(color.z) * 255.0);
	return ((rgb[0] << 16) | (rgb[1] << 8) | rgb[2]);
}

static t_color	render_pixel(t_render_ctx *ctx, int c[2])
{
	t_color		color;
	t_ray		r;
	int			s[2];
	uint32_t	seed;

	color = vec3_create(0, 0, 0);
	seed = ((uint32_t)c[1] * 73856093u ^ (uint32_t)c[0] * 19349663u) + 32;
	s[1] = -1;
	while (++s[1] < ctx->cam->sqrt_spp)
	{
		s[0] = -1;
		while (++s[0] < ctx->cam->sqrt_spp)
		{
			r = get_ray_stratified(ctx->cam, c, s);
			color = vec3_add(color, ray_color(&r, ctx,
						ctx->cam->max_depth, &seed));
		}
	}
	return (vec3_scale(color, ctx->cam->pixel_samples_scale));
}

static void	*render_worker(void *arg)
{
	t_render_ctx	*ctx;
	int				c[2];
	t_color			color;

	ctx = (t_render_ctx *)arg;
	c[1] = ctx->start_y;
	while (c[1] < ctx->end_y)
	{
		c[0] = 0;
		while (c[0] < ctx->cam->image_width)
		{
			color = render_pixel(ctx, c);
			ctx->image_buffer[c[1] * ctx->cam->image_width + c[0]]
				= rgb_to_int(color);
			c[0]++;
		}
		c[1]++;
	}
	return (NULL);
}

void	camera_render_threaded(t_camera *cam, t_hittable *world,
			int *buffer, t_scene *s)
{
	pthread_t		threads[THREAD_COUNT];
	t_render_ctx	ctx[THREAD_COUNT];
	int				i;
	int				rows;

	rows = cam->image_height / THREAD_COUNT;
	i = -1;
	while (++i < THREAD_COUNT)
	{
		ctx[i].cam = cam;
		ctx[i].world = world;
		ctx[i].scene = s;
		ctx[i].image_buffer = buffer;
		ctx[i].start_y = i * rows;
		ctx[i].end_y = (i + 1) * rows;
		if (i == THREAD_COUNT - 1)
			ctx[i].end_y = cam->image_height;
		pthread_create(&threads[i], NULL, render_worker, &ctx[i]);
	}
	i = -1;
	while (++i < THREAD_COUNT)
		pthread_join(threads[i], NULL);
}
