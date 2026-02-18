#include "parse.h"
#include <stdlib.h>

static t_object	*create_base_object(t_obj_type type, t_color color)
{
	t_object	*obj;

	obj = malloc(sizeof(t_object));
	if (!obj)
		return (NULL);
	obj->type = type;
	obj->material.color = color;
	obj->material.diffuse = 1.0;
	obj->material.specular = 0.0;
	obj->shape = NULL;
	obj->next = NULL;
	return (obj);
}

int	parse_sphere(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_sphere	*sp;
	t_color		color;

	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (0);
	if (!parse_color(tokens[3], &color))
		return (0);
	obj = create_base_object(OBJ_SPHERE, color);
	sp = malloc(sizeof(t_sphere));
	if (!obj || !sp || !parse_vec3(tokens[1], &sp->center))
	{
		free(obj);
		free(sp);
		return (0);
	}
	sp->radius = ft_atof(tokens[2]) / 2.0;
	obj->shape = sp;
	object_add_back(&scene->objects, obj);
	return (1);
}

int	parse_plane(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_plane		*pl;
	t_color		color;

	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (0);
	if (!parse_color(tokens[3], &color))
		return (0);
	obj = create_base_object(OBJ_PLANE, color);
	pl = malloc(sizeof(t_plane));
	if (!obj || !pl || !parse_vec3(tokens[1], &pl->point) || \
		!parse_vec3(tokens[2], &pl->normal) || !is_normalized(pl->normal))
	{
		free(obj);
		free(pl);
		return (0);
	}
	obj->shape = pl;
	object_add_back(&scene->objects, obj);
	return (1);
}