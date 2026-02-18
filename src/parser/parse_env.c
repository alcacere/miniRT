#include "parse.h"
#include <stdlib.h>

int	parse_ambient(char **tokens, t_scene *scene)
{
	if (!tokens[1] || !tokens[2] || tokens[3])
		return (0);
	if (!is_valid_double(tokens[1]))
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
	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (0);
	if (!parse_vec3(tokens[1], &scene->camera.origin))
		return (0);
	if (!parse_vec3(tokens[2], &scene->camera.direction))
		return (0);
	if (!is_normalized(scene->camera.direction))
		return (0);
	if (!is_valid_double(tokens[1]))
		return (0);
	scene->camera.fov = ft_atof(tokens[3]);
	if (scene->camera.fov < 0.0 || scene->camera.fov > 180.0)
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
	if (!is_valid_double(tokens[2]))
		return (0);
	new_light->brightness = ft_atof(tokens[2]);
	if (new_light->brightness < 0.0 || new_light->brightness > 1.0)
		return (free(new_light), 0);
	new_light->next = NULL;
	ligth_add_back(&scene->lights, new_light);
	return (1);
}