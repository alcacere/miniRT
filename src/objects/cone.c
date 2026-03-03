#include "objects.h"
#include "structures.h"
#include <stdlib.h>
#include <math.h>

typedef struct s_cone_data
{
	t_vec3	tip;
	t_vec3	axis;
	double	k2;
	double	a;
	double	half_b;
	double	c;
	double	m;
}	t_cone_data;

static int	check_cap(t_cone *co, const t_ray *r, t_interval rayt, \
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

static int	check_root(t_cone *co, t_cone_data *d, const t_ray *r, \
						t_interval rayt, t_hit_record *rec, double root)
{
	t_vec3	cp;
	t_vec3	normal;

	if (root <= rayt.min || root >= rayt.max)
		return (0);
	rec->p = vec3_add(r->origin, vec3_scale(r->direction, root));
	cp = vec3_sub(rec->p, d->tip);
	d->m = vec3_dot(cp, d->axis);
	if (d->m < 0.0 || d->m > co->height)
		return (0);
	rec->t = root;
	normal = vec3_normalize(vec3_sub(cp, vec3_scale(d->axis, d->m * d->k2)));
	set_face_normal(rec, r, &normal);
	return (1);
}

int	hit_cone(const void *obj, const t_ray *r, t_interval rayt, \
				t_hit_record *rec)
{
	t_object	*node;
	t_cone		*co;
	t_cone_data	d;
	t_vec3		oc;
	int			hit;

	node = (t_object *)obj;
	co = (t_cone *)node->shape;
	hit = 0;
	if (check_cap(co, r, rayt, rec) && (hit = 1))
		rayt.max = rec->t; 
	d.tip = vec3_add(co->center, vec3_scale(co->axis, co->height));
	d.axis = vec3_scale(co->axis, -1.0);
	d.k2 = (co->radius / co->height) * (co->radius / co->height) + 1.0;
	oc = vec3_sub(r->origin, d.tip);
	d.a = vec3_dot(r->direction, r->direction) - d.k2 * \
			vec3_dot(r->direction, d.axis) * vec3_dot(r->direction, d.axis);
	d.half_b = vec3_dot(r->direction, oc) - d.k2 * \
				vec3_dot(r->direction, d.axis) * vec3_dot(oc, d.axis);
	d.c = vec3_dot(oc, oc) - d.k2 * vec3_dot(oc, d.axis) * vec3_dot(oc, d.axis);
	d.m = d.half_b * d.half_b - d.a * d.c;
	if (d.m >= 0.0)
	{
		if (check_root(co, &d, r, rayt, rec, (-d.half_b - sqrt(d.m)) / d.a) && (hit = 1))
			rayt.max = rec->t;
		if (check_root(co, &d, r, rayt, rec, (-d.half_b + sqrt(d.m)) / d.a))
			hit = 1;
	}
	if (hit)
		rec->mat = &node->material;
	return (hit);
}

t_hittable	*create_hittable_cone(t_object *obj)
{
	t_hittable	*node;
	t_cone		*co;
	t_vec3		tip;
	t_aabb		base_box;
	t_aabb		tip_box;

	node = malloc(sizeof(t_hittable));
	if (!node)
		return (NULL);
	co = (t_cone *)obj->shape;
	node->object = obj;
	node->hit = hit_cone;
	tip = vec3_add(co->center, vec3_scale(co->axis, co->height));
	base_box.min = vec3_sub(co->center, vec3_create(co->radius, co->radius, co->radius));
	base_box.max = vec3_add(co->center, vec3_create(co->radius, co->radius, co->radius));
	tip_box.min = tip;
	tip_box.max = tip;
	node->bbox = aabb_merge(base_box, tip_box);
	aabb_pad(&node->bbox);
	return (node);
}