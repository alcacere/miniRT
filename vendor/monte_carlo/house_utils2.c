/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   house_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 18:07:27 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/06 00:57:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "house.h"

/* ============================================================================ */
/*                          SCENE CONSTRUCTION                                  */
/* ============================================================================ */

/* Helper to add a wall quad - simplified version */
static void add_quad_to_world(t_hittable_list *world,
							  real_t px, real_t py, real_t pz,
							  real_t ux, real_t uy, real_t uz,
							  real_t vx, real_t vy, real_t vz,
							  t_material *mat)
{
	t_point3 p = point3_create(px, py, pz);
	t_vec3 u = vec3_create(ux, uy, uz);
	t_vec3 v = vec3_create(vx, vy, vz);

	t_quad q = quad_create(&p, &u, &v, mat);
	t_quad *cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		/* Use hittable_list_add_wrapper with owned=true so the quad persists */
		t_hittable_wrapper wrap = {
			.object = cpy,
			.owned = true,
			.set_current = set_current_quad,
			.hit_noobj = quad_hit_noobj,
			.bbox = q.bbox};
		hittable_list_add_wrapper(world, &wrap);
	}
}

void build_floor(t_hittable_list *world, t_material *floor_mat)
{
	/* Floor quad - facing up (+y normal) */
	t_point3 floor_p = point3_create(-300.0, 0.0, -200.0);
	t_vec3 floor_u = vec3_create(600.0, 0.0, 0.0);
	t_vec3 floor_v = vec3_create(0.0, 0.0, 500.0);
	t_quad floor_q = quad_create(&floor_p, &floor_u, &floor_v, floor_mat);
	t_quad *floor_copy = (t_quad *)malloc(sizeof(t_quad));
	if (floor_copy)
	{
		*floor_copy = floor_q;
		hittable_list_add_nonowned(world, floor_copy, set_current_quad, quad_hit_noobj, &floor_q.bbox);
	}
}

void build_walls(t_hittable_list *world, t_material *wall_mat, t_material *accent_mat)
{
	/* ========== BACK WALL (z=300, facing -z into room) ========== */
	add_quad_to_world(world,
					  250.0, 0.0, 300.0,
					  -500.0, 0.0, 0.0,
					  0.0, 300.0, 0.0,
					  accent_mat);

	/* ========== LEFT WALL WITH MIRROR CUTOUT ========== */
	real_t lx = -250.0;
	real_t mir_y_min = 40.0, mir_y_max = 220.0;
	real_t mir_z_min = -60.0, mir_z_max = 120.0;
	real_t lwall_y_min = 0.0, lwall_y_max = 300.0;
	real_t lwall_z_min = -200.0, lwall_z_max = 300.0;

	/* BOTTOM strip */
	add_quad_to_world(world,
					  lx, lwall_y_min, lwall_z_max,
					  0.0, 0.0, -(lwall_z_max - lwall_z_min),
					  0.0, mir_y_min - lwall_y_min, 0.0,
					  wall_mat);

	/* TOP strip */
	add_quad_to_world(world,
					  lx, mir_y_max, lwall_z_max,
					  0.0, 0.0, -(lwall_z_max - lwall_z_min),
					  0.0, lwall_y_max - mir_y_max, 0.0,
					  wall_mat);

	/* LEFT of mirror (high z side) */
	add_quad_to_world(world,
					  lx, mir_y_min, lwall_z_max,
					  0.0, 0.0, -(lwall_z_max - mir_z_max),
					  0.0, mir_y_max - mir_y_min, 0.0,
					  wall_mat);

	/* RIGHT of mirror (low z side) */
	add_quad_to_world(world,
					  lx, mir_y_min, mir_z_min,
					  0.0, 0.0, -(mir_z_min - lwall_z_min),
					  0.0, mir_y_max - mir_y_min, 0.0,
					  wall_mat);

	/* ========== RIGHT WALL WITH WINDOW CUTOUT ========== */
	real_t rx = 250.0;
	real_t win_y_min = 75.0, win_y_max = 245.0;
	real_t win_z_min = 15.0, win_z_max = 145.0;
	real_t rwall_y_min = 0.0, rwall_y_max = 300.0;
	real_t rwall_z_min = -200.0, rwall_z_max = 300.0;

	/* BOTTOM strip */
	add_quad_to_world(world,
					  rx, rwall_y_min, rwall_z_min,
					  0.0, 0.0, rwall_z_max - rwall_z_min,
					  0.0, win_y_min - rwall_y_min, 0.0,
					  wall_mat);

	/* TOP strip */
	add_quad_to_world(world,
					  rx, win_y_max, rwall_z_min,
					  0.0, 0.0, rwall_z_max - rwall_z_min,
					  0.0, rwall_y_max - win_y_max, 0.0,
					  wall_mat);

	/* LEFT of window (low z side) */
	add_quad_to_world(world,
					  rx, win_y_min, rwall_z_min,
					  0.0, 0.0, win_z_min - rwall_z_min,
					  0.0, win_y_max - win_y_min, 0.0,
					  wall_mat);

	/* RIGHT of window (high z side) */
	add_quad_to_world(world,
					  rx, win_y_min, win_z_max,
					  0.0, 0.0, rwall_z_max - win_z_max,
					  0.0, win_y_max - win_y_min, 0.0,
					  wall_mat);

	/* ========== CEILING ========== */
	add_quad_to_world(world,
					  -250.0, 300.0, 300.0,
					  0.0, 0.0, -500.0,
					  500.0, 0.0, 0.0,
					  wall_mat);
}

