#ifndef LIGHT_H
# define LIGHT_H

# include "minirt.h"
# include "hittable.h"

t_vec3	reflect_vector(t_vec3 v, t_vec3 n);
int		is_in_shadow(t_hittable *world, t_point3 hit_p, t_light *light);
t_color	calculate_lighting(t_scene *scene, t_hittable *world, \
							t_hit_record *rec, t_ray *cam_ray);

#endif