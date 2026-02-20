#include "camera.h"
#include "hittable.h"
#include "light.h"

static t_vec3	sample_square(int s_i, int s_j, double recip)
{
	double	px;
	double	py;

	px = ((s_i + 0.5) * recip) - 0.5;
	py = ((s_j + 0.5) * recip) - 0.5;
	return (vec3_create(px, py, 0));
}

t_ray	get_ray_stratified(t_camera *c, int coords[2], int s_coords[2])
{
	t_vec3	offset;
	t_vec3	pixel_sample;
	t_vec3	ray_dir;
	t_vec3	tmp;

	offset = sample_square(s_coords[0], s_coords[1], c->recip_sqrt_spp);
	tmp = vec3_scale(c->pixel_delta_u, coords[0] + offset.x);
	pixel_sample = vec3_add(c->pixel00_loc, tmp);
	tmp = vec3_scale(c->pixel_delta_v, coords[1] + offset.y);
	pixel_sample = vec3_add(pixel_sample, tmp);
	
	ray_dir = vec3_sub(pixel_sample, c->center);
	return (ray_create(c->center, ray_dir, 0.0));
}

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