/* ============================================================================ */
/*                          FURNITURE BUILDING HELPERS                          */
/* ============================================================================ */

#include "house.h"

/* Build a rectangular table/bench top from two triangles */
void build_table_top(t_hittable_list *world, const t_point3 *center,
					 real_t width, real_t depth, real_t thickness, t_material *mat)
{
	real_t hw = width / 2.0;
	real_t hd = depth / 2.0;
	real_t ht = thickness / 2.0;

	t_point3 t0 = point3_create(center->x - hw, center->y + ht, center->z - hd);
	t_point3 t1 = point3_create(center->x + hw, center->y + ht, center->z - hd);
	t_point3 t2 = point3_create(center->x + hw, center->y + ht, center->z + hd);
	t_point3 t3 = point3_create(center->x - hw, center->y + ht, center->z + hd);

	t_triangle tri1 = triangle_create(&t0, &t1, &t2, mat);
	t_triangle tri2 = triangle_create(&t0, &t2, &t3, mat);
	hittable_list_add_triangle(world, &tri1);
	hittable_list_add_triangle(world, &tri2);

	t_point3 b0 = point3_create(center->x - hw, center->y - ht, center->z - hd);
	t_point3 b1 = point3_create(center->x + hw, center->y - ht, center->z - hd);
	t_point3 b2 = point3_create(center->x + hw, center->y - ht, center->z + hd);
	t_point3 b3 = point3_create(center->x - hw, center->y - ht, center->z + hd);

	t_triangle tri3 = triangle_create(&b0, &b2, &b1, mat);
	t_triangle tri4 = triangle_create(&b0, &b3, &b2, mat);
	hittable_list_add_triangle(world, &tri3);
	hittable_list_add_triangle(world, &tri4);

	t_triangle sf1 = triangle_create(&t0, &b0, &b1, mat);
	t_triangle sf2 = triangle_create(&t0, &b1, &t1, mat);
	hittable_list_add_triangle(world, &sf1);
	hittable_list_add_triangle(world, &sf2);

	t_triangle sb1 = triangle_create(&t2, &b2, &b3, mat);
	t_triangle sb2 = triangle_create(&t2, &b3, &t3, mat);
	hittable_list_add_triangle(world, &sb1);
	hittable_list_add_triangle(world, &sb2);

	t_triangle sl1 = triangle_create(&t0, &t3, &b3, mat);
	t_triangle sl2 = triangle_create(&t0, &b3, &b0, mat);
	hittable_list_add_triangle(world, &sl1);
	hittable_list_add_triangle(world, &sl2);

	t_triangle sr1 = triangle_create(&t1, &b1, &b2, mat);
	t_triangle sr2 = triangle_create(&t1, &b2, &t2, mat);
	hittable_list_add_triangle(world, &sr1);
	hittable_list_add_triangle(world, &sr2);
}

void build_cushion(t_hittable_list *world, const t_point3 *center,
				   real_t width, real_t height, real_t depth, t_material *mat)
{
	t_mesh cushion_mesh;
	mesh_init(&cushion_mesh);
	t_point3 min = point3_create(center->x - width / 2, center->y - height / 2, center->z - depth / 2);
	t_point3 max = point3_create(center->x + width / 2, center->y + height / 2, center->z + depth / 2);
	mesh_add_box(&cushion_mesh, &min, &max, mat);
	mesh_add_to_list(&cushion_mesh, world);
	mesh_clear(&cushion_mesh);
}

void build_table_leg(t_hittable_list *world, const t_point3 *base,
					 real_t radius, real_t height, t_material *mat)
{
	t_cylinder leg = cylinder_create_y(base, radius, height, mat);
	hittable_list_add_cylinder(world, &leg);
}

void build_lamp_shade(t_hittable_list *world, const t_point3 *apex,
					  real_t angle, real_t height, t_material *mat)
{
	t_vec3 down = vec3_create(0.0, -1.0, 0.0);
	t_cone shade = cone_create(apex, &down, angle, height, mat);
	hittable_list_add_cone(world, &shade);
}

void build_tv_stand(t_hittable_list *world, const t_point3 *pos,
					real_t width, real_t depth, real_t height, t_material *wood_mat)
{
	t_point3 top_center = point3_create(pos->x, pos->y + height - 2.0, pos->z);
	build_table_top(world, &top_center, width, depth, 4.0, wood_mat);

	real_t leg_inset = 5.0;
	real_t leg_offsets[4][2] = {
		{-width / 2 + leg_inset, -depth / 2 + leg_inset},
		{width / 2 - leg_inset, -depth / 2 + leg_inset},
		{-width / 2 + leg_inset, depth / 2 - leg_inset},
		{width / 2 - leg_inset, depth / 2 - leg_inset}};

	for (int i = 0; i < 4; ++i)
	{
		t_point3 leg_base = point3_create(pos->x + leg_offsets[i][0], pos->y, pos->z + leg_offsets[i][1]);
		build_table_leg(world, &leg_base, 3.0, height - 4.0, wood_mat);
	}

	t_point3 shelf_center = point3_create(pos->x, pos->y + height / 2, pos->z);
	build_table_top(world, &shelf_center, width - 10.0, depth - 10.0, 3.0, wood_mat);
}

