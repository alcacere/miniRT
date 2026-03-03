#include "minirt.h"

void	object_add_back(t_object **head, t_object *node)
{
	t_object	*tmp;

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