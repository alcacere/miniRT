/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 15:45:31 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 15:45:32 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "objects.h"
#include <stdlib.h>
#include <math.h>

static int	check_cap(t_cone *co, const t_ray *r, t_interval rayt,
						t_hit_record *rec)
{
	double	denom;
	double	t;
	t_vec3	oc;
	t_vec3	p;

	denom = vec3_dot(r->direction, co->axis);
	if (fabs(denom) < 1e-8)
		return (0);
	oc = vec3_sub(co->center, r->origin);
	t = vec3_dot(oc, co->axis) / denom;
	if (t <= rayt.min || t >= rayt.max)
		return (0);
	p = vec3_add(r->origin, vec3_scale(r->direction, t));
	oc = vec3_sub(p, co->center);
	if (vec3_dot(oc, oc) > co->radius * co->radius)
		return (0);
	rec->t = t;
	rec->p = p;
	oc = vec3_scale(co->axis, -1.0);
	set_face_normal(rec, r, &oc);
	return (1);
}

static int	check_root(t_cone *co, t_cone_data *d, const t_ray *r,
						t_hit_record *rec)
{
	t_vec3	p;
	t_vec3	cp;
	t_vec3	normal;

	if (d->root <= d->t_min || d->root >= d->t_max)
		return (0);
	p = vec3_add(r->origin, vec3_scale(r->direction, d->root));
	cp = vec3_sub(p, d->tip);
	d->m = vec3_dot(cp, d->axis);
	if (d->m < 0.0 || d->m > co->height)
		return (0);
	rec->t = d->root;
	rec->p = p;
	normal = vec3_normalize(vec3_sub(cp, vec3_scale(d->axis, d->m * d->k2)));
	set_face_normal(rec, r, &normal);
	return (1);
}

static void	get_roots(t_cone *co, t_cone_data *d, const t_ray *r)
{
	t_vec3	oc;

	d->tip = vec3_add(co->center, vec3_scale(co->axis, co->height));
	d->axis = vec3_scale(co->axis, -1.0);
	d->k2 = (co->radius / co->height) * (co->radius / co->height) + 1.0;
	oc = vec3_sub(r->origin, d->tip);
	d->a = vec3_dot(r->direction, r->direction) - d->k2
		* pow(vec3_dot(r->direction, d->axis), 2);
	d->half_b = vec3_dot(r->direction, oc) - d->k2
		* vec3_dot(r->direction, d->axis) * vec3_dot(oc, d->axis);
	d->c = vec3_dot(oc, oc) - d->k2 * pow(vec3_dot(oc, d->axis), 2);
	d->r1 = INFINITY;
	d->r2 = INFINITY;
	if (fabs(d->a) < 1e-8 && fabs(d->half_b) > 1e-8)
		d->r1 = -d->c / (2.0 * d->half_b);
	else if (fabs(d->a) >= 1e-8)
	{
		d->disc = d->half_b * d->half_b - d->a * d->c;
		if (d->disc >= 0.0)
		{
			d->r1 = (-d->half_b - sqrt(d->disc)) / d->a;
			d->r2 = (-d->half_b + sqrt(d->disc)) / d->a;
		}
	}
}

int	hit_cone(const void *obj, const t_ray *r, t_interval rayt,
				t_hit_record *rec)
{
	t_cone_data	d;
	int			hit;

	hit = check_cap(((t_object *)obj)->shape, r, rayt, rec);
	d.t_min = rayt.min;
	d.t_max = rayt.max;
	if (hit)
		d.t_max = rec->t;
	get_roots(((t_object *)obj)->shape, &d, r);
	d.root = d.r1;
	if (check_root(((t_object *)obj)->shape, &d, r, rec))
	{
		hit = 1;
		d.t_max = rec->t;
	}
	d.root = d.r2;
	if (check_root(((t_object *)obj)->shape, &d, r, rec))
		hit = 1;
	if (hit)
		rec->mat = &((t_object *)obj)->material;
	return (hit);
}

t_hittable	*create_hittable_cone(t_object *obj)
{
	t_hittable	*node;
	t_cone		*co;
	t_aabb		b_box;
	t_aabb		t_box;
	t_vec3		rad;

	node = malloc(sizeof(t_hittable));
	if (!node)
		return (NULL);
	co = (t_cone *)obj->shape;
	node->object = obj;
	node->hit = hit_cone;
	t_box.min = vec3_add(co->center, vec3_scale(co->axis, co->height));
	t_box.max = t_box.min;
	rad = vec3_create(co->radius, co->radius, co->radius);
	b_box.min = vec3_sub(co->center, rad);
	b_box.max = vec3_add(co->center, rad);
	node->bbox = aabb_merge(b_box, t_box);
	aabb_pad(&node->bbox);
	return (node);
}