void build_tv_corner(t_hittable_list *world, const t_point3 *center,
					 real_t width, real_t height, real_t angle_deg,
					 t_material *frame_mat, t_material *screen_mat)
{
	real_t angle_rad = degrees_to_radians(angle_deg);
	real_t cos_a = (real_t)cos((double)angle_rad);
	real_t sin_a = (real_t)sin((double)angle_rad);

	t_vec3 screen_right = vec3_create(cos_a, 0.0, -sin_a);
	t_vec3 screen_up = vec3_create(0.0, 1.0, 0.0);
	t_vec3 screen_normal = cross(&screen_right, &screen_up);

	t_vec3 right_offset = vec3_mul_scalar(&screen_right, width / 2.0);
	t_vec3 up_offset = vec3_mul_scalar(&screen_up, height / 2.0);

	t_point3 screen_p = vec3_sub(center, &right_offset);
	screen_p = vec3_sub(&screen_p, &up_offset);

	t_vec3 screen_u = vec3_mul_scalar(&screen_right, width);
	t_vec3 screen_v = vec3_mul_scalar(&screen_up, height);

	/* Front emissive screen */
	t_quad screen_q = quad_create(&screen_p, &screen_u, &screen_v, screen_mat);
	t_quad *screen_copy = (t_quad *)malloc(sizeof(t_quad));
	if (screen_copy)
	{
		*screen_copy = screen_q;
		hittable_list_add_nonowned(world, screen_copy, set_current_quad, quad_hit_noobj, &screen_q.bbox);
	}

	/* RESTORED: Back panel to block light from behind TV */
	t_material *back_mat = lambertian_create(vec3_create(0.02, 0.02, 0.02));
	t_vec3 back_offset = vec3_mul_scalar(&screen_normal, -5.0);
	t_point3 back_corner = vec3_add(&screen_p, &back_offset);
	/* Swap u and v to flip normal direction */
	t_quad back_q = quad_create(&back_corner, &screen_v, &screen_u, back_mat);
	t_quad *back_copy = (t_quad *)malloc(sizeof(t_quad));
	if (back_copy)
	{
		*back_copy = back_q;
		hittable_list_add_nonowned(world, back_copy, set_current_quad, quad_hit_noobj, &back_q.bbox);
	}

	/* TV frame bezel */
	real_t bezel_r = 2.5;
	t_point3 corners[4];
	corners[0] = screen_p;
	corners[1] = vec3_add(&screen_p, &screen_u);
	corners[2] = vec3_add(&corners[1], &screen_v);
	corners[3] = vec3_add(&screen_p, &screen_v);

	t_cylinder top_cyl = cylinder_create(&corners[3], &screen_right, bezel_r, width, frame_mat);
	hittable_list_add_cylinder(world, &top_cyl);

	t_cylinder bot_cyl = cylinder_create(&corners[0], &screen_right, bezel_r, width, frame_mat);
	hittable_list_add_cylinder(world, &bot_cyl);

	t_cylinder left_cyl = cylinder_create(&corners[0], &screen_up, bezel_r, height, frame_mat);
	hittable_list_add_cylinder(world, &left_cyl);

	t_cylinder right_cyl = cylinder_create(&corners[1], &screen_up, bezel_r, height, frame_mat);
	hittable_list_add_cylinder(world, &right_cyl);
}

void build_large_window(t_hittable_list *world, const t_point3 *center,
						real_t width, real_t height,
						t_material *frame_mat, t_material *glass_mat)
{
	real_t bar_r = 3.0;
	t_vec3 up = vec3_create(0.0, 1.0, 0.0);
	t_vec3 horiz = vec3_create(0.0, 0.0, 1.0);

	t_point3 left_bar = point3_create(center->x, center->y - height / 2, center->z - width / 2);
	t_cylinder left_cyl = cylinder_create(&left_bar, &up, bar_r, height, frame_mat);
	hittable_list_add_cylinder(world, &left_cyl);

	t_point3 right_bar = point3_create(center->x, center->y - height / 2, center->z + width / 2);
	t_cylinder right_cyl = cylinder_create(&right_bar, &up, bar_r, height, frame_mat);
	hittable_list_add_cylinder(world, &right_cyl);

	t_point3 top_bar = point3_create(center->x, center->y + height / 2, center->z - width / 2);
	t_cylinder top_cyl = cylinder_create(&top_bar, &horiz, bar_r, width, frame_mat);
	hittable_list_add_cylinder(world, &top_cyl);

	t_point3 bot_bar = point3_create(center->x, center->y - height / 2, center->z - width / 2);
	t_cylinder bot_cyl = cylinder_create(&bot_bar, &horiz, bar_r, width, frame_mat);
	hittable_list_add_cylinder(world, &bot_cyl);

	t_point3 vcenter_bar = point3_create(center->x, center->y - height / 2, center->z);
	t_cylinder vcenter_cyl = cylinder_create(&vcenter_bar, &up, bar_r * 0.6, height, frame_mat);
	hittable_list_add_cylinder(world, &vcenter_cyl);

	t_point3 hcenter_bar = point3_create(center->x, center->y, center->z - width / 2);
	t_cylinder hcenter_cyl = cylinder_create(&hcenter_bar, &horiz, bar_r * 0.6, width, frame_mat);
	hittable_list_add_cylinder(world, &hcenter_cyl);

	if (!glass_mat)
		return;

	t_point3 glass_p = point3_create(center->x - 1.0, center->y - height / 2 + 5.0, center->z - width / 2 + 5.0);
	t_vec3 glass_u = vec3_create(0.0, 0.0, width - 10.0);
	t_vec3 glass_v = vec3_create(0.0, height - 10.0, 0.0);
	t_quad glass_q = quad_create(&glass_p, &glass_u, &glass_v, glass_mat);
	t_quad *glass_copy = (t_quad *)malloc(sizeof(t_quad));
	if (glass_copy)
	{
		*glass_copy = glass_q;
		hittable_list_add_nonowned(world, glass_copy, set_current_quad, quad_hit_noobj, &glass_q.bbox);
	}
}

