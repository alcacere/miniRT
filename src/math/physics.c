/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:40:42 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/05 22:41:36 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include <math.h>

t_vec3	reflect(t_vec3 v, t_vec3 n)
{
	return (vec3_sub(v, vec3_scale(n, 2.0 * vec3_dot(v, n))));
}

t_vec3	refract(t_vec3 uv, t_vec3 n, double etai_over_etat)
{
	double	cos_theta;
	t_vec3	r_out_perp;
	t_vec3	r_out_parallel;

	cos_theta = fmin(vec3_dot(vec3_scale(uv, -1.0), n), 1.0);
	r_out_perp = vec3_scale(vec3_add(uv,
				vec3_scale(n, cos_theta)), etai_over_etat);
	r_out_parallel = vec3_scale(n, -sqrt(fabs(1.0
					- vec3_dot(r_out_perp, r_out_perp))));
	return (vec3_add(r_out_perp, r_out_parallel));
}

double	reflectance(double cosine, double ref_idx)
{
	double	r0;

	r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
	r0 = r0 * r0;
	return (r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0));
}
