#include "camera.h"
#include "hittable.h"
#include "random.h"

/*
  Extraemos la lógica del cielo/fondo para mantener 'ray_color' bajo 25 líneas.
  Esta función imita el Lerp entre blanco y el color de fondo original.
 */
static t_vec3	get_background_color(const t_ray *r, t_color bg)
{
	t_vec3	unit_dir;
	double	a;
	t_vec3	white;

	if (bg.x < 0.01 && bg.y < 0.01 && bg.z < 0.01)
		return (vec3_create(0, 0, 0));
	unit_dir = vec3_normalize(r->dir);
	a = 0.5 * (unit_dir.y + 1.0);
	white = vec3_create(1.0, 1.0, 1.0);
	return (vec3_add(vec3_scale(white, 1.0 - a), vec3_scale(bg, a)));
}

/*
  Función recursiva de Monte Carlo.
  Evalúa colisiones, calcula emisión de luz y lanza rayos difusos/especulares.
 */
t_vec3	ray_color(t_ray *r, t_hittable *world, int depth, t_color bg)
{
	t_hit_record	rec;
	t_ray			scattered;
	t_color			attenuation;
	t_color			emission;

	if (depth <= 0)
		return (vec3_create(0, 0, 0));
	if (!world->hit(world->object, r, interval_create(0.001, INFINITY), &rec))
		return (get_background_color(r, bg));
	emission = vec3_create(0, 0, 0);
	if (rec.mat && rec.mat->emitted)
		emission = rec.mat->emitted(rec.mat, rec.u, rec.v, &rec.p);
	if (rec.mat && rec.mat->scatter(rec.mat, r, &rec, &attenuation, &scattered))
	{
		return (vec3_add(emission, vec3_mul(attenuation, 
			ray_color(&scattered, world, depth - 1, bg))));
	}
	return (emission);
}

/*
  coords[0] = i, coords[1] = j
  s_coords[0] = s_i, s_coords[1] = s_j
  Agrupados para no superar los 4 argumentos por función de La Norma.
 */
t_ray	get_ray_stratified(t_camera *c, int coords[2], int s_coords[2])
{
	t_vec3	offset;
	t_vec3	pixel_sample;
	t_vec3	ray_origin;
	t_vec3	ray_dir;
	t_vec3	tmp;

	offset = sample_square_stratified(s_coords[0], s_coords[1], 
		c->recip_sqrt_spp);
	tmp = vec3_scale(c->pixel_delta_u, coords[0] + offset.x);
	pixel_sample = vec3_add(c->pixel00_loc, tmp);
	tmp = vec3_scale(c->pixel_delta_v, coords[1] + offset.y);
	pixel_sample = vec3_add(pixel_sample, tmp);
	if (c->defocus_angle <= 0.0)
		ray_origin = c->center;
	else
		ray_origin = defocus_disk_sample(c);
	ray_dir = vec3_sub(pixel_sample, ray_origin);
	return (ray_create(ray_origin, ray_dir, random_real()));
}