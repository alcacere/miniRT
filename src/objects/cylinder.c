/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 15:45:42 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 15:45:43 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "objects.h"
#include <math.h>

static double	find_cylinder_root(t_cylinder *cy, const t_ray *r,
	t_interval rayt)
{
	t_vec3	oc;
	double	abc[3];
	double	dots[2];
	double	disc;
	double	t;

	oc = vec3_sub(r->origin, cy->center);
	dots[0] = vec3_dot(r->direction, cy->axis);
	dots[1] = vec3_dot(oc, cy->axis);
	abc[0] = vec3_dot(r->direction, r->direction) - (dots[0] * dots[0]);
	abc[1] = vec3_dot(r->direction, oc) - (dots[0] * dots[1]);
	abc[2] = vec3_dot(oc, oc) - (dots[1] * dots[1]) - (cy->radius * cy->radius);
	disc = (abc[1] * abc[1]) - (abc[0] * abc[2]);
	if (disc < 0)
		return (-1.0);
	t = (-abc[1] - sqrt(disc)) / abc[0];
	if (t > rayt.min && t < rayt.max
		&& fabs(dots[1] + t * dots[0]) <= cy->height / 2.0)
		return (t);
	t = (-abc[1] + sqrt(disc)) / abc[0];
	if (t > rayt.min && t < rayt.max
		&& fabs(dots[1] + t * dots[0]) <= cy->height / 2.0)
		return (t);
	return (-1.0);
}

int	hit_cylinder(const void *obj, const t_ray *r,
	t_interval rayt, t_hit_record *rec)
{
	t_object	*cy_obj;
	t_cylinder	*cy;
	double		m;
	t_vec3		outward_normal;

	cy_obj = (t_object *)obj;
	cy = (t_cylinder *)cy_obj->shape;
	rec->t = find_cylinder_root(cy, r, rayt);
	if (rec->t < 0.0)
		return (0);
	rec->p = vec3_add(r->origin, vec3_scale(r->direction, rec->t));
	m = vec3_dot(vec3_sub(rec->p, cy->center), cy->axis);
	outward_normal = vec3_sub(rec->p,
			vec3_add(cy->center, vec3_scale(cy->axis, m)));
	outward_normal = vec3_scale(outward_normal, 1.0 / cy->radius);
	set_face_normal(rec, r, &outward_normal);
	rec->mat = &cy_obj->material;
	return (1);
}

t_hittable	*create_hittable_cylinder(t_object *obj)
{
	t_hittable	*node;
	t_cylinder	*cy;
	t_vec3		ext;
	double		max_d;

	node = malloc(sizeof(t_hittable));
	if (!node)
		return (NULL);
	cy = (t_cylinder *)obj->shape;
	node->object = obj;
	node->hit = hit_cylinder;
	max_d = sqrt(cy->radius * cy->radius + (cy->height / 2.0)
			* (cy->height / 2.0));
	ext = vec3_create(max_d, max_d, max_d);
	node->bbox.min = vec3_sub(cy->center, ext);
	node->bbox.max = vec3_add(cy->center, ext);
	return (node);
}
