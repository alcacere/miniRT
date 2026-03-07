#include "objects.h"
#include <stdlib.h>
#include <math.h>

static double	find_nearest_root(t_sphere *sp, const t_ray *r, t_interval rayt)
{
	t_vec3	oc;
	double	abc[3];
	double	disc_root[2];

	oc = vec3_sub(r->origin, sp->center);
	abc[0] = vec3_dot(r->direction, r->direction);
	abc[1] = vec3_dot(oc, r->direction);
	abc[2] = vec3_dot(oc, oc) - (sp->radius * sp->radius);
	disc_root[0] = (abc[1] * abc[1]) - (abc[0] * abc[2]);
	if (disc_root[0] < 0)
		return (-1.0);
	disc_root[1] = (-abc[1] - sqrt(disc_root[0])) / abc[0];
	if (disc_root[1] <= rayt.min || disc_root[1] >= rayt.max)
	{
		disc_root[1] = (-abc[1] + sqrt(disc_root[0])) / abc[0];
		if (disc_root[1] <= rayt.min || disc_root[1] >= rayt.max)
			return (-1.0);
	}
	return (disc_root[1]);
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
