/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_ray.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:19:41 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:19:43 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "graphics.h"

static t_vec3	sample_square(int s_i, int s_j, double recip)
{
	double	px;
	double	py;

	px = ((s_i + 0.5) * recip) - 0.5;
	py = ((s_j + 0.5) * recip) - 0.5;
	return (vec3_create(px, py, 0));
}

t_ray	get_ray_stratified(t_camera *c, int coords[2], int s_coords[2])
{
	t_vec3	offset;
	t_vec3	pixel_sample;
	t_vec3	ray_dir;
	t_vec3	tmp;

	offset = sample_square(s_coords[0], s_coords[1], c->recip_sqrt_spp);
	tmp = vec3_scale(c->pixel_delta_u, coords[0] + offset.x);
	pixel_sample = vec3_add(c->pixel00_loc, tmp);
	tmp = vec3_scale(c->pixel_delta_v, coords[1] + offset.y);
	pixel_sample = vec3_add(pixel_sample, tmp);
	ray_dir = vec3_sub(pixel_sample, c->center);
	return (ray_create(c->center, ray_dir, 0.0));
}

t_vec3	ray_color(t_ray *r, t_render_ctx *ctx, int depth, uint32_t *seed)
{
	t_hit_record	rec;
	t_ray			rays[2];
	t_color			att;
	t_vec3			bounced;

	if (depth <= 0)
		return (vec3_create(0, 0, 0));
	if (ctx->world->hit(ctx->world->object, r,
			interval_create(0.001, INFINITY), &rec))
	{
		if (rec.mat->type == MAT_EMISSION)
			return (vec3_scale(rec.mat->color, rec.mat->emit_strength));
		rays[0] = *r;
		if (scatter(rays, &rec, &att, seed))
		{
			bounced = ray_color(&rays[1], ctx, depth - 1, seed);
			return (vec3_mul(att, bounced));
		}
		return (vec3_create(0, 0, 0));
	}
	return (vec3_scale(ctx->scene->ambient.color, ctx->scene->ambient.ratio));
}
