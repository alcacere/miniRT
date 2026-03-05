/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:20:55 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:20:58 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "texture.h"
#include "minirt.h"
#include <math.h>

t_color	apply_checkerboard(t_vec3 hit_point, t_color base_color)
{
	double	scale;
	int		spatial_wave;
	t_color	secondary_color;

	scale = 5.0;
	secondary_color = vec3_create(0.05, 0.05, 0.05);
	spatial_wave = (int)(floor(hit_point.x * scale)
			+ floor(hit_point.y * scale) + floor(hit_point.z * scale));
	if (spatial_wave % 2 == 0)
		return (base_color);
	return (secondary_color);
}

void	apply_bumpmap(t_vec3 p, t_vec3 *normal)
{
	double	scale;
	double	bumpiness;
	t_vec3	bump;

	scale = 20.0;
	bumpiness = 1.8;
	bump.x = sin(scale * p.x) * bumpiness;
	bump.y = sin(scale * p.y) * bumpiness;
	bump.z = sin(scale * p.z) * bumpiness;
	*normal = vec3_normalize(vec3_add(*normal, bump));
}
