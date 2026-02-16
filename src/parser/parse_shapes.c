#include "minirt.h"

// init base material; the choosen default texture is a mate with the given
// color;
static t_material	get_base_mat(t_color color)
{
	t_material	mat;

	mat.color = color;
	mat.difuse = 1.0;
	mat.specular = 0.0;
	mat.shininess = 0.0;
	mat.is_checkerboard = 0;
	return (mat);
}

static void	parse_sphere(char **tokens, t_scene *scene)
{
	t_point3	center;
	double		radius;
	t_material	mat;
	t_object	obj;

	if (!tokens[1] || !tokens[2] || !tokens[3])
		return ;
	center = parse_vec3(tokens[1]);
	radius = ft_atof(tokens[2]) / 2.0;
	mat = get_base_mat(parse_color(tokens[3]));
	obj = obj_new_sphere(center, radius, mat);
	obj_add_back(&scene->objects, obj);
}

static void	parse_plane(t_point **tokens, t_scene *scene)
{
	t_point3	point;
	t_normal	normal;
	t_material	mat;
	t_object	*obj;

	if (!tokens[1] || !tokens[2] || !tokens[3])
		return ;
	point = parse_vec3(tokens[1]);
	normal = parse_vec3(tokens[2]);
	mat = get_base_mat(parse_color(tokens[3]));
	obj = obj_new_plane(point, normal, mat);
	obj_add_back(&scene->objects, obj);
}

static void	parse_cylinder(char **tokens, t_scene *scene)
{
	t_point3	center;
	t_normal	axis;
	double		dims[2];
	t_material	mat;
	t_object	*obj;

	if (!tokens[1] || !tokens[2] || !tokens[3] || !tokens[4] || !tokens[5])
		return ;
	center = parse_vec3(tokens[1]);
	axis = tokens[2];
	dims[0] = ft_atof(tokens[3]);
	dims[1] = ft_atof(tokens[4]);
	mat = get_base_mat(tokens[5]);
	obj = obj_new_cylinder(center, axis, dims, mat);
	obj_add_back(&scane->objects, obj);
}

void	parse_shape(char **tokens, t_scene *scene)
{
	if (ft_strncmp(tokens[0], "sp", 3) == 0)
		parse_sphere(tokens, scene);
	else if (ft_strncmp(tokens[0], "pl", 3) == 0)
		parse_sphere(tokens, scene);
	else if (ft_strncmp(tokens[0], "cy", 3) == 0)
		parse_cylinder(tokens, scene);
}
