#ifndef HOUSE_H
#define HOUSE_H

#include "common.h"
#include "triangle.h"
#include "cylinder.h"
#include "bvh.h"

/* ============================================================================ */
/*                          FURNITURE BUILDING HELPERS                          */
/* ============================================================================ */

void build_table_top(t_hittable_list *world, const t_point3 *center,
					 real_t width, real_t depth, real_t thickness, t_material *mat);

void build_cushion(t_hittable_list *world, const t_point3 *center,
				   real_t width, real_t height, real_t depth, t_material *mat);

void build_table_leg(t_hittable_list *world, const t_point3 *base,
					 real_t radius, real_t height, t_material *mat);

void build_lamp_shade(t_hittable_list *world, const t_point3 *apex,
					  real_t angle, real_t height, t_material *mat);

void build_tv_stand(t_hittable_list *world, const t_point3 *pos,
					real_t width, real_t depth, real_t height, t_material *wood_mat);

void build_tv_corner(t_hittable_list *world, const t_point3 *center,
					 real_t width, real_t height, real_t angle_deg,
					 t_material *frame_mat, t_material *screen_mat);

void build_large_window(t_hittable_list *world, const t_point3 *center,
						real_t width, real_t height,
						t_material *frame_mat, t_material *glass_mat);

void build_moon_outside(t_hittable_list *world, const t_point3 *window_center,
						t_material *moon_mat);

void build_stars(t_hittable_list *world, const t_point3 *window_center,
				 real_t width, real_t height, t_material *star_mat);

void build_moonlight(t_hittable_list *world, const t_point3 *window_center,
					 real_t width, real_t height, t_material *light_mat);

void build_side_table(t_hittable_list *world, const t_point3 *pos,
					  t_material *wood_mat, t_material *marble_mat,
					  t_material *lamp_shade_mat, t_material *light_mat);

/* ============================================================================ */
/*                          SCENE CONSTRUCTION                                  */
/* ============================================================================ */

void build_floor(t_hittable_list *world, t_material *floor_mat);

void build_walls(t_hittable_list *world, t_material *wall_mat, t_material *accent_mat);

void build_metallic_sculpture(t_hittable_list *world, const t_point3 *pos,
							  t_material *chrome_mat, t_material *gold_mat, t_material *copper_mat);

void build_glass_coffee_table(t_hittable_list *world, const t_point3 *pos,
							  t_material *glass_mat, t_material *metal_mat);

void build_triangle_decorations(t_hittable_list *world,
								t_material *mat_a, t_material *mat_b, t_material *mat_c);

void build_colored_lamp(t_hittable_list *world, const t_point3 *pos,
						t_material *base_mat, t_material *shade_mat, t_material *light_mat,
						real_t lamp_height);

void build_sofa(t_hittable_list *world, const t_point3 *pos,
				t_material *main_fabric_mat, t_material *accent_fabric_mat,
				t_material *cushion_mat);

void build_plant_pot(t_hittable_list *world, const t_point3 *pos, t_material *pot_mat);

void build_menhir_lamp(t_hittable_list *world, const t_point3 *pos,
					   real_t base_radius, real_t height, int num_lights);

void build_rug(t_hittable_list *world, const t_point3 *center,
			   real_t width, real_t depth, t_material *mat);

void build_mirror_leds(t_hittable_list *world, const t_point3 *mirror_corner,
					   real_t width, real_t height, int num_leds);

#endif