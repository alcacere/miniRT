/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scatter.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:20:49 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:20:50 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "texture.h"
#include <math.h>

static int	scatter_lambertian(t_hit_record *rec, t_ray *scattered,
			uint32_t *seed)
{
	t_vec3	dir;

	dir = vec3_add(rec->normal, random_unit_vector(seed));
	if (vec3_length(dir) < 1e-8)
		dir = rec->normal;
	*scattered = ray_create(rec->p, dir, 0);
	return (1);
}

static int	scatter_metal(t_ray rays[2], t_hit_record *rec, uint32_t *seed)
{
	t_vec3	dir;
	t_vec3	fuzz;

	dir = reflect(vec3_normalize(rays[0].direction), rec->normal);
	fuzz = vec3_scale(random_unit_vector(seed), rec->mat->fuzz);
	dir = vec3_add(dir, fuzz);
	rays[1] = ray_create(rec->p, dir, 0);
	return (vec3_dot(rays[1].direction, rec->normal) > 0);
}

static int	scatter_dielectric(t_ray rays[2], t_hit_record *rec,
			t_color *att, uint32_t *seed)
{
	double	ref_ratio;
	double	cos_t;
	double	sin_t;
	t_vec3	dir;

	*att = vec3_create(1.0, 1.0, 1.0);
	if (rec->front_face)
		ref_ratio = 1.0 / rec->mat->ir;
	else
		ref_ratio = rec->mat->ir;
	dir = vec3_normalize(rays[0].direction);
	cos_t = fmin(vec3_dot(vec3_scale(dir, -1.0), rec->normal), 1.0);
	sin_t = sqrt(1.0 - cos_t * cos_t);
	if (ref_ratio * sin_t > 1.0)
		dir = reflect(dir, rec->normal);
	else if (reflectance(cos_t, ref_ratio) > random_double(seed))
		dir = reflect(dir, rec->normal);
	else
		dir = refract(dir, rec->normal, ref_ratio);
	rays[1] = ray_create(rec->p, dir, 0);
	return (1);
}

int	scatter(t_ray rays[2], t_hit_record *rec, t_color *att, uint32_t *seed)
{
	if (rec->mat->is_bumpmap)
		apply_bumpmap(rec->p, &rec->normal);
	if (rec->mat->is_checkerboard)
		*att = apply_checkerboard(rec->p, rec->mat->color);
	else
		*att = rec->mat->color;
	if (rec->mat->type == MAT_LAMBERTIAN)
		return (scatter_lambertian(rec, &rays[1], seed));
	if (rec->mat->type == MAT_METAL)
		return (scatter_metal(rays, rec, seed));
	if (rec->mat->type == MAT_DIELECTRIC)
		return (scatter_dielectric(rays, rec, att, seed));
	return (0);
}
