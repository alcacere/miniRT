/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_ray.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:19:41 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 19:33:51 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "graphics.h"
#include <math.h>

static t_vec3	sample_square(int s_i, int s_j, double recip)
{
	double	px;
	double	py;

	px = ((s_i + 0.5) * recip) - 0.5;
	py = ((s_j + 0.5) * recip) - 0.5;
	return (vec3_create(px, py, 0));
}

/*
** Direct light sampling: cast a shadow ray from the hit point toward
** one light source. If nothing blocks the path, add the light's
** contribution using Lambert's cosine law and 1/dist falloff
** (softened from physical 1/dist² to avoid overly dark scenes).
*/
static t_color	sample_one_light(t_hit_record *rec, t_object *light,
		t_render_ctx *ctx)
{
	t_vec3			to_light;
	double			dist;
	double			nl;
	t_ray			shadow;
	t_hit_record	tmp;
	t_sphere		*sp;

	sp = (t_sphere *)light->shape;
	to_light = vec3_sub(sp->center, rec->p);
	dist = vec3_length(to_light);
	if (dist < 1e-6)
		return (vec3_create(0, 0, 0));
	to_light = vec3_scale(to_light, 1.0 / dist);
	nl = vec3_dot(rec->normal, to_light);
	if (nl <= 0.0)
		return (vec3_create(0, 0, 0));
	shadow = ray_create(rec->p, to_light, 0);
	if (ctx->world->hit(ctx->world->object, &shadow,
			interval_create(0.001, dist - sp->radius - 0.01), &tmp))
		return (vec3_create(0, 0, 0));
	return (vec3_scale(light->material.color,
			light->material.emit_strength * nl / dist));
}

/*
** Iterate all scene objects; for each emissive light, accumulate
** its direct contribution at the given hit point.
*/
static t_color	sample_direct_lights(t_hit_record *rec, t_render_ctx *ctx)
{
	t_color		total;
	t_object	*obj;

	total = vec3_create(0, 0, 0);
	obj = ctx->scene->objects;
	while (obj)
	{
		if (obj->material.type == MAT_EMISSION)
			total = vec3_add(total, sample_one_light(rec, obj, ctx));
		obj = obj->next;
	}
	return (total);
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

/*
** ray_color: trace a ray into the scene.
** On hit: compute direct light (shadow rays) + indirect light (bounce).
** On miss: return ambient background.
*/
t_vec3	ray_color(t_ray *r, t_render_ctx *ctx, int depth, uint32_t *seed)
{
	t_hit_record	rec;
	t_ray			rays[2];
	t_color			att;
	t_vec3			result;

	if (depth <= 0)
		return (vec3_create(0, 0, 0));
	if (!ctx->world->hit(ctx->world->object, r,
			interval_create(0.001, INFINITY), &rec))
		return (vec3_scale(ctx->scene->ambient.color,
				ctx->scene->ambient.ratio));
	if (rec.mat->type == MAT_EMISSION)
		return (vec3_scale(rec.mat->color, rec.mat->emit_strength));
	rays[0] = *r;
	if (!scatter(rays, &rec, &att, seed))
		return (vec3_create(0, 0, 0));
	result = sample_direct_lights(&rec, ctx);
	result = vec3_add(result, ray_color(&rays[1], ctx, depth - 1, seed));
	return (vec3_mul(att, result));
}
