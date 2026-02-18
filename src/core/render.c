#include "camera.h"
#include <stdlib.h>

static int	rgb_to_int(t_vec3 color)
{
	int	r;
	int	g;
	int	b;

	r = (int)(255.999 * color.x);
	g = (int)(255.999 * color.y);
	b = (int)(255.999 * color.z);
	return ((r << 16) | (g << 8) | b);
}

static void	*render_worker(void *arg)
{
	t_render_ctx	*ctx;
	int				c[2];
	int				s[2];
	t_color			pixel_col;
	t_ray			r;

	ctx = (t_render_ctx *)arg;
	c[1] = ctx->start_y;
	while (c[1] < ctx->end_y)
	{
		c[0] = 0;
		while (c[0] < ctx->cam->image_width)
		{
			pixel_col = vec3_create(0, 0, 0);
			s[1] = -1;
			while (++s[1] < ctx->cam->sqrt_spp)
			{
				s[0] = -1;
				while (++s[0] < ctx->cam->sqrt_spp)
				{
					r = get_ray_stratified(ctx->cam, c, s);
					pixel_col = vec3_add(pixel_col, \
						ray_color(&r, ctx->world, ctx->cam->max_depth, ctx->cam->background));
				}
			}
			pixel_col = vec3_scale(pixel_col, ctx->cam->pixel_samples_scale);
			ctx->image_buffer[c[1] * ctx->cam->image_width + c[0]] = rgb_to_int(pixel_col);
			c[0]++;
		}
		c[1]++;
	}
	return (NULL);
}

static void	init_thread_ctx(t_render_ctx *ctx, t_camera *c, 
	t_hittable *w, int *buf)
{
	int	i;
	int	rows_per_thread;

	rows_per_thread = c->image_height / THREAD_COUNT;
	i = 0;
	while (i < THREAD_COUNT)
	{
		ctx[i].cam = c;
		ctx[i].world = w;
		ctx[i].image_buffer = buf;
		ctx[i].start_y = i * rows_per_thread;
		if (i == THREAD_COUNT - 1)
			ctx[i].end_y = c->image_height;
		else
			ctx[i].end_y = (i + 1) * rows_per_thread;
		i++;
	}
}

void	camera_render_threaded(t_camera *cam, t_hittable *world, int *buffer)
{
	pthread_t		threads[THREAD_COUNT];
	t_render_ctx	ctx[THREAD_COUNT];
	int				i;

	init_thread_ctx(ctx, cam, world, buffer);
	i = 0;
	while (i < THREAD_COUNT)
	{
		pthread_create(&threads[i], NULL, render_worker, &ctx[i]);
		i++;
	}
	i = 0;
	while (i < THREAD_COUNT)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}