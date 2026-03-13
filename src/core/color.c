/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 23:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 02:00:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "graphics.h"
#include <math.h>

/*
** clamp_01: clamp a value to [0, 1] range.
*/
static double	clamp_01(double v)
{
	if (v < 0.0)
		return (0.0);
	if (v > 1.0)
		return (1.0);
	return (v);
}

/*
** apply_contrast: S-curve contrast centered at midpoint 0.5.
** CONTRAST > 1 increases contrast, < 1 reduces it.
*/
static double	apply_contrast(double v, double contrast)
{
	v = v - 0.5;
	v = v * contrast;
	return (v + 0.5);
}

/*
** rgb_to_int: converts a linear HDR color to 8-bit sRGB integer.
** Pipeline: gamma correction -> contrast -> saturation -> clamp -> pack.
** Tunable via GAMMA, CONTRAST, SATURATION macros in graphics.h.
*/
int	rgb_to_int(t_color color)
{
	double	gamma_inv;
	double	luma;
	int		rgb[3];

	color.x = color.x * EXPOSURE;
	color.y = color.y * EXPOSURE;
	color.z = color.z * EXPOSURE;
	gamma_inv = 1.0 / GAMMA;
	color.x = pow(fmax(color.x, 0.0), gamma_inv);
	color.y = pow(fmax(color.y, 0.0), gamma_inv);
	color.z = pow(fmax(color.z, 0.0), gamma_inv);
	color.x = apply_contrast(color.x, CONTRAST);
	color.y = apply_contrast(color.y, CONTRAST);
	color.z = apply_contrast(color.z, CONTRAST);
	luma = 0.2126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
	color.x = luma + SATURATION * (color.x - luma);
	color.y = luma + SATURATION * (color.y - luma);
	color.z = luma + SATURATION * (color.z - luma);
	rgb[0] = (int)(clamp_01(color.x) * 255.0);
	rgb[1] = (int)(clamp_01(color.y) * 255.0);
	rgb[2] = (int)(clamp_01(color.z) * 255.0);
	return ((rgb[0] << 16) | (rgb[1] << 8) | rgb[2]);
}
