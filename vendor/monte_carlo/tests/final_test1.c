#include "../common.h"

/* Check if a new sphere at 'center' with 'radius' collides with any existing sphere */
static bool check_collision(const t_point3 *center, real_t radius,
							const t_point3 *placed, const real_t *radii, int count)
{
	for (int i = 0; i < count; ++i)
	{
		t_vec3 diff = vec3_sub(center, &placed[i]);
		real_t min_dist = radius + radii[i];
		if (vec3_length(&diff) < min_dist + 0.01) /* small epsilon for safety */
			return true;
	}
	return false;
}

int main(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Ground: simple gray lambertian (matching original C++) */
	t_material *ground_material = lambertian_create(vec3_create(0.5, 0.5, 0.5));
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(0.5, 0.5, 0.5), ground_material);
	hittable_list_add_sphere(&world, &ground);

	/* Track all placed spheres for collision detection */
	t_point3 placed[500];
	real_t radii[500];
	int placed_count = 0;

	/* Pre-register the three big spheres to avoid collisions */
	placed[placed_count] = point3_create(0.0, 1.0, 0.0);
	radii[placed_count++] = 1.0;
	placed[placed_count] = point3_create(-4.0, 1.0, 0.0);
	radii[placed_count++] = 1.0;
	placed[placed_count] = point3_create(4.0, 1.0, 0.0);
	radii[placed_count++] = 1.0;

	/* Random small spheres - matching original C++ logic */
	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			real_t choose_mat = random_double();
			t_point3 center = point3_create(
				(real_t)a + (real_t)0.9 * random_double(),
				(real_t)0.2,
				(real_t)b + (real_t)0.9 * random_double());

			/* Check distance from point (4, 0.2, 0) - original C++ condition */
			t_point3 ref = point3_create(4.0, 0.2, 0.0);
			t_vec3 delta = vec3_sub(&center, &ref);
			if (vec3_length(&delta) > 0.9)
			{
				/* Check collision with all previously placed spheres */
				if (check_collision(&center, 0.2, placed, radii, placed_count))
					continue;

				t_material *sphere_material = NULL;

				if (choose_mat < 0.8)
				{
					/* Diffuse: albedo = random() * random() */
					t_vec3 r1 = vec3_random();
					t_vec3 r2 = vec3_random();
					t_vec3 albedo = vec3_mul_elem(&r1, &r2);
					sphere_material = lambertian_create(albedo);
				}
				else if (choose_mat < 0.95)
				{
					/* Metal: albedo in [0.5, 1], fuzz in [0, 0.5] */
					t_vec3 albedo = vec3_random_interval((real_t)0.5, (real_t)1.0);
					real_t fuzz = random_real_interval((real_t)0.0, (real_t)0.5);
					sphere_material = metal_create_fuzz(albedo, fuzz);
				}
				else
				{
					/* Glass */
					sphere_material = dielectric_create((real_t)1.5);
				}

				if (sphere_material)
				{
					t_sphere s = create_sphere(&center, (real_t)0.2, vec3_create(1.0, 1.0, 1.0), sphere_material);
					hittable_list_add_sphere(&world, &s);

					/* Register this sphere for future collision checks */
					if (placed_count < (int)(sizeof(placed) / sizeof(placed[0])))
					{
						placed[placed_count] = center;
						radii[placed_count] = 0.2;
						placed_count++;
					}
				}
			}
		}
	}

	/* Three big spheres - exactly as in original C++ */

	/* Glass sphere at center */
	t_material *material1 = dielectric_create((real_t)1.5);
	t_point3 c1 = point3_create(0.0, 1.0, 0.0);
	t_sphere s1 = create_sphere(&c1, (real_t)1.0, vec3_create(1.0, 1.0, 1.0), material1);
	hittable_list_add_sphere(&world, &s1);

	/* Lambertian (matte brown) sphere on left */
	t_material *material2 = lambertian_create(vec3_create(0.4, 0.2, 0.1));
	t_point3 c2 = point3_create(-4.0, 1.0, 0.0);
	t_sphere s2 = create_sphere(&c2, (real_t)1.0, vec3_create(0.4, 0.2, 0.1), material2);
	hittable_list_add_sphere(&world, &s2);

	/* Metal sphere on right (no fuzz = perfect mirror) */
	t_material *material3 = metal_create_fuzz(vec3_create(0.7, 0.6, 0.5), (real_t)0.0);
	t_point3 c3 = point3_create(4.0, 1.0, 0.0);
	t_sphere s3 = create_sphere(&c3, (real_t)1.0, vec3_create(0.7, 0.6, 0.5), material3);
	hittable_list_add_sphere(&world, &s3);

	/* Camera - matching original C++ settings with DEPTH OF FIELD enabled */
	t_camera cam;
	cam.aspect_ratio = (real_t)(16.0 / 9.0);
	cam.image_width = 1200;
	cam.samples_per_pixel = 100.0; /* matching original */
	cam.max_depth = 50;
	cam.vfov = 20.0;
	cam.lookfrom = point3_create(13.0, 2.0, 3.0);
	cam.lookat = point3_create(0.0, 0.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);

	/* DEPTH OF FIELD - this creates the bokeh/blur effect! */
	cam.defocus_angle = 0.6; /* aperture size - larger = more blur */
	cam.focus_dist = 10.0;	 /* distance to focus plane */

	/* Sky background color */
	cam.background = vec3_create(0.70, 0.80, 1.00);

	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);

	hittable_list_clear(&world);
	return 0;
}
