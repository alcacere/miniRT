#include "objects.h"
#include <stdlib.h>

typedef struct s_mt_data
{
	t_vec3	edge1;
	t_vec3	edge2;
	t_vec3	h;
	t_vec3	s;
	t_vec3	q;
	double	a;
	double	f;
	double	u;
	double	v;
}	t_mt_data;

int	hit_triangle(const void *object, const t_ray *r, t_interval rayt, \
					t_hit_record *rec)
{
	t_object	*obj_node;
	t_triangle	*tr;
	t_mt_data	d;
	double		t;
	t_vec3		normal;

	obj_node = (t_object *)object;
	tr = (t_triangle *)obj_node->shape;
	d.edge1 = vec3_sub(tr->b, tr->a);
	d.edge2 = vec3_sub(tr->c, tr->a);
	d.h = vec3_cross(r->direction, d.edge2);
	d.a = vec3_dot(d.edge1, d.h);
	if (d.a > -1e-8 && d.a < 1e-8)
		return (0);
	d.f = 1.0 / d.a;
	d.s = vec3_sub(r->origin, tr->a);
	d.u = d.f * vec3_dot(d.s, d.h);
	if (d.u < 0.0 || d.u > 1.0)
		return (0);
	d.q = vec3_cross(d.s, d.edge1);
	d.v = d.f * vec3_dot(r->direction, d.q);
	if (d.v < 0.0 || d.u + d.v > 1.0)
		return (0);
	t = d.f * vec3_dot(d.edge2, d.q);
	if (t <= rayt.min || t >= rayt.max)
		return (0);
	rec->t = t;
	rec->p = vec3_add(r->origin, vec3_scale(r->direction, rec->t));
	normal = vec3_normalize(vec3_cross(d.edge1, d.edge2));
	set_face_normal(rec, r, &normal);
	rec->mat = &obj_node->material;
	return (1);
}

t_hittable	*create_hittable_triangle(t_object *obj)
{
	t_hittable	*node;

	node = malloc(sizeof(t_hittable));
	if (!node)
		return (NULL);
	node->object = obj;
	node->hit = hit_triangle;
	return (node);
}