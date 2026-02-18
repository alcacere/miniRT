/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin_spheres.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:20:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 01:19:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "noise_texture.h"
#include <stdio.h>

void perlin_spheres(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Create noise texture with better scale for detail */
	t_texture *pertext = noise_texture_create((real_t)4.0);
	if (!pertext)
	{
		fprintf(stderr, "Failed to create noise texture\n");
		return;
	}

	/* Create material using the noise texture */
	t_material *mat = lambertian_create_texture(pertext);
	if (!mat)
	{
		fprintf(stderr, "Failed to create lambertian material\n");
		pertext->destroy(pertext);
		free(pertext);
		return;
	}

	/* Ground sphere: large radius, positioned below */
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(&world, &ground);

	/* Upper sphere */
	t_point3 upper_center = point3_create(0.0, 2.0, 0.0);
	t_sphere upper = create_sphere(&upper_center, 2.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(&world, &upper);

	/* Setup camera with improved quality settings */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 800;		  /* increased from 400 */
	camera.samples_per_pixel = 300.0; /* increased from 100 */
	camera.max_depth = 50;			  /* keep high for complex light paths */
	camera.vfov = 20.0;
	camera.lookfrom = point3_create(13.0, 2.0, 3.0);
	camera.lookat = point3_create(0.0, 0.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.70, 0.80, 1.00);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render to stdout */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	hittable_list_clear(&world);

	/* Now safely destroy the shared material and texture */
	mat->destroy(mat);
	free(mat);
	pertext->destroy(pertext);
	free(pertext);
}
