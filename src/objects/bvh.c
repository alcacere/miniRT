#include "objects.h"
#include <stdlib.h>

static void	sort_hittables(t_hittable **arr, int n, int axis)
{
	int			i;
	int			j;
	t_hittable	*tmp;
	double		a;
	double		b;

	i = -1;
	while (++i < n - 1)
	{
		j = -1;
		while (++j < n - i - 1)
		{
			a = arr[j]->bbox.min.x; b = arr[j + 1]->bbox.min.x;
			if (axis == 1) { a = arr[j]->bbox.min.y; b = arr[j + 1]->bbox.min.y; }
			if (axis == 2) { a = arr[j]->bbox.min.z; b = arr[j + 1]->bbox.min.z; }
			if (a > b)
			{
				tmp = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = tmp;
			}
		}
	}
}

int	hit_bvh(const void *object, const t_ray *r, t_interval rayt, t_hit_record *rec)
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

t_hittable	*build_bvh(t_hittable **list, int n, uint32_t *seed)
{
	t_hittable	*node;
	t_bvh_node	*bvh;
	int			axis;

	node = malloc(sizeof(t_hittable));
	bvh = malloc(sizeof(t_bvh_node));
	axis = (int)(random_double(seed) * 3.0) % 3;
	if (n == 1)
		bvh->left = bvh->right = list[0];
	else if (n == 2)
	{
		bvh->left = list[0]; bvh->right = list[1];
	}
	else
	{
		sort_hittables(list, n, axis);
		bvh->left = build_bvh(list, n / 2, seed);
		bvh->right = build_bvh(list + n / 2, n - n / 2, seed);
	}
	bvh->bbox = aabb_merge(bvh->left->bbox, bvh->right->bbox);
	node->object = bvh;
	node->hit = hit_bvh;
	node->bbox = bvh->bbox;
	return (node);
}