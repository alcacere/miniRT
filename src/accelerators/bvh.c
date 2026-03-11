/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 15:45:23 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "objects.h"
#include <stdlib.h>

t_hittable	*build_bvh(t_hittable **list, int n, uint32_t *seed);
void		ft_swap_ptr(void **a, void **b);

static double	get_axis_val(t_hittable *node, int axis)
{
	if (axis == 1)
		return (node->bbox.min.y);
	if (axis == 2)
		return (node->bbox.min.z);
	return (node->bbox.min.x);
}

static void	sort_hittables(t_hittable **arr, int n, int axis)
{
	int		i;
	int		j;

	i = -1;
	while (++i < n - 1)
	{
		j = -1;
		while (++j < n - i - 1)
		{
			if (get_axis_val(arr[j], axis) > get_axis_val(arr[j + 1], axis))
				ft_swap_ptr((void **)&arr[j], (void **)&arr[j + 1]);
		}
	}
}

int	hit_bvh(const void *object, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_bvh_node	*bvh;
	int			hit_left;
	int			hit_right;

	bvh = (t_bvh_node *)object;
	if (!hit_aabb(&bvh->bbox, r, rayt))
		return (0);
	hit_left = bvh->left->hit(bvh->left->object, r, rayt, rec);
	if (hit_left)
		rayt.max = rec->t;
	hit_right = bvh->right->hit(bvh->right->object, r, rayt, rec);
	return (hit_left || hit_right);
}

static void	set_bvh_children(t_bvh_node *bvh, t_hittable **list, int n,
		uint32_t *seed)
{
	if (n == 1)
	{
		bvh->left = list[0];
		bvh->right = list[0];
	}
	else if (n == 2)
	{
		bvh->left = list[0];
		bvh->right = list[1];
	}
	else
	{
		sort_hittables(list, n, (int)(random_double(seed) * 3.0) % 3);
		bvh->left = build_bvh(list, n / 2, seed);
		bvh->right = build_bvh(list + n / 2, n - n / 2, seed);
	}
}

t_hittable	*build_bvh(t_hittable **list, int n, uint32_t *seed)
{
	t_hittable	*node;
	t_bvh_node	*bvh;

	node = malloc(sizeof(t_hittable));
	bvh = malloc(sizeof(t_bvh_node));
	if (!node || !bvh)
		return (NULL);
	set_bvh_children(bvh, list, n, seed);
	bvh->bbox = aabb_merge(bvh->left->bbox, bvh->right->bbox);
	node->object = bvh;
	node->hit = hit_bvh;
	node->bbox = bvh->bbox;
	return (node);
}
