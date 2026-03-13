/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 15:45:48 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 15:45:49 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "objects.h"
#include <math.h>

int	hit_plane(const void *obj, const t_ray *r,
				t_interval rayt, t_hit_record *rec)
{
	t_object	*plane_obj;
	t_plane		*pl;
	double		denominator;
	double		t;
	t_vec3		oc;

	plane_obj = (t_object *)obj;
	pl = (t_plane *)plane_obj->shape;
	denominator = vec3_dot(r->direction, pl->normal);
	if (fabs(denominator) < 1e-8)
		return (0);
	oc = vec3_sub(pl->point, r->origin);
	t = vec3_dot(oc, pl->normal) / denominator;
	if (t <= rayt.min || t >= rayt.max)
		return (0);
	rec->t = t;
	rec->p = vec3_add(r->origin, vec3_scale(r->direction, rec->t));
	set_face_normal(rec, r, &pl->normal);
	rec->mat = &plane_obj->material;
	return (1);
}

t_hittable	*create_hittable_plane(t_object *obj)
{
	t_hittable	*node;
	t_vec3		huge;

	node = malloc(sizeof(t_hittable));
	if (!node)
		return (NULL);
	node->object = obj;
	node->hit = hit_plane;
	huge = vec3_create(1e5, 1e5, 1e5);
	node->bbox.min = vec3_scale(huge, -1.0);
	node->bbox.max = huge;
	aabb_pad(&node->bbox);
	return (node);
}
