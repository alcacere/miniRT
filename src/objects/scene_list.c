#include "minirt.h"

void	object_add_back(t_object **head, t_object *node)
{
	t_object	*tmp;

	if (!head || !node)
		return ;
	if (!*head)
	{
		*head = node;
		return ;
	}
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = node;
}

void	object_free(t_object *head)
{
	t_object	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		if (tmp->shape)
			free(tmp->shape);
		free(tmp);
	}
}
