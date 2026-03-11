/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 15:45:54 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 15:45:56 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "objects.h"
#include <stdlib.h>
#include <math.h>

static double	get_valid_root(double t1, double t2, t_interval rayt)
{
	double	min_t;
	double	max_t;

	min_t = fmin(t1, t2);
	max_t = fmax(t1, t2);
	if (min_t > rayt.min && min_t < rayt.max)
		return (min_t);
	if (max_t > rayt.min && max_t < rayt.max)
		return (max_t);
	return (-1.0);
}

static double	find_nearest_root(t_sphere *sp, const t_ray *r, t_interval rayt)
{
	t_vec3	oc;
	double	abc[3];
	double	q[2];
	double	t[2];

	oc = vec3_sub(r->origin, sp->center);
	abc[0] = vec3_dot(r->direction, r->direction);
	abc[1] = vec3_dot(oc, r->direction);
	abc[2] = vec3_dot(oc, oc) - (sp->radius * sp->radius);
	q[0] = (abc[1] * abc[1]) - (abc[0] * abc[2]);
	if (q[0] < 0.0 || fabs(abc[0]) < 1e-8)
		return (-1.0);
	q[1] = -abc[1] - sqrt(q[0]);
	if (abc[1] < 0.0)
		q[1] = -abc[1] + sqrt(q[0]);
	t[0] = q[1] / abc[0];
	t[1] = -1.0;
	if (q[1] != 0.0)
		t[1] = abc[2] / q[1];
	return (get_valid_root(t[0], t[1], rayt));
}

int	hit_sphere(const void *obj, const t_ray *r,
				t_interval rayt, t_hit_record *rec)
{
	t_object	*sphere_obj;
	t_sphere	*sp;
	double		root;
	t_vec3		outward_normal;

	sphere_obj = (t_object *)obj;
	sp = (t_sphere *)sphere_obj->shape;
	root = find_nearest_root(sp, r, rayt);
	if (root < 0.0)
		return (0);
	rec->t = root;
	rec->p = vec3_add(r->origin, vec3_scale(r->direction, rec->t));
	outward_normal = vec3_scale(vec3_sub(rec->p, sp->center), 1.0 / sp->radius);
	set_face_normal(rec, r, &outward_normal);
	rec->mat = &sphere_obj->material;
	return (1);
}

t_hittable	*create_hittable_sphere(t_object *obj)
{
	t_hittable	*node;
	t_sphere	*sp;
	t_vec3		rvec;

	node = malloc(sizeof(t_hittable));
	if (!node)
		return (NULL);
	sp = (t_sphere *)obj->shape;
	node->object = obj;
	node->hit = hit_sphere;
	rvec = vec3_create(sp->radius, sp->radius, sp->radius);
	node->bbox.min = vec3_sub(sp->center, rvec);
	node->bbox.max = vec3_add(sp->center, rvec);
	return (node);
}
