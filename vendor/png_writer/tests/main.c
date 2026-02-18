/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 00:23:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "material.h"
#include "texture.h"
#include "bvh.h"

#include "tests/common.h"
#include "tests/material.h"
#include "tests/texture.h"
#include "tests/hittable_list.h"
#include <stdio.h>
#include <stdlib.h>
#include "../error.h"

static long filesize(const char *path)
{
	FILE *f = fopen(path, "rb");
	if (!f)
		return -1;
	if (fseek(f, 0, SEEK_END) != 0)
	{
		fclose(f);
		return -1;
	}
	long s = ftell(f);
	fclose(f);
	return s;
}

static int earth(const char *tex_path)
{
	t_texture *earth_tex = image_texture_create_png(tex_path);
	if (!earth_tex)
	{
		fprintf(stderr, "Error: failed to load texture: %s\n", tex_path);
		return 1;
	}

	t_material *earth_surface = lambertian_create_texture(earth_tex);
	if (!earth_surface)
	{
		earth_tex->destroy(earth_tex);
		free(earth_tex);
		return 1;
	}

	t_hittable_list world;
	hittable_list_init(&world);

	t_point3 center = point3_create(0.0, 0.0, 0.0);
	t_sphere globe = create_sphere(&center, 2.0, vec3_create(1.0, 1.0, 1.0), earth_surface);
	if (!hittable_list_add_sphere(&world, &globe))
	{
		fprintf(stderr, "Error: failed to add earth sphere\n");
		hittable_list_clear(&world);
		earth_surface->destroy(earth_surface);
		free(earth_surface);
		return 1;
	}

	t_camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 600;		 /* Increased from 400 for better detail */
	cam.samples_per_pixel = 500; /* Increased from 100 for less noise */
	cam.max_depth = 200;		 /* Increased from 50 for better light bounces */

	cam.vfov = 20.0;
	cam.lookfrom = point3_create(0.0, 0.0, 12.0);
	cam.lookat = point3_create(0.0, 0.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);

	cam.defocus_angle = 0.0; /* Sharp focus on earth */
	cam.focus_dist = 12.0;

	cam.u = vec3_zero();
	cam.v = vec3_zero();
	cam.w = vec3_zero();
	cam.defocus_disk_u = vec3_zero();
	cam.defocus_disk_v = vec3_zero();

	camera_init(&cam, cam.aspect_ratio, cam.image_width);

	FILE *out = fopen("earth.ppm", "w");
	if (!out)
	{
		fprintf(stderr, "Error: failed to open output file\n");
		hittable_list_clear(&world);
		earth_surface->destroy(earth_surface);
		free(earth_surface);
		return 1;
	}

	printf("Rendering earth with high quality settings...\n");
	printf("Resolution: %dx%d, Samples: %d, Max depth: %d\n",
		   cam.image_width, (int)(cam.image_width / cam.aspect_ratio),
		   (int)cam.samples_per_pixel, cam.max_depth);

	camera_render(&cam, out, &world);
	fclose(out);

	printf("Render complete! Output saved to earth.ppm\n");

	hittable_list_clear(&world);
	earth_surface->destroy(earth_surface);
	free(earth_surface);
	return 0;
}

int main(int argc, char **argv)
{
	printf("png_writer smoke-test executable\n");
	if (argc > 1)
	{
		long s = filesize(argv[1]);
		if (s < 0)
		{
			fprintf(stderr, "Failed to stat file: %s\n", argv[1]);
			return 1;
		}
		printf("File: %s, size: %ld bytes\n", argv[1], s);
	}

	const char *tex_path = (argc > 1) ? argv[1] : "assets/earthmap.png";

	switch (3)
	{
	// case 1: return bouncing_spheres();
	// case 2: return checkered_spheres();
	case 3:
		return earth(tex_path);
	default:
		return 0;
	}
}