void build_metallic_sculpture(t_hittable_list *world, const t_point3 *pos,
							  t_material *chrome_mat, t_material *gold_mat, t_material *copper_mat)
{
	t_point3 c0 = point3_create(pos->x, pos->y + 15.0, pos->z);
	t_sphere s0 = create_sphere(&c0, 12.0, vec3_create(0.9, 0.9, 0.95), chrome_mat);
	hittable_list_add_sphere(world, &s0);

	for (int i = 0; i < 6; ++i)
	{
		real_t angle = (real_t)i * (2.0 * PI / 6.0);
		real_t r = 18.0;
		t_point3 ci = point3_create(
			pos->x + r * (real_t)cos((double)angle),
			pos->y + 15.0 + 5.0 * (real_t)sin((double)(angle * 2)),
			pos->z + r * (real_t)sin((double)angle));
		t_material *mat = (i % 3 == 0) ? gold_mat : ((i % 3 == 1) ? copper_mat : chrome_mat);
		t_sphere si = create_sphere(&ci, 6.0, vec3_create(0.8, 0.7, 0.6), mat);
		hittable_list_add_sphere(world, &si);
	}

	t_point3 top = point3_create(pos->x, pos->y + 32.0, pos->z);
	t_sphere stop = create_sphere(&top, 8.0, vec3_create(0.85, 0.65, 0.2), gold_mat);
	hittable_list_add_sphere(world, &stop);
}

