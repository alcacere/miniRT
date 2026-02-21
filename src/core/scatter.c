#include "camera.h"
#include "texture.h"
#include <math.h>

int	scatter(t_ray *r_in, t_hit_record *rec, t_color *attenuation, \
			t_ray *scattered, uint32_t *seed)
{
	t_vec3	dir;
	double	ref_ratio;
	double	cos_t;
	double	sin_t;

	if (rec->mat->is_checkerboard)
		*attenuation = apply_checkerboard(rec->p, rec->mat->color);
	else
		*attenuation = rec->mat->color;
	if (rec->mat->type == MAT_LAMBERTIAN)
	{
		dir = vec3_add(rec->normal, random_unit_vector(seed));
		if (vec3_length(dir) < 1e-8)
			dir = rec->normal;
		*scattered = ray_create(rec->p, dir, 0);
		return (1);
	}
	if (rec->mat->type == MAT_METAL)
	{
		dir = reflect(vec3_normalize(r_in->direction), rec->normal);
		dir = vec3_add(dir, vec3_scale(random_unit_vector(seed), rec->mat->fuzz));
		*scattered = ray_create(rec->p, dir, 0);
		return (vec3_dot(scattered->direction, rec->normal) > 0);
	}
	if (rec->mat->type == MAT_DIELECTRIC)
	{
		*attenuation = vec3_create(1.0, 1.0, 1.0);
		ref_ratio = rec->front_face ? (1.0 / rec->mat->ir) : rec->mat->ir;
		dir = vec3_normalize(r_in->direction);
		cos_t = fmin(vec3_dot(vec3_scale(dir, -1.0), rec->normal), 1.0);
		sin_t = sqrt(1.0 - cos_t * cos_t);
		if (ref_ratio * sin_t > 1.0 || reflectance(cos_t, ref_ratio) > random_double(seed))
			dir = reflect(dir, rec->normal);
		else
			dir = refract(dir, rec->normal, ref_ratio);
		*scattered = ray_create(rec->p, dir, 0);
		return (1);
	}
	return (0);
}