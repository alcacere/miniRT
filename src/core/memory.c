/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:20:36 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:21:08 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void	free_scene(t_scene *scene)
{
	if (!scene)
		return ;
	object_free(scene->objects);
}