void build_moon_outside(t_hittable_list *world, const t_point3 *window_center,
						t_material *moon_mat)
{
	t_point3 moon_pos = point3_create(
		window_center->x + 500.0,
		window_center->y + 120.0,
		window_center->z + 30.0);
	t_sphere moon = create_sphere(&moon_pos, 50.0, vec3_create(1.0, 1.0, 0.95), moon_mat);
	hittable_list_add_sphere(world, &moon);
}

void build_stars(t_hittable_list *world, const t_point3 *window_center,
				 real_t width, real_t height, t_material *star_mat)
{
	real_t star_distance = 800.0;
	for (int i = 0; i < 40; ++i)
	{
		real_t offset_z = random_real_interval(-width * 1.5, width * 1.5);
		real_t offset_y = random_real_interval(-height * 0.3, height * 1.2);
		real_t star_size = random_real_interval(0.8, 2.0);
		t_point3 star_pos = point3_create(
			window_center->x + star_distance,
			window_center->y + offset_y,
			window_center->z + offset_z);
		t_sphere star = create_sphere(&star_pos, star_size, vec3_create(1.0, 1.0, 1.0), star_mat);
		hittable_list_add_sphere(world, &star);
	}
}

void build_moonlight(t_hittable_list *world, const t_point3 *window_center,
					 real_t width, real_t height, t_material *light_mat)
{
	t_point3 p = point3_create(window_center->x + 20.0, window_center->y - height / 2 + 10, window_center->z - width / 2 + 10);
	t_vec3 u = vec3_create(0.0, 0.0, width - 20);
	t_vec3 v = vec3_create(0.0, height - 20, 0.0);
	t_quad q = quad_create(&p, &u, &v, light_mat);
	t_quad *copy = (t_quad *)malloc(sizeof(t_quad));
	if (copy)
	{
		*copy = q;
		hittable_list_add_nonowned(world, copy, set_current_quad, quad_hit_noobj, &q.bbox);
	}
}

void build_side_table(t_hittable_list *world, const t_point3 *pos,
					  t_material *wood_mat, t_material *marble_mat,
					  t_material *lamp_shade_mat, t_material *light_mat)
{
	t_point3 top_center = point3_create(pos->x, pos->y + 45.0, pos->z);
	build_table_top(world, &top_center, 40.0, 40.0, 3.0, wood_mat);

	real_t offsets[4][2] = {{-15.0, -15.0}, {15.0, -15.0}, {-15.0, 15.0}, {15.0, 15.0}};
	for (int i = 0; i < 4; ++i)
	{
		t_point3 leg_base = point3_create(pos->x + offsets[i][0], pos->y, pos->z + offsets[i][1]);
		build_table_leg(world, &leg_base, 2.0, 45.0, wood_mat);
	}

	t_point3 lamp_base = point3_create(pos->x, pos->y + 47.0, pos->z);
	t_cylinder base_cyl = cylinder_create_y(&lamp_base, 6.0, 10.0, marble_mat);
	hittable_list_add_cylinder(world, &base_cyl);

	t_point3 stem_base = point3_create(pos->x, pos->y + 57.0, pos->z);
	t_cylinder stem = cylinder_create_y(&stem_base, 2.0, 20.0, marble_mat);
	hittable_list_add_cylinder(world, &stem);

	t_point3 shade_apex = point3_create(pos->x, pos->y + 85.0, pos->z);
	t_vec3 down = vec3_create(0.0, -1.0, 0.0);
	t_cone shade = cone_create(&shade_apex, &down, 22.0, 15.0, lamp_shade_mat);
	hittable_list_add_cone(world, &shade);

	t_point3 bulb_center = point3_create(pos->x, pos->y + 75.0, pos->z);
	t_sphere bulb = create_sphere(&bulb_center, 5.0, vec3_create(1.0, 1.0, 1.0), light_mat);
	hittable_list_add_sphere(world, &bulb);
}
