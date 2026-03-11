/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:12:58 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 20:46:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GRAPHICS_H
# define GRAPHICS_H

# include "minirt.h"

# ifndef KEY_ESC
#  define KEY_ESC		65307
# endif

/* Window resolution macros — change these to resize the render */
# define WIN_W			1200
# define WIN_H			675

/* Render quality macros — higher = better but slower */
/* SQRT_SPP: sqrt of samples per pixel (total spp = SQRT_SPP²)       */
/*   3 = fast preview (9 spp), 10 = default (100), 30 = HQ (900)     */
# define SQRT_SPP		30
/* MAX_DEPTH: max ray bounces for reflections/refractions             */
/*   5 = fast, 10 = default, 50 = very accurate glass/mirrors        */
# define MAX_DEPTH		10

/* Color grading macros — adjust the look and feel of the render      */
/* GAMMA: gamma correction exponent (1/value applied to final color)  */
/*   2.2 = standard sRGB, 1.8 = brighter, 2.6 = darker              */
# define GAMMA			1.8

/* CONTRAST: contrast boost (1.0 = unchanged, 1.3 = punchy)          */
/*   applies an S-curve centered at mid-gray                         */
# define CONTRAST		1.3

/* SATURATION: color saturation multiplier                            */
/*   1.0 = unchanged, 1.5 = vivid, 0.0 = grayscale                  */
# define SATURATION		1.5

/* EXPOSURE: brightness multiplier applied before tone mapping        */
/*   1.0 = unchanged, 2.0 = 2x brighter, 0.5 = dimmer               */
# define EXPOSURE		1.0

/* Emission light color tint — multiplied onto every light's color   */
/* 1.0 = no change, 0.8 = darken that channel, 0.0 = remove channel  */
/* Example: warm white = 1.0 / 0.92 / 0.75  cool = 0.75 / 0.9 / 1.0 */
# define LIGHT_R		0.025
# define LIGHT_G		1.0
# define LIGHT_B		0.50

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
}	t_img;

void	init_graphics(t_minirt *rt, t_img *img);
int		key_hook(int keycode, t_minirt *rt);
int		close_window(t_minirt *rt);
int		expose_hook(t_minirt *rt);

#endif
