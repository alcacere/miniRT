#include "light.h"
#include "texture.h"
#include <math.h>

static t_color	calc_ambient(t_scene *scene, t_color obj_color)
{
	t_color	ambient;

	ambient = vec3_scale(scene->ambient.color, scene->ambient.ratio);
	return (vec3_mul(ambient, obj_color));
}

static t_color	calc_point_light(t_light *l, t_hit_record *rec, t_ray *cam_r)
{
	t_vec3	light_dir;
	t_vec3	view_dir;
	t_vec3	reflect_dir;
	double	factors[2];
	t_color	result;

	light_dir = vec3_normalize(vec3_sub(l->position, rec->p));
	view_dir = vec3_normalize(vec3_scale(cam_r->direction, -1.0));
	factors[0] = fmax(vec3_dot(rec->normal, light_dir), 0.0);
	reflect_dir = reflect_vector(vec3_scale(light_dir, -1.0), rec->normal);
	factors[1] = pow(fmax(vec3_dot(view_dir, reflect_dir), 0.0), 
		rec->mat->shininess);
	result = vec3_scale(l->color, l->brightness * factors[0] * rec->mat->diffuse);
	result = vec3_add(result, vec3_scale(l->color, 
		l->brightness * factors[1] * rec->mat->specular));
	return (result);
}

t_color	calculate_lighting(t_scene *scene, t_hittable *world,
							t_hit_record *rec, t_ray *cam_ray)
{
	t_color	final_color;
	t_color	light_contrib;
	t_color	albedo;
	t_light	*current_light;

	albedo = rec->mat->color;
	if (rec->mat->is_checkerboard)
		albedo = apply_checkerboard(rec->p, albedo);
	final_color = calc_ambient(scene, albedo);
	current_light = scene->lights;
	while (current_light)
	{
		if (!is_in_shadow(world, rec->p, current_light))
		{
			light_contrib = calc_point_light(current_light, rec, cam_ray);
			light_contrib = vec3_mul(light_contrib, albedo);
			final_color = vec3_add(final_color, light_contrib);
		}
		current_light = current_light->next;
	}
	
	final_color.x = fmin(final_color.x, 1.0);
	final_color.y = fmin(final_color.y, 1.0);
	final_color.z = fmin(final_color.z, 1.0);
	return (final_color);
}