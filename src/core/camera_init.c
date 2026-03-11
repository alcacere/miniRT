/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:19:36 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 19:00:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "graphics.h"
#include <math.h>

static void	calc_viewport(t_camera *c, double *vp_h, double *vp_w)
{
	double	theta;
	double	h;
	t_vec3	vup;

	theta = c->vfov * M_PI / 180.0;
	h = tan(theta / 2.0);
	c->focus_dist = 1.0;
	*vp_h = 2.0 * h * c->focus_dist;
	*vp_w = *vp_h * ((double)c->image_width / c->image_height);
	c->w = vec3_normalize(vec3_sub(c->lookfrom, c->lookat));
	vup = vec3_create(0, 1, 0);
	if (fabs(c->w.x) < 1e-6 && fabs(c->w.z) < 1e-6)
		vup = vec3_create(0, 0, -1);
	c->u = vec3_normalize(vec3_cross(vup, c->w));
	c->v = vec3_cross(c->w, c->u);
}

void	init_camera(t_camera *c)
{
	double	vp_dim[2];
	t_vec3	vp_u;
	t_vec3	vp_v;
	t_vec3	vp_ul;

	c->center = c->lookfrom;
	c->image_width = WIN_W;
	c->image_height = WIN_H;
	if (c->image_height < 1)
		c->image_height = 1;
	c->aspect_ratio = (double)c->image_width / (double)c->image_height;
	c->sqrt_spp = SQRT_SPP;
	c->recip_sqrt_spp = 1.0 / c->sqrt_spp;
	c->pixel_samples_scale = 1.0 / (c->sqrt_spp * c->sqrt_spp);
	c->max_depth = MAX_DEPTH;
	calc_viewport(c, &vp_dim[0], &vp_dim[1]);
	vp_u = vec3_scale(c->u, vp_dim[1]);
	vp_v = vec3_scale(c->v, -vp_dim[0]);
	c->pixel_delta_u = vec3_scale(vp_u, 1.0 / c->image_width);
	c->pixel_delta_v = vec3_scale(vp_v, 1.0 / c->image_height);
	vp_ul = vec3_sub(c->center, vec3_scale(c->w, c->focus_dist));
	vp_ul = vec3_sub(vp_ul, vec3_scale(vp_u, 0.5));
	vp_ul = vec3_sub(vp_ul, vec3_scale(vp_v, 0.5));
	c->pixel00_loc = vec3_add(vp_ul, vec3_scale(vec3_add(c->pixel_delta_u,
					c->pixel_delta_v), 0.5));
}
