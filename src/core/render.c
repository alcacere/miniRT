/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:20:42 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:20:45 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include <stdlib.h>
#include <math.h>

int	rgb_to_int(t_color color)
{
	int	r;
	int	g;
	int	b;

	color.x = sqrt(fabs(color.x));
	color.y = sqrt(fabs(color.y));
	color.z = sqrt(fabs(color.z));
	if (color.x > 1.0)
		color.x = 1.0;
	if (color.y > 1.0)
		color.y = 1.0;
	if (color.z > 1.0)
		color.z = 1.0;
	r = (int)(color.x * 255.0);
	g = (int)(color.y * 255.0);
	b = (int)(color.z * 255.0);
	return ((r << 16) | (g << 8) | b);
}

static t_color	render_pixel(t_render_ctx *ctx, int c[2])
{
	t_color		color;
	t_ray		r;
	int			s[2];
	uint32_t	seed;

	color = vec3_create(0, 0, 0);
	seed = (uint32_t)(c[1] * 73856093 ^ c[0] * 19349663);
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