void build_glass_coffee_table(t_hittable_list *world, const t_point3 *pos,
							  t_material *glass_mat, t_material *metal_mat)
{
	(void)metal_mat;
	t_point3 top_center = point3_create(pos->x, pos->y + 35.0, pos->z);
	build_table_top(world, &top_center, 100.0, 60.0, 3.0, glass_mat);

	real_t leg_offsets[4][2] = {{-42.0, -22.0}, {42.0, -22.0}, {-42.0, 22.0}, {42.0, 22.0}};
	for (int i = 0; i < 4; ++i)
	{
		t_point3 leg_base = point3_create(pos->x + leg_offsets[i][0], pos->y, pos->z + leg_offsets[i][1]);
		t_cylinder leg = cylinder_create_y(&leg_base, 4.0, 35.0, glass_mat);
		hittable_list_add_cylinder(world, &leg);
	}

	t_point3 gs1 = point3_create(pos->x - 15.0, pos->y + 44.0, pos->z);
	t_sphere g1 = create_sphere(&gs1, 5.0, vec3_create(1.0, 1.0, 1.0), glass_mat);
	hittable_list_add_sphere(world, &g1);

	t_point3 gs2 = point3_create(pos->x + 10.0, pos->y + 42.0, pos->z - 8.0);
	t_sphere g2 = create_sphere(&gs2, 3.5, vec3_create(1.0, 1.0, 1.0), glass_mat);
	hittable_list_add_sphere(world, &g2);

	t_point3 gs3 = point3_create(pos->x + 5.0, pos->y + 43.5, pos->z + 12.0);
	t_sphere g3 = create_sphere(&gs3, 4.0, vec3_create(1.0, 1.0, 1.0), glass_mat);
	hittable_list_add_sphere(world, &g3);
}

void build_triangle_decorations(t_hittable_list *world,
								t_material *mat_a, t_material *mat_b, t_material *mat_c)
{
	t_mesh mesh;
	mesh_init(&mesh);

	t_point3 c1 = point3_create(120.0, 28.0, 70.0);
	mesh_add_icosahedron(&mesh, &c1, 18.0, mat_a);

	t_point3 b0 = point3_create(-210.0, 0.5, 130.0);
	mesh_add_pyramid(&mesh, &b0, 26.0, 32.0, mat_b);

	t_point3 b1 = point3_create(-210.0, 26.0, 130.0);
	mesh_add_pyramid(&mesh, &b1, 20.0, 26.0, mat_c);

	t_point3 b2 = point3_create(-210.0, 47.0, 130.0);
	mesh_add_pyramid(&mesh, &b2, 14.0, 20.0, mat_b);

	t_point3 c2 = point3_create(-20.0, 52.0, -35.0);
	mesh_add_icosahedron(&mesh, &c2, 8.0, mat_c);

	mesh_add_to_list(&mesh, world);
	mesh_clear(&mesh);
}

void build_colored_lamp(t_hittable_list *world, const t_point3 *pos,
						t_material *base_mat, t_material *shade_mat, t_material *light_mat,
						real_t lamp_height)
{
	t_point3 base = *pos;
	t_cylinder base_cyl = cylinder_create_y(&base, 10.0, 4.0, base_mat);
	hittable_list_add_cylinder(world, &base_cyl);

	t_point3 pole_base = point3_create(pos->x, pos->y + 4.0, pos->z);
	t_cylinder pole = cylinder_create_y(&pole_base, 2.0, lamp_height - 30.0, base_mat);
	hittable_list_add_cylinder(world, &pole);

	t_point3 shade_apex = point3_create(pos->x, pos->y + lamp_height, pos->z);
	build_lamp_shade(world, &shade_apex, 25.0, 20.0, shade_mat);

	t_point3 bulb_center = point3_create(pos->x, pos->y + lamp_height - 12.0, pos->z);
	t_sphere bulb = create_sphere(&bulb_center, 7.0, vec3_create(1.0, 1.0, 1.0), light_mat);
	hittable_list_add_sphere(world, &bulb);
}

