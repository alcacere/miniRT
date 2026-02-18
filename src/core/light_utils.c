#include "light.h"
#include <math.h>

t_vec3	reflect_vector(t_vec3 v, t_vec3 n)
{
	double	dot_vn;
	t_vec3	scaled_n;

	dot_vn = vec3_dot(v, n);
	scaled_n = vec3_scale(n, 2.0 * dot_vn);
	return (vec3_sub(v, scaled_n));
}

int	is_in_shadow(t_hittable *world, t_point3 hit_p, t_light *light)
{
	t_ray			shadow_ray;
	t_vec3			light_dir;
	double			light_dist;
	t_hit_record	temp_rec;

	light_dir = vec3_sub(light->position, hit_p);
	light_dist = vec3_length(light_dir);
	shadow_ray.origin = hit_p;
	shadow_ray.direction = vec3_normalize(light_dir);
	if (world->hit(world->object, &shadow_ray, \
		interval_create(0.001, light_dist), &temp_rec))
		return (1);
	return (0);
}