#include "parse.h"
#include <stdlib.h>

int	parse_ambient(char **tokens, t_scene *scene)
{
	if (!tokens[1] || !tokens[2] || tokens[3])
		return (0);
	scene->ambient.ratio = ft_atof(tokens[1]);
	if (scene->ambient.ratio < 0.0 || scene->ambient.ratio > 1.0)
		return (0);
	if (!parse_color(tokens[2], &scene->ambient.color))
		return (0);
	return (1);
}

int	parse_camera(char **tokens, t_scene *scene)
{
	t_vec3	dir;

	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (0);
	if (!parse_vec3(tokens[1], &scene->camera.center))
		return (0);
	scene->camera.lookfrom = scene->camera.center;
	if (!parse_vec3(tokens[2], &dir))
		return (0);
	if (!is_normalized(dir))
		return (0);
	scene->camera.lookat = vec3_add(scene->camera.center, dir);
	scene->camera.vfov = ft_atof(tokens[3]);
	if (scene->camera.vfov < 0.0 || scene->camera.vfov > 180.0)
		return (0);
	return (1);
}

int	parse_light(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_sphere	*sp;
	double		brightness;

	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (0);
	brightness = ft_atof(tokens[2]);
	if (brightness < 0.0 || brightness > 1.0)
		return (0);
	obj = malloc(sizeof(t_object));
	sp = malloc(sizeof(t_sphere));
	if (!obj || !sp || !parse_vec3(tokens[1], &sp->center) || \
		!parse_color(tokens[3], &obj->material.color))
		return (free(obj), free(sp), 0);
	obj->type = OBJ_SPHERE;
	obj->material.type = MAT_EMISSION;
	obj->material.emit_strength = brightness * 20.0;
	obj->material.is_checkerboard = 0;
	sp->radius = 1.0; 
	obj->shape = sp;
	obj->next = NULL;
	object_add_back(&scene->objects, obj);
	return (1);
}