/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:40:31 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:41:41 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include <math.h>

static void	check_axis(t_interval ax, double orig, double inv, t_interval *t)
{
	double	t0;
	double	t1;
	double	tmp;

	t0 = (ax.min - orig) * inv;
	t1 = (ax.max - orig) * inv;
	if (inv < 0.0)
	{
		tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	if (t0 > t->min)
		t->min = t0;
	if (t1 < t->max)
		t->max = t1;
}

int	hit_aabb(const t_aabb *box, const t_ray *r, t_interval rayt)
{
	check_axis(interval_create(box->min.x, box->max.x),
		r->origin.x, r->inv_dir.x, &rayt);
	if (rayt.max <= rayt.min)
		return (0);
	check_axis(interval_create(box->min.y, box->max.y),
		r->origin.y, r->inv_dir.y, &rayt);
	if (rayt.max <= rayt.min)
		return (0);
	check_axis(interval_create(box->min.z, box->max.z),
		r->origin.z, r->inv_dir.z, &rayt);
	if (rayt.max <= rayt.min)
		return (0);
	return (1);
}

t_aabb	aabb_merge(t_aabb box0, t_aabb box1)
{
	t_aabb	new_box;

	new_box.min.x = fmin(box0.min.x, box1.min.x);
	new_box.min.y = fmin(box0.min.y, box1.min.y);
	new_box.min.z = fmin(box0.min.z, box1.min.z);
	new_box.max.x = fmax(box0.max.x, box1.max.x);
	new_box.max.y = fmax(box0.max.y, box1.max.y);
	new_box.max.z = fmax(box0.max.z, box1.max.z);
	return (new_box);
}

void	aabb_pad(t_aabb *box)
{
	double	delta;

	delta = 0.0001;
	if (fabs(box->max.x - box->min.x) < delta)
	{
		box->min.x -= delta;
		box->max.x += delta;
	}
	if (fabs(box->max.y - box->min.y) < delta)
	{
		box->min.y -= delta;
		box->max.y += delta;
	}
	if (fabs(box->max.z - box->min.z) < delta)
	{
		box->min.z -= delta;
		box->max.z += delta;
	}
}
