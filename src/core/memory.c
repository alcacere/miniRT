/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:20:36 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 22:48:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "hittable.h"
#include <stdlib.h>

int	hit_bvh(const void *object, const t_ray *r,
		t_interval rayt, t_hit_record *rec);

void	free_bvh(t_hittable *node)
{
	t_bvh_node	*bvh;

	if (!node)
		return ;
	if (node->hit == hit_bvh)
	{
		bvh = (t_bvh_node *)node->object;
		if (bvh->left != bvh->right)
		{
			free_bvh(bvh->left);
			free_bvh(bvh->right);
		}
		else
			free_bvh(bvh->left);
		free(bvh);
	}
	free(node);
}

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
