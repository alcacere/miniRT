#include "minirt.h"

static t_object	*create_base_node(t_obj_type type, t_material mat)
{
	t_object	*node;

	node = (t_object *)malloc(sizeof(t_object));
	if (!node)
		return (NULL);
	node->type = type;
	node->material = mat;
	node->next = NULL;
	return(node);
}

t_object	*create_sphere(t_point3 center, double radius, t_material mat)
{
	t_object	*node;
	t_sphere	*sphere;

	node = create_base_node(OBJ_SPHERE, mat);
	if (!node)
		return (NULL);
	sphere = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sphere)
		return (free(node), NULL);
	sphere->center = center;
	sphere->radius = radius;
	node->shape = (void *)sphere;
	return (node);
}

t_object	*create_pane(t_point3 point, t_normal normal, t_material mat)
{
	t_object	*node;
	t_plane		*plane;

	node = create_base_node(OBJ_PLANE, mat);
	if (!node)
		return (NULL);
	plane = (t_plane *)malloc(sizeof(t_plane));
	if (!plane)
		return (free(node), NULL);
	plane->point = point;
	plane->normal = normal;
	node->shape = (void *)plane;
}

t_object	*create_cylinder(t_point3 center, t_normal axis,
		double dims[2], t_material mat)
{
	t_object	*node;
	t_cylinder	*cy;

	node = create_base_node(OBJ_CYLINDER, mat);
	if (!node)
		return (NULL);
	cy = (t_cylinder *)malloc(sizeof(t_cylinder));
	if (!cy)
		return (free(node), NULL);
	cy->center = center;
	cy->radius = dims[0];
	cy->height = dims[1];
	node->shape = (void *)cy;
	return (node);
}
