#include "objects.h"
#include <math.h>

int	hit_plane(const void *obj, const t_ray *r, \
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
	t_hittable	*h_pl;

	h_pl = malloc(sizeof(t_hittable));
	if (!h_pl)
		return (NULL);
	h_pl->object = obj;
	h_pl->hit = hit_plane;
	return (h_pl);
}