void build_sofa(t_hittable_list *world, const t_point3 *pos,
				t_material *main_fabric_mat, t_material *accent_fabric_mat,
				t_material *cushion_mat)
{
	t_point3 base_center = point3_create(pos->x, pos->y + 12.0, pos->z);
	build_cushion(world, &base_center, 170.0, 22.0, 70.0, main_fabric_mat);

	t_point3 back_center = point3_create(pos->x, pos->y + 42.0, pos->z + 30.0);
	build_cushion(world, &back_center, 170.0, 40.0, 15.0, main_fabric_mat);

	t_point3 left_arm = point3_create(pos->x - 78.0, pos->y + 28.0, pos->z);
	build_cushion(world, &left_arm, 15.0, 32.0, 70.0, accent_fabric_mat);

	t_point3 right_arm = point3_create(pos->x + 78.0, pos->y + 28.0, pos->z);
	build_cushion(world, &right_arm, 15.0, 32.0, 70.0, accent_fabric_mat);

	for (int i = -1; i <= 1; ++i)
	{
		t_point3 cushion_pos = point3_create(pos->x + i * 52.0, pos->y + 28.0, pos->z - 8.0);
		build_cushion(world, &cushion_pos, 48.0, 10.0, 52.0, cushion_mat);
	}

	t_material *pillow1_mat = lambertian_create(vec3_create(0.95, 0.75, 0.20));
	t_material *pillow2_mat = lambertian_create(vec3_create(0.85, 0.25, 0.35));

	t_point3 pillow1_pos = point3_create(pos->x - 50.0, pos->y + 40.0, pos->z + 15.0);
	build_cushion(world, &pillow1_pos, 25.0, 25.0, 10.0, pillow1_mat);

	t_point3 pillow2_pos = point3_create(pos->x + 55.0, pos->y + 38.0, pos->z + 12.0);
	build_cushion(world, &pillow2_pos, 22.0, 22.0, 10.0, pillow2_mat);

	t_material *leg_mat = metal_create_fuzz(vec3_create(0.85, 0.85, 0.88), 0.05);
	real_t leg_offsets[4][2] = {{-75.0, -28.0}, {75.0, -28.0}, {-75.0, 28.0}, {75.0, 28.0}};
	for (int i = 0; i < 4; ++i)
	{
		t_point3 leg_base = point3_create(pos->x + leg_offsets[i][0], pos->y, pos->z + leg_offsets[i][1]);
		build_table_leg(world, &leg_base, 2.0, 5.0, leg_mat);
	}
}

void build_plant_pot(t_hittable_list *world, const t_point3 *pos, t_material *pot_mat)
{
	t_cylinder pot = cylinder_create_y(pos, 10.0, 22.0, pot_mat);
	hittable_list_add_cylinder(world, &pot);

	t_material *soil = lambertian_create(vec3_create(0.10, 0.08, 0.04));
	t_point3 soil_center = point3_create(pos->x, pos->y + 20.0, pos->z);
	t_sphere soil_sphere = create_sphere(&soil_center, 8.0, vec3_create(0.10, 0.08, 0.04), soil);
	hittable_list_add_sphere(world, &soil_sphere);

	t_material *leaf = lambertian_create(vec3_create(0.08, 0.32, 0.12));
	for (int i = 0; i < 7; ++i)
	{
		real_t angle = (real_t)i * (2.0 * PI / 7.0);
		real_t r = 7.0 + random_real_interval(-1.5, 1.5);
		real_t h = 26.0 + i * 4.0 + random_real_interval(-2.0, 2.0);
		t_point3 leaf_center = point3_create(
			pos->x + r * cos(angle),
			pos->y + h,
			pos->z + r * sin(angle));
		t_sphere leaf_sphere = create_sphere(&leaf_center, 5.0 + random_real_interval(-0.8, 0.8),
											 vec3_create(0.08, 0.32, 0.12), leaf);
		hittable_list_add_sphere(world, &leaf_sphere);
	}
}

