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
	
	/* 1. Parseamos el Origen hacia 'center' y 'lookfrom' */
	if (!parse_vec3(tokens[1], &scene->camera.center))
		return (0);
	scene->camera.lookfrom = scene->camera.center;
	
	/* 2. Parseamos el Vector Dirección temporalmente */
	if (!parse_vec3(tokens[2], &dir))
		return (0);
	if (!is_normalized(dir))
		return (0);
		
	/* Calculamos 'lookat' sumando la dirección al origen */
	scene->camera.lookat = vec3_add(scene->camera.center, dir);
	
	/* 3. Parseamos el FOV hacia 'vfov' */
	scene->camera.vfov = ft_atof(tokens[3]);
	if (scene->camera.vfov < 0.0 || scene->camera.vfov > 180.0)
		return (0);
		
	return (1);
}

int	parse_light(char **tokens, t_scene *scene)
{
	t_light	*new_light;

	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (0);
	new_light = malloc(sizeof(t_light));
	if (!new_light)
		return (0);
	if (!parse_vec3(tokens[1], &new_light->position) || \
		!parse_color(tokens[3], &new_light->color))
	{
		free(new_light);
		return (0);
	}
	new_light->brightness = ft_atof(tokens[2]);
	if (new_light->brightness < 0.0 || new_light->brightness > 1.0)
		return (free(new_light), 0);
	new_light->next = NULL;
	ligth_add_back(&scene->lights, new_light);
	return (1);
}