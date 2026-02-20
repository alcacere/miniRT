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
	(void)tm;
	return (r);
}

void	set_face_normal(t_hit_record *hit, const t_ray *r, \
						const t_vec3 *outward_normal)
{
	hit->front_face = vec3_dot(r->direction, *outward_normal) < 0;
	if (hit->front_face)
		hit->normal = *outward_normal;
	else
		hit->normal = vec3_scale(*outward_normal, -1.0);
}