void build_menhir_lamp(t_hittable_list *world, const t_point3 *pos,
					   real_t base_radius, real_t height, int num_lights)
{
	t_material *marble_body = dielectric_create(1.45);
	t_material *marble_solid = lambertian_create(vec3_create(0.92, 0.90, 0.88));

	real_t section_height = height / (real_t)(num_lights + 2);

	t_point3 base_center = *pos;
	t_cylinder base_cyl = cylinder_create_y(&base_center, base_radius * 1.3, section_height * 0.8, marble_solid);
	hittable_list_add_cylinder(world, &base_cyl);

	/* Predefined vibrant colors for variety */
	t_vec3 colors[7] = {
		vec3_create(1.0, 0.2, 0.2), /* Red */
		vec3_create(0.2, 1.0, 0.3), /* Green */
		vec3_create(0.2, 0.4, 1.0), /* Blue */
		vec3_create(1.0, 0.8, 0.1), /* Yellow */
		vec3_create(1.0, 0.3, 0.8), /* Magenta */
		vec3_create(0.1, 0.9, 0.9), /* Cyan */
		vec3_create(1.0, 0.5, 0.1)	/* Orange */
	};

	for (int i = 0; i < num_lights; ++i)
	{
		real_t y_offset = section_height * (1.0 + i);
		real_t taper = 1.0 - (real_t)i * 0.08;
		real_t section_r = base_radius * taper;

		t_point3 section_base = point3_create(pos->x, pos->y + y_offset, pos->z);
		t_cylinder section = cylinder_create_y(&section_base, section_r, section_height * 0.7, marble_body);
		hittable_list_add_cylinder(world, &section);

		/* Use predefined color with high intensity for brightness */
		t_vec3 base_color = colors[i % 7];
		real_t intensity = 35.0 + (real_t)i * 5.0;
		t_vec3 light_color = vec3_create(
			base_color.x * intensity,
			base_color.y * intensity,
			base_color.z * intensity);
		t_material *light_mat = diffuse_light_create(light_color);

		t_point3 light_center = point3_create(pos->x, pos->y + y_offset + section_height * 0.35, pos->z);
		t_sphere light_sphere = create_sphere(&light_center, section_r * 0.6, vec3_create(1.0, 1.0, 1.0), light_mat);
		hittable_list_add_sphere(world, &light_sphere);

		if (i < num_lights - 1)
		{
			t_point3 ring_pos = point3_create(pos->x, pos->y + y_offset + section_height * 0.7, pos->z);
			t_cylinder ring = cylinder_create_y(&ring_pos, section_r * 1.1, section_height * 0.3, marble_solid);
			hittable_list_add_cylinder(world, &ring);
		}
	}

	real_t top_y = pos->y + height - base_radius * 0.4;
	real_t top_r = base_radius * (1.0 - num_lights * 0.08) * 0.8;
	t_point3 dome_center = point3_create(pos->x, top_y, pos->z);
	t_sphere dome = create_sphere(&dome_center, top_r, vec3_create(0.95, 0.93, 0.90), marble_solid);
	hittable_list_add_sphere(world, &dome);
}

