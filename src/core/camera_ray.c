#include "camera.h"
#include "hittable.h"
#include "light.h"

t_vec3	ray_color(t_ray *r, t_scene *scene, t_hittable *world)
{
	t_hit_record	rec;
	t_vec3			bg_color;
	t_vec3			unit_dir;
	double			a;

	if (world->hit(world->object, r, interval_create(0.001, INFINITY), &rec))
		return (calculate_lighting(scene, world, &rec, r));
	unit_dir = vec3_normalize(r->direction);
	a = 0.5 * (unit_dir.y + 1.0);
	bg_color = vec3_add(vec3_scale(vec3_create(1.0, 1.0, 1.0), 1.0 - a), \
						vec3_scale(scene->camera.background, a));
	return (bg_color);
}