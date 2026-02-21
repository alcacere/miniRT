#include "parse.h"
#include <stdlib.h>

static void	apply_material(t_object *obj, char *token)
{
	if (!token)
		return ;
	if (ft_strncmp(token, "checker", 8) == 0)
		obj->material.is_checkerboard = 1;
	else if (ft_strncmp(token, "metal", 6) == 0)
	{
		obj->material.type = MAT_METAL;
		obj->material.fuzz = 0.05;
	}
	else if (ft_strncmp(token, "glass", 6) == 0)
	{
		obj->material.type = MAT_DIELECTRIC;
		obj->material.ir = 1.5;
	}
}

static t_object	*create_base_object(t_obj_type type, t_color color)
{
	t_object	*obj;

	obj = malloc(sizeof(t_object));
	if (!obj)
		return (NULL);
	obj->type = type;
	obj->material.color = color;
	obj->material.type = MAT_LAMBERTIAN;
	obj->material.fuzz = 0.0;
	obj->material.ir = 1.5;
	obj->material.is_checkerboard = 0;
	obj->shape = NULL;
	obj->next = NULL;
	return (obj);
}

int	parse_sphere(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_sphere	*sp;
	t_color		color;

	if (!tokens[1] || !tokens[2] || !tokens[3])
		return (0);
	if (!parse_color(tokens[3], &color))
		return (0);
	obj = create_base_object(OBJ_SPHERE, color);
	sp = malloc(sizeof(t_sphere));
	if (!obj || !sp || !parse_vec3(tokens[1], &sp->center))
		return (free(obj), free(sp), 0);
	apply_material(obj, tokens[4]);
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

	if (!tokens[1] || !tokens[2] || !tokens[3])
		return (0);
	if (!parse_color(tokens[3], &color))
		return (0);
	obj = create_base_object(OBJ_PLANE, color);
	pl = malloc(sizeof(t_plane));
	if (!obj || !pl || !parse_vec3(tokens[1], &pl->point) || \
		!parse_vec3(tokens[2], &pl->normal) || !is_normalized(pl->normal))
		return (free(obj), free(pl), 0);
	apply_material(obj, tokens[4]);
	obj->shape = pl;
	object_add_back(&scene->objects, obj);
	return (1);
}

int	parse_cylinder(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_cylinder	*cy;
	t_color		color;

	if (!tokens[1] || !tokens[2] || !tokens[3] || !tokens[4] || !tokens[5])
		return (0);
	if (!parse_color(tokens[5], &color))
		return (0);
	obj = create_base_object(OBJ_CYLINDER, color);
	cy = malloc(sizeof(t_cylinder));
	if (!obj || !cy || !parse_vec3(tokens[1], &cy->center) || \
		!parse_vec3(tokens[2], &cy->axis))
		return (free(obj), free(cy), 0);
	apply_material(obj, tokens[6]);
	cy->axis = vec3_normalize(cy->axis);
	cy->radius = ft_atof(tokens[3]) / 2.0;
	cy->height = ft_atof(tokens[4]);
	obj->shape = cy;
	object_add_back(&scene->objects, obj);
	return (1);
}