/* Build LED lights around mirror frame */
void build_mirror_leds(t_hittable_list *world, const t_point3 *mirror_corner,
					   real_t width, real_t height, int num_leds)
{
	/* LED colors - vibrant neon colors */
	t_vec3 led_colors[6] = {
		vec3_create(1.0, 0.1, 0.1), /* Red */
		vec3_create(0.1, 1.0, 0.2), /* Green */
		vec3_create(0.1, 0.3, 1.0), /* Blue */
		vec3_create(1.0, 0.0, 0.8), /* Pink */
		vec3_create(0.0, 0.8, 1.0), /* Cyan */
		vec3_create(1.0, 0.6, 0.0)	/* Orange */
	};

	real_t led_radius = 3.0;
	real_t led_intensity = 20.0;
	real_t offset_from_wall = 8.0;

	int color_idx = 0;

	/* Mirror is on left wall (x = -248), extending in -z direction for width, +y for height */
	real_t mirror_x = mirror_corner->x;
	real_t mirror_y_min = mirror_corner->y;
	real_t mirror_y_max = mirror_corner->y + height;
	real_t mirror_z_max = mirror_corner->z;
	real_t mirror_z_min = mirror_corner->z - width;

	/* Top edge LEDs */
	for (int i = 0; i < num_leds; ++i)
	{
		real_t t = (real_t)i / (real_t)(num_leds - 1);
		t_point3 led_pos = point3_create(
			mirror_x + offset_from_wall,
			mirror_y_max + 5.0,
			mirror_z_max - t * width);

		t_vec3 c = led_colors[color_idx % 6];
		t_vec3 emit = vec3_create(c.x * led_intensity, c.y * led_intensity, c.z * led_intensity);
		t_material *led_mat = diffuse_light_create(emit);
		t_sphere led = create_sphere(&led_pos, led_radius, vec3_create(1.0, 1.0, 1.0), led_mat);
		hittable_list_add_sphere(world, &led);
		color_idx++;
	}

	/* Bottom edge LEDs */
	for (int i = 0; i < num_leds; ++i)
	{
		real_t t = (real_t)i / (real_t)(num_leds - 1);
		t_point3 led_pos = point3_create(
			mirror_x + offset_from_wall,
			mirror_y_min - 5.0,
			mirror_z_max - t * width);

		t_vec3 c = led_colors[color_idx % 6];
		t_vec3 emit = vec3_create(c.x * led_intensity, c.y * led_intensity, c.z * led_intensity);
		t_material *led_mat = diffuse_light_create(emit);
		t_sphere led = create_sphere(&led_pos, led_radius, vec3_create(1.0, 1.0, 1.0), led_mat);
		hittable_list_add_sphere(world, &led);
		color_idx++;
	}

	/* Left edge LEDs (vertical) */
	int num_vertical = (int)(num_leds * height / width);
	if (num_vertical < 3)
		num_vertical = 3;
	for (int i = 0; i < num_vertical; ++i)
	{
		real_t t = (real_t)i / (real_t)(num_vertical - 1);
		t_point3 led_pos = point3_create(
			mirror_x + offset_from_wall,
			mirror_y_min + t * height,
			mirror_z_max + 5.0);

		t_vec3 c = led_colors[color_idx % 6];
		t_vec3 emit = vec3_create(c.x * led_intensity, c.y * led_intensity, c.z * led_intensity);
		t_material *led_mat = diffuse_light_create(emit);
		t_sphere led = create_sphere(&led_pos, led_radius, vec3_create(1.0, 1.0, 1.0), led_mat);
		hittable_list_add_sphere(world, &led);
		color_idx++;
	}

	/* Right edge LEDs (vertical) */
	for (int i = 0; i < num_vertical; ++i)
	{
		real_t t = (real_t)i / (real_t)(num_vertical - 1);
		t_point3 led_pos = point3_create(
			mirror_x + offset_from_wall,
			mirror_y_min + t * height,
			mirror_z_min - 5.0);

		t_vec3 c = led_colors[color_idx % 6];
		t_vec3 emit = vec3_create(c.x * led_intensity, c.y * led_intensity, c.z * led_intensity);
		t_material *led_mat = diffuse_light_create(emit);
		t_sphere led = create_sphere(&led_pos, led_radius, vec3_create(1.0, 1.0, 1.0), led_mat);
		hittable_list_add_sphere(world, &led);
		color_idx++;
	}
}

void build_rug(t_hittable_list *world, const t_point3 *center,
			   real_t width, real_t depth, t_material *mat)
{
	t_point3 rug_p = point3_create(center->x - width / 2.0, center->y + 0.2, center->z - depth / 2.0);
	t_vec3 rug_u = vec3_create(width, 0.0, 0.0);
	t_vec3 rug_v = vec3_create(0.0, 0.0, depth);
	t_quad rug_q = quad_create(&rug_p, &rug_u, &rug_v, mat);
	t_quad *rug_copy = (t_quad *)malloc(sizeof(t_quad));
	if (rug_copy)
	{
		*rug_copy = rug_q;
		hittable_list_add_nonowned(world, rug_copy, set_current_quad, quad_hit_noobj, &rug_q.bbox);
	}
}