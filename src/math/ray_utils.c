/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:40:52 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:41:30 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "hittable.h"

t_interval	interval_create(double min, double max)
{
	t_interval	i;

	i.min = min;
	i.max = max;
	return (i);
}

t_ray	ray_create(t_point3 origin, t_vec3 direction, double tm)
{
	t_ray	r;

	r.origin = origin;
	r.direction = direction;
	r.inv_dir.x = 1.0 / direction.x;
	r.inv_dir.y = 1.0 / direction.y;
	r.inv_dir.z = 1.0 / direction.z;
	(void)tm;
	return (r);
}

void	set_face_normal(t_hit_record *hit, const t_ray *r,
			const t_vec3 *outward_normal)
{
	hit->front_face = vec3_dot(r->direction, *outward_normal) < 0;
	if (hit->front_face)
		hit->normal = *outward_normal;
	else
		hit->normal = vec3_scale(*outward_normal, -1.0);
}
