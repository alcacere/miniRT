#include "minirt.h"
#include <stdlib.h>

void	object_free(t_object *head)
{
	t_object	*tmp;

	while (head)
	{
		tmp = head->next;
		if (head->shape)
			free(head->shape);
		free(head);
		head = tmp;
	}
}

void	light_free(t_light *head)
{
	t_light	*tmp;

	while (head)
	{
		tmp = head->next;
		free(head);
		head = tmp;
	}
}

void	free_scene(t_scene *scene)
{
	if (!scene)
		return ;
	if (scene->objects)
	{
		object_free(scene->objects);
		scene->objects = NULL;
	}
	if (scene->lights)
	{
		light_free(scene->lights);
		scene->lights = NULL;
	}
}