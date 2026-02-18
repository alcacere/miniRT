/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 23:53:07 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 15:57:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include "types.h"
#include "point.h"
#include "vector.h"
#include "ray.h"
#include "hittable_list.h"
#include "random.h"
#include "color.h"

/* Camera type */
typedef struct s_camera
{
	real_t aspect_ratio;
	real_t samples_per_pixel;
	real_t pixel_samples_scale;
	int image_width;
	int image_height;
	int max_depth;
	real_t vfov;
	t_point3 lookfrom;
	t_point3 lookat;
	t_vec3 vup;
	real_t defocus_angle;
	real_t focus_dist;
	t_point3 center;
	t_point3 pixel00_loc;
	t_vec3 pixel_delta_u;
	t_vec3 pixel_delta_v;
	t_vec3 u;
	t_vec3 v;
	t_vec3 w;
	t_vec3 defocus_disk_u;
	t_vec3 defocus_disk_v;
} t_camera;

/* Initialize camera in-place */
static inline void camera_init(t_camera *camera, real_t aspect_ratio, int image_width)
{
	real_t focal_length;
	real_t viewport_height;
	real_t viewport_width;
	real_t theta;
	real_t h;
	t_vec3 viewport_u;
	t_vec3 viewport_v;
	t_vec3 viewport_upper_left;
	real_t defocus_radius;

	if (!camera)
		return;

	camera->max_depth = 50;
	camera->aspect_ratio = (aspect_ratio > 0) ? aspect_ratio : (real_t)1.0;
	camera->image_width = (image_width > 0) ? image_width : 100;
	camera->image_height = (int)((real_t)camera->image_width / camera->aspect_ratio);
	if (camera->image_height < 1)
		camera->image_height = 1;

	camera->center = camera->lookfrom;

	/* Calculate w: direction from lookfrom to lookat, then negate */
	t_vec3 lookat_minus_lookfrom = vec3_sub(&camera->lookat, &camera->lookfrom);
	camera->w = unit_vector(&lookat_minus_lookfrom);
	camera->w = vec3_neg(&camera->w);

	/* Calculate focal_length from lookfrom - lookat distance */
	focal_length = vec3_length(&lookat_minus_lookfrom);
	if (focal_length < (real_t)1e-6)
		focal_length = (real_t)1.0;

	/* Determine viewport dimensions using vfov and focus_dist */
	theta = degrees_to_radians(camera->vfov);
	h = (real_t)tan((double)theta / 2.0);
	viewport_height = (real_t)(2.0 * h * camera->focus_dist);
	viewport_width = viewport_height * ((real_t)camera->image_width / (real_t)camera->image_height);

	/* Calculate camera basis vectors: u, v from w */
	t_vec3 cross_vup_w = cross(&camera->vup, &camera->w);
	camera->u = unit_vector(&cross_vup_w);
	camera->v = cross(&camera->w, &camera->u);

	/* Calculate viewport edges using camera basis vectors */
	viewport_u = vec3_mul_scalar(&camera->u, viewport_width);
	/* Negate v to go downward in image space */
	t_vec3 neg_v = vec3_neg(&camera->v);
	viewport_v = vec3_mul_scalar(&neg_v, viewport_height);

	camera->pixel_delta_u = vec3_div_scalar(&viewport_u, (real_t)camera->image_width);
	camera->pixel_delta_v = vec3_div_scalar(&viewport_v, (real_t)camera->image_height);

	/* Calculate viewport upper left */
	t_vec3 focal_w = vec3_mul_scalar(&camera->w, camera->focus_dist);
	t_vec3 center_minus_focal = vec3_sub(&camera->center, &focal_w);
	t_vec3 half_u = vec3_div_scalar(&viewport_u, (real_t)2.0);
	t_vec3 half_v = vec3_div_scalar(&viewport_v, (real_t)2.0);
	viewport_upper_left = vec3_sub(&center_minus_focal, &half_u);
	viewport_upper_left = vec3_sub(&viewport_upper_left, &half_v);

	if (camera->samples_per_pixel <= (real_t)0.0)
		camera->samples_per_pixel = (real_t)1.0;
	camera->pixel_samples_scale = (real_t)1.0 / camera->samples_per_pixel;

	t_vec3 sum = vec3_add(&camera->pixel_delta_u, &camera->pixel_delta_v);
	t_vec3 half_sum = vec3_mul_scalar(&sum, (real_t)0.5);
	camera->pixel00_loc = vec3_add(&viewport_upper_left, &half_sum);

	/* Calculate defocus disk basis vectors */
	defocus_radius = camera->focus_dist * (real_t)tan((double)degrees_to_radians(camera->defocus_angle / (real_t)2.0));
	camera->defocus_disk_u = vec3_mul_scalar(&camera->u, defocus_radius);
	camera->defocus_disk_v = vec3_mul_scalar(&camera->v, defocus_radius);
}

/* Return a random point in the [-0.5,-0.5] .. [+0.5,+0.5] square (z = 0) */
static inline t_vec3 sample_square(void)
{
	return vec3_create(random_real() - (real_t)0.5, random_real() - (real_t)0.5, (real_t)0.0);
}

/* Return a random point on the defocus disk */
static inline t_vec3 defocus_disk_sample(const t_camera *cam)
{
	t_vec3 p = random_in_unit_disk();
	t_vec3 p0_term = vec3_mul_scalar(&cam->defocus_disk_u, p.x);
	t_vec3 p1_term = vec3_mul_scalar(&cam->defocus_disk_v, p.y);
	t_vec3 offset = vec3_add(&p0_term, &p1_term);
	return vec3_add(&cam->center, &offset);
}

/* construct a camera ray originating from the defocus disk (or center) and directed at a
   randomly sampled point around pixel (i,j), with random time for motion blur */
static inline t_ray get_ray(const t_camera *cam, int i, int j)
{
	t_vec3 offset = sample_square();

	/* tmp_u = (i + offset.x) * pixel_delta_u */
	t_vec3 tmp_u = vec3_mul_scalar(&cam->pixel_delta_u, (real_t)(i + offset.x));
	/* tmp_v = (j + offset.y) * pixel_delta_v */
	t_vec3 tmp_v = vec3_mul_scalar(&cam->pixel_delta_v, (real_t)(j + offset.y));

	t_vec3 tmp = vec3_add(&cam->pixel00_loc, &tmp_u);
	t_vec3 pixel_sample = vec3_add(&tmp, &tmp_v);

	/* Ray origin: defocus disk if enabled, otherwise camera center */
	t_vec3 ray_origin = (cam->defocus_angle <= (real_t)0.0) ? cam->center : defocus_disk_sample(cam);

	/* Ray direction: from origin to pixel sample */
	t_vec3 ray_direction = vec3_sub(&pixel_sample, &ray_origin);

	/* Random time in [0, 1] for motion blur */
	real_t ray_time = random_real();

	return ray_create(ray_origin, ray_direction, ray_time);
}

/* Render function: stream PPM output to `out` using given world */
static inline void camera_render(const t_camera *camera, FILE *out, const t_hittable_list *world)
{
	if (!camera || !out)
		return;

	fprintf(out, "P3\n%d %d\n255\n", camera->image_width, camera->image_height);

	for (int j = 0; j < camera->image_height; ++j)
	{
		fprintf(stderr, "\rScanlines remaining: %d ", (camera->image_height - j));
		fflush(stderr);
		for (int i = 0; i < camera->image_width; ++i)
		{
			t_color pixel_color = vec3_zero();
			for (int sample = 0; sample < (int)camera->samples_per_pixel; ++sample)
			{
				t_ray r = get_ray(camera, i, j);
				/* use camera-controlled max depth */
				t_vec3 sample_color = ray_color_depth(&r, world, camera->max_depth);
				pixel_color = vec3_add(&pixel_color, &sample_color);
			}
			t_vec3 scaled = vec3_mul_scalar(&pixel_color, camera->pixel_samples_scale);
			/* debug: print central pixel computed color once */
			if (i == camera->image_width / 2 && j == camera->image_height / 2)
			{
				fprintf(stderr, "\nDEBUG: central pixel (i=%d,j=%d) accumulated = (%f,%f,%f) scaled = (%f,%f,%f)\n",
						i, j,
						pixel_color.x, pixel_color.y, pixel_color.z,
						scaled.x, scaled.y, scaled.z);
			}
			write_color(out, &scaled);
		}
	}
	fprintf(stderr, "\rDone.                 \n");
}

#endif