/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:40:48 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:41:33 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

double	random_double(uint32_t *seed)
{
	*seed ^= *seed << 13;
	*seed ^= *seed >> 17;
	*seed ^= *seed << 5;
	return ((double)*seed / 4294967295.0);
}

double	random_double_range(uint32_t *seed, double min, double max)
{
	return (min + (max - min) * random_double(seed));
}

t_vec3	random_vec3_range(uint32_t *seed, double min, double max)
{
	return (vec3_create(random_double_range(seed, min, max),
			random_double_range(seed, min, max),
			random_double_range(seed, min, max)));
}

t_vec3	random_in_unit_sphere(uint32_t *seed)
{
	t_vec3	p;

	while (1)
	{
		p = random_vec3_range(seed, -1.0, 1.0);
		if (vec3_dot(p, p) < 1.0)
			return (p);
	}
}

t_vec3	random_unit_vector(uint32_t *seed)
{
	return (vec3_normalize(random_in_unit_sphere(seed)));
}
