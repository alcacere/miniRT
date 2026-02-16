#include "minirt.h"


// parse the ambient values, the spected values are: A [ratio] [RGB];
// and save the values in the scene struct;
void	parse_ambient(char **tokens, t_scene *scene)
{
	if (!tokens[1] || !tokens[2])
	{
		// ERROR HANDLER;
		return ;
	}
	scene->ambient.ratio = ft_atof(tokens[1]);
	scene->ambient.color = parse_color(tokens[2]);
}

// parse the main camera, the spected format is: C [X, Y, Z pos]
// [X, Y, Z dir] [FOV]; the direction vector has to be normalized by default;
void	parse_camera(char **tokens, t_scene *scene)
{
	if (!tokens[1] || !tokens[2] || !tokens[3])
	{
		// error HANDLER
		return ;
	}
	scene->camera.origin = parse_vec(tokens[1]);
	scene->camera.direction = vec3_normalize(parse_vec3(tokens[2]));
	scene->camera.fov = ft_atof(tokens[3]);
}

// parse light creates a node and add it to a list.
// the spected format is: L [X, Y, Z pos] [brightness] [RGB];
void	parse_light(char **tokens, t_scene *scene)
{
	t_ligth	new_light;
	if (!tokens[1] || !tokens[2] || !tokens[3])
		return ;
	new_light = (t_ligth *)malloc(sizeof(t_ligth));
	if (!new_light)
		return ;
	new_light->position = parse_vec3(tokens[1]);
	new_light->brightness = ft_atof(tokens[2]);
	new_light->color = parse_color(tokens[3]);
	new_light->next = NULL;
	ft_lstadd_back((t_list *)&scene->new_lights, (t_list *)new_ligth);
}
