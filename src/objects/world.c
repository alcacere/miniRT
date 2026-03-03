#include "objects.h"
#include <stdlib.h>

t_hittable	*build_bvh(t_hittable **list, int n, uint32_t *seed);

static t_hittable	*create_wrapper(t_object *node)
{
	if (node->type == OBJ_SPHERE)
		return (create_hittable_sphere(node));
	if (node->type == OBJ_PLANE)
		return (create_hittable_plane(node));
	if (node->type == OBJ_CYLINDER)
		return (create_hittable_cylinder(node));
	if (node->type == OBJ_TRIANGLE)
		return (create_hittable_triangle(node));
	if (node->type == OBJ_CONE)
		return (create_hittable_cone(node));
	return (NULL);
}

t_hittable	*build_world(t_object *objects)
{
	t_hittable	**arr;
	int			count;
	int			i;
	t_object	*tmp;
	t_hittable	*bvh_root;
	uint32_t	seed;

	count = 0;
	tmp = objects;
	while (tmp && ++count)
		tmp = tmp->next;
	if (count == 0)
		return (NULL);
	arr = malloc(sizeof(t_hittable *) * count);
	tmp = objects;
	i = 0;
	while (tmp)
	{
		arr[i++] = create_wrapper(tmp);
		tmp = tmp->next;
	}
	seed = 4242;
	bvh_root = build_bvh(arr, count, &seed);
	free(arr);
	return (bvh_root);
}