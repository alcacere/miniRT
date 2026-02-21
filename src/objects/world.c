#include "objects.h"
#include <stdlib.h>

static int	count_objects(t_object *head)
{
	int	count;

	count = 0;
	while (head)
	{
		count++;
		head = head->next;
	}
	return (count);
}

static t_hittable	*create_wrapper(t_object *node)
{
	if (node->type == OBJ_SPHERE)
		return (create_hittable_sphere(node));
	else if (node->type == OBJ_PLANE)
		return (create_hittable_plane(node));
	else if (node->type == OBJ_CYLINDER)
		return (create_hittable_cylinder(node));
	if (node->type == OBJ_TRIANGLE)
		return (create_hittable_triangle(node));
	return (NULL);
}

t_hittable	*build_world(t_object *objects)
{
	t_hittable		*world;
	t_hittable_list	*list;
	int				i;

	world = malloc(sizeof(t_hittable));
	list = malloc(sizeof(t_hittable_list));
	if (!world || !list)
		return (NULL);
	list->count = count_objects(objects);
	list->elements = malloc(sizeof(t_hittable *) * list->count);
	if (!list->elements)
		return (NULL);
	i = 0;
	while (objects)
	{
		list->elements[i] = create_wrapper(objects);
		objects = objects->next;
		i++;
	}
	world->object = list;
	world->hit = hit_list;
	return (world);
}