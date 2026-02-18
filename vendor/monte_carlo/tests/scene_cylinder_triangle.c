/* ============================================================================ */
/*                                                                              */
/*  Modern Living Room Scene - Nighttime Atmosphere                             */
/*  Built using cylinders, triangles, spheres, quads, and cones                 */
/*                                                                              */
/* ============================================================================ */

#include "../house.h"

/* ============================================================================ */
/*                          MAIN SCENE FUNCTION                                 */
/* ============================================================================ */

void scene_cylinder_triangle(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* ===== LIGHT INTENSITY CONTROL ===== */
	const real_t LIGHT_SCALE = 2.0;

	/* ===== MATERIALS ===== */

	t_material *floor_mat = lambertian_create(vec3_create(0.55, 0.45, 0.35));
	t_material *wall_mat = lambertian_create(vec3_create(0.72, 0.68, 0.62));
	t_material *accent_mat = lambertian_create(vec3_create(0.78, 0.50, 0.42));

	t_material *sofa_main = lambertian_create(vec3_create(0.08, 0.42, 0.45));
	t_material *sofa_accent = lambertian_create(vec3_create(0.06, 0.35, 0.38));
	t_material *sofa_cushion = lambertian_create(vec3_create(0.10, 0.48, 0.50));

	t_material *thick_glass = dielectric_create(1.55);
	t_material *chrome = metal_create_fuzz(vec3_create(0.9, 0.9, 0.92), 0.02);
	t_material *gold = metal_create_fuzz(vec3_create(0.85, 0.65, 0.15), 0.08);
	t_material *copper = metal_create_fuzz(vec3_create(0.72, 0.45, 0.35), 0.1);
	t_material *brushed_aluminum = metal_create_fuzz(vec3_create(0.6, 0.6, 0.62), 0.15);

	t_material *marble_mat = lambertian_create(vec3_create(0.92, 0.90, 0.88));
	t_material *shade_mat = lambertian_create(vec3_create(0.95, 0.92, 0.85));
	t_material *frame_mat = lambertian_create(vec3_create(0.15, 0.12, 0.10));
	t_material *window_glass = dielectric_create(1.52);

	/* Lights */
	t_vec3 orange_color = vec3_create(25.0 * LIGHT_SCALE, 12.0 * LIGHT_SCALE, 6.0 * LIGHT_SCALE);
	t_material *orange_light = diffuse_light_create(orange_color);

	t_vec3 magenta_color = vec3_create(18.0 * LIGHT_SCALE, 8.0 * LIGHT_SCALE, 14.0 * LIGHT_SCALE);
	t_material *magenta_light = diffuse_light_create(magenta_color);

	t_vec3 cyan_color = vec3_create(10.0 * LIGHT_SCALE, 14.0 * LIGHT_SCALE, 18.0 * LIGHT_SCALE);
	t_material *cyan_light = diffuse_light_create(cyan_color);

	/* TV screen */
	t_texture *tv_tex = image_texture_create_png("/home/dlesieur/Documents/minirt/learn_path/output/images/country.ppm.png");
	t_vec3 tv_color = vec3_create(6.0 * LIGHT_SCALE, 8.0 * LIGHT_SCALE, 12.0 * LIGHT_SCALE);
	const real_t TV_EMIT_SCALE = (real_t)2.2;
	t_material *tv_screen = tv_tex
								? diffuse_light_create_texture_scaled(tv_tex, TV_EMIT_SCALE)
								: diffuse_light_create_scaled(tv_color, TV_EMIT_SCALE);
	t_material *tv_frame = metal_create_fuzz(vec3_create(0.05, 0.05, 0.05), 0.4);

	t_material *dark_wood = lambertian_create(vec3_create(0.18, 0.12, 0.08));
	t_material *walnut_wood = lambertian_create(vec3_create(0.25, 0.15, 0.10));
	t_material *rug_mat = lambertian_create(vec3_create(0.45, 0.25, 0.20));
	t_material *pot_mat = lambertian_create(vec3_create(0.6, 0.35, 0.25));

	t_material *star_mat = diffuse_light_create(vec3_create(40.0, 40.0, 50.0));
	t_vec3 moon_color = vec3_create(10.0 * LIGHT_SCALE, 10.0 * LIGHT_SCALE, 9.0 * LIGHT_SCALE);
	t_material *moon_mat = diffuse_light_create(moon_color);
	t_vec3 moonlight_color = vec3_create(8.0 * LIGHT_SCALE, 9.0 * LIGHT_SCALE, 10.0 * LIGHT_SCALE);
	t_material *moonlight = diffuse_light_create(moonlight_color);
	t_material *mirror_mat = metal_create_fuzz(vec3_create(0.98, 0.98, 0.98), 0.005);

	/* ===== BUILD SCENE ===== */

	build_floor(&world, floor_mat);
	build_walls(&world, wall_mat, accent_mat);

	t_point3 sofa_pos = point3_create(-20.0, 0.0, 180.0);
	build_sofa(&world, &sofa_pos, sofa_main, sofa_accent, sofa_cushion);

	t_point3 table_pos = point3_create(-20.0, 0.0, 60.0);
	build_glass_coffee_table(&world, &table_pos, thick_glass, chrome);

	t_point3 rug_center = point3_create(-20.0, 0.0, 70.0);
	build_rug(&world, &rug_center, 140.0, 100.0, rug_mat);

	t_point3 lamp2_pos = point3_create(150.0, 0.0, 250.0);
	build_colored_lamp(&world, &lamp2_pos, brushed_aluminum, shade_mat, magenta_light, 120.0);

	t_point3 lamp3_pos = point3_create(180.0, 0.0, 50.0);
	build_colored_lamp(&world, &lamp3_pos, chrome, shade_mat, cyan_light, 100.0);

	/* TV stand and TV - MOVED FURTHER BACK */
	t_point3 tv_stand_pos = point3_create(-20.0, 0.0, -120.0);
	build_tv_stand(&world, &tv_stand_pos, 140.0, 50.0, 45.0, dark_wood);

	t_point3 tv_center = point3_create(-20.0, 80.0, -118.0);
	build_tv_corner(&world, &tv_center, 90.0, 54.0, 0.0, tv_frame, tv_screen);

	t_point3 side_table_pos = point3_create(100.0, 0.0, 160.0);
	build_side_table(&world, &side_table_pos, walnut_wood, marble_mat, shade_mat, orange_light);

	t_point3 window_center = point3_create(248.0, 160.0, 80.0);
	build_large_window(&world, &window_center, 130.0, 170.0, frame_mat, window_glass);

	build_moon_outside(&world, &window_center, moon_mat);
	build_stars(&world, &window_center, 130.0, 170.0, star_mat);
	build_moonlight(&world, &window_center, 130.0, 170.0, moonlight);

	/* Mirror with LED frame - positioned to align with wall cutout */
	/* Wall cutout: z from -60 to 120, y from 40 to 220 */
	/* Mirror should be at x=-248 (slightly in front of wall at -250) */
	t_point3 mirror_p = point3_create(-248.0, 40.0, 120.0); /* corner: y=40, z=120 (top-left of cutout) */
	t_vec3 mirror_u = vec3_create(0.0, 0.0, -180.0);		/* extends -z by 180: from z=120 to z=-60 */
	t_vec3 mirror_v = vec3_create(0.0, 180.0, 0.0);			/* extends +y by 180: from y=40 to y=220 */
	t_quad mirror_q = quad_create(&mirror_p, &mirror_u, &mirror_v, mirror_mat);
	t_quad *mirror_copy = (t_quad *)malloc(sizeof(t_quad));
	if (mirror_copy)
	{
		*mirror_copy = mirror_q;
		hittable_list_add_nonowned(&world, mirror_copy, set_current_quad, quad_hit_noobj, &mirror_q.bbox);
	}

	/* Add colored LEDs around the mirror - must match mirror dimensions */
	build_mirror_leds(&world, &mirror_p, 180.0, 180.0, 8);

	t_point3 menhir_pos = point3_create(-200.0, 0.0, 200.0);
	build_menhir_lamp(&world, &menhir_pos, 12.0, 100.0, 5);

	t_point3 plant_pos = point3_create(180.0, 0.0, 180.0);
	build_plant_pot(&world, &plant_pos, pot_mat);

	t_point3 sculpture_pos = point3_create(-180.0, 0.0, 80.0);
	build_metallic_sculpture(&world, &sculpture_pos, chrome, gold, copper);

	build_triangle_decorations(&world,
							   metal_create_fuzz(vec3_create(0.90, 0.90, 0.92), 0.03),
							   metal_create_fuzz(vec3_create(0.85, 0.65, 0.15), 0.10),
							   lambertian_create(vec3_create(0.12, 0.40, 0.55)));

	/* Build BVH */
	t_bvh_node *world_bvh = bvh_node_create(&world);
	t_hittable_list accel;
	hittable_list_init(&accel);
	if (world_bvh)
	{
		t_hittable_wrapper bvh_wrap = {
			.object = world_bvh,
			.owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit,
			.bbox = world_bvh->bbox};
		hittable_list_add_wrapper(&accel, &bvh_wrap);
	}

	/* ===== CAMERA ===== */
	t_camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	cam.vfov = 80.0;
	cam.lookfrom = point3_create(80.0, 80.0, 140.0);
	cam.lookat = point3_create(-400.0, 70.0, -100.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	t_vec3 focus_vec = vec3_sub(&cam.lookfrom, &cam.lookat);
	cam.focus_dist = vec3_length(&focus_vec);

	camera_init(&cam, cam.aspect_ratio, cam.image_width);

	const t_hittable_list *render_world = world_bvh ? &accel : &world;
	camera_render(&cam, stdout, render_world);

	hittable_list_clear(&accel);
	hittable_list_clear(&world);
}

int main(void)
{
	scene_cylinder_triangle();
	return 0;
}