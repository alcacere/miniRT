#include "common.h"
#include "bvh.h"
#include "constant_medium.h"

/* scene forward declarations */
void bouncing_spheres(void);
void checkered_spheres(void);
void earth(void);
void perlin_spheres(void);
void quads(void);
void simple_light(void);
void cornell_box(void);
void cornell_smoke(void);

static inline t_point3 random_point3(real_t min, real_t max)
{
	return point3_create(
		random_real_interval(min, max),
		random_real_interval(min, max),
		random_real_interval(min, max));
}

void final_scene(int image_width, int samples_per_pixel, int max_depth)
{
	t_hittable_list boxes1;
	hittable_list_init(&boxes1);
	t_material *ground = lambertian_create(vec3_create(0.48, 0.83, 0.53));

	int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; ++i)
		for (int j = 0; j < boxes_per_side; ++j)
		{
			real_t w = 100.0;
			real_t x0 = -1000.0 + i * w;
			real_t z0 = -1000.0 + j * w;
			real_t x1 = x0 + w;
			real_t y1 = random_real_interval(1.0, 101.0);
			real_t z1 = z0 + w;
			t_point3 a = point3_create(x0, 0.0, z0);
			t_point3 b = point3_create(x1, y1, z1);
			box(&boxes1, &a, &b, ground);
		}

	t_hittable_list world;
	hittable_list_init(&world);

	t_bvh_node *boxes1_bvh = bvh_node_create(&boxes1);
	if (boxes1_bvh)
	{
		t_hittable_wrapper wrap = {
			.object = boxes1_bvh,
			.owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit,
			.bbox = boxes1_bvh->bbox};
		hittable_list_add_wrapper(&world, &wrap);
	}

	t_material *light = diffuse_light_create(vec3_create(7.0, 7.0, 7.0));
	t_point3 lq = point3_create(123.0, 554.0, 147.0);
	t_vec3 lu = vec3_create(300.0, 0.0, 0.0);
	t_vec3 lv = vec3_create(0.0, 0.0, 265.0);
	t_quad q = quad_create(&lq, &lu, &lv, light);
	t_quad *qptr = (t_quad *)malloc(sizeof(t_quad));
	if (qptr)
	{
		*qptr = q;
		t_hittable_wrapper qw = {.object = qptr, .owned = true, .set_current = set_current_quad, .hit_noobj = quad_hit_noobj, .bbox = q.bbox};
		hittable_list_add_wrapper(&world, &qw);
	}

	t_material *move_mat = lambertian_create(vec3_create(0.7, 0.3, 0.1));
	t_point3 c1 = point3_create(400.0, 400.0, 200.0);
	t_point3 c2 = point3_create(430.0, 400.0, 200.0);
	t_sphere moving = create_sphere_moving(&c1, &c2, 50.0, vec3_create(0.7, 0.3, 0.1), move_mat);
	hittable_list_add_sphere(&world, &moving);

	t_material *glass = dielectric_create(1.5);
	t_point3 p1 = point3_create(260.0, 150.0, 45.0);
	t_sphere s1 = create_sphere(&p1, 50.0, vec3_create(1.0, 1.0, 1.0), glass);
	hittable_list_add_sphere(&world, &s1);

	t_material *metal_mat = metal_create_fuzz(vec3_create(0.8, 0.8, 0.9), 1.0);
	t_point3 p2 = point3_create(0.0, 150.0, 145.0);
	t_sphere s2 = create_sphere(&p2, 50.0, vec3_create(0.8, 0.8, 0.9), metal_mat);
	hittable_list_add_sphere(&world, &s2);

	t_sphere *boundary1 = (t_sphere *)malloc(sizeof(t_sphere));
	if (boundary1)
	{
		t_point3 bc1 = point3_create(360.0, 150.0, 145.0);
		*boundary1 = create_sphere(&bc1, 70.0, vec3_create(1.0, 1.0, 1.0), glass);
		t_hittable_wrapper bw1 = {.object = boundary1, .owned = true, .set_current = set_current_sphere, .hit_noobj = sphere_hit_noobj, .bbox = boundary1->bbox};
		hittable_list_add_wrapper(&world, &bw1);

		t_constant_medium *med1 = constant_medium_create_color(&bw1, 0.2, vec3_create(0.2, 0.4, 0.9));
		if (med1)
		{
			t_hittable_wrapper mw1 = {.object = med1, .owned = true, .set_current = set_current_medium, .hit_noobj = constant_medium_hit_noobj, .bbox = constant_medium_bounding_box(med1)};
			hittable_list_add_wrapper(&world, &mw1);
		}
	}

	t_sphere *boundary2 = (t_sphere *)malloc(sizeof(t_sphere));
	if (boundary2)
	{
		t_point3 bc2 = point3_create(0.0, 0.0, 0.0);
		*boundary2 = create_sphere(&bc2, 5000.0, vec3_create(1.0, 1.0, 1.0), glass);
		t_hittable_wrapper bw2 = {.object = boundary2, .owned = false, .set_current = set_current_sphere, .hit_noobj = sphere_hit_noobj, .bbox = boundary2->bbox};
		t_constant_medium *med2 = constant_medium_create_color(&bw2, 0.0001, vec3_create(1.0, 1.0, 1.0));
		if (med2)
		{
			t_hittable_wrapper mw2 = {.object = med2, .owned = true, .set_current = set_current_medium, .hit_noobj = constant_medium_hit_noobj, .bbox = constant_medium_bounding_box(med2)};
			hittable_list_add_wrapper(&world, &mw2);
		}
	}

	t_texture *earth_tex = image_texture_create_png("../png_writer/assets/earthmap.png");
	t_material *earth_mat = earth_tex ? lambertian_create_texture(earth_tex) : lambertian_create(vec3_create(0.4, 0.2, 0.1));
	t_point3 earth_c = point3_create(400.0, 200.0, 400.0);
	t_sphere earth_s = create_sphere(&earth_c, 100.0, vec3_create(1.0, 1.0, 1.0), earth_mat);
	hittable_list_add_sphere(&world, &earth_s);

	t_texture *pertex = noise_texture_create(0.2);
	t_material *permat = pertex ? lambertian_create_texture(pertex) : lambertian_create(vec3_create(1.0, 1.0, 1.0));
	t_point3 per_c = point3_create(220.0, 280.0, 300.0);
	t_sphere per_s = create_sphere(&per_c, 80.0, vec3_create(1.0, 1.0, 1.0), permat);
	hittable_list_add_sphere(&world, &per_s);

	t_hittable_list boxes2;
	hittable_list_init(&boxes2);
	t_material *white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
	int ns = 1000;
	for (int j = 0; j < ns; ++j)
	{
		t_point3 rp = random_point3(0.0, 165.0);
		t_sphere s = create_sphere(&rp, 10.0, vec3_create(0.73, 0.73, 0.73), white);
		hittable_list_add_sphere(&boxes2, &s);
	}

	t_bvh_node *boxes2_bvh = bvh_node_create(&boxes2);
	if (boxes2_bvh)
	{
		t_hittable_wrapper bw = {.object = boxes2_bvh, .owned = true, .set_current = set_current_bvh, .hit_noobj = bvh_node_hit, .bbox = boxes2_bvh->bbox};
		t_rotate_y_wrap *rot = rotate_y_create(&bw, 15.0);
		if (rot)
		{
			t_hittable_wrapper rw = {.object = rot, .owned = true, .set_current = set_current_rotate, .hit_noobj = rotate_y_hit_noobj, .bbox = rot->bbox};
			t_vec3 offset = vec3_create(-100.0, 270.0, 395.0);
			t_translate_wrap *tr = translate_create(&rw, &offset);
			if (tr)
			{
				t_hittable_wrapper tw = {.object = tr, .owned = true, .set_current = set_current_translate, .hit_noobj = translate_hit_noobj, .bbox = tr->bbox};
				hittable_list_add_wrapper(&world, &tw);
			}
		}
	}

	t_camera cam;
	cam.aspect_ratio = 1.0;
	cam.image_width = image_width;
	cam.samples_per_pixel = samples_per_pixel;
	cam.max_depth = max_depth;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	cam.vfov = 40.0;
	cam.lookfrom = point3_create(478.0, 278.0, -600.0);
	cam.lookat = point3_create(278.0, 278.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	t_vec3 focus_vec = vec3_sub(&cam.lookfrom, &cam.lookat);
	cam.focus_dist = vec3_length(&focus_vec);

	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
}

int main(void)
{
	switch (9)
	{
	case 1:
		bouncing_spheres();
		break;
	case 2:
		checkered_spheres();
		break;
	case 3:
		earth();
		break;
	case 4:
		perlin_spheres();
		break;
	case 5:
		quads();
		break;
	case 6:
		simple_light();
		break;
	case 7:
		cornell_box();
		break;
	case 8:
		cornell_smoke();
		break;
	case 9:
		final_scene(800, 10000, 40);
		break;
	default:
		final_scene(400, 250, 4);
		break;
	}
	return 0;
}
