#ifndef MINIRT_H
# define MINIRT_H

# include "libft.h"
# include "structures.h"
# include <math.h>
# include <stdint.h>

# ifndef INFINITY
#  define INFINITY (1e30)
# endif

t_vec3		vec3_create(double x, double y, double z);
t_vec3		vec3_add(t_vec3 v1, t_vec3 v2);
t_vec3		vec3_sub(t_vec3 v1, t_vec3 v2);
t_vec3		vec3_scale(t_vec3 v, double scale);
t_vec3		vec3_mul(t_vec3 v1, t_vec3 v2);
double		vec3_dot(t_vec3 v1, t_vec3 v2);
t_vec3		vec3_cross(t_vec3 v1, t_vec3 v2);
double		vec3_length(t_vec3 v);
t_vec3		vec3_normalize(t_vec3 v);

t_interval	interval_create(double min, double max);
t_ray		ray_create(t_point3 origin, t_vec3 direction, double tm);
double		ft_atof(const char *str);

void		object_add_back(t_object **head, t_object *node);
void		ligth_add_back(t_light **head, t_light *node);
void		free_scene(t_scene *scene);
void		object_free(t_object *head);
void		light_free(t_light *head);

struct s_hit_record;
void		set_face_normal(struct s_hit_record *hit, const t_ray *r, \
							const t_vec3 *outward_normal);


double	random_double(uint32_t *seed);
double	random_double_range(uint32_t *seed, double min, double max);
t_vec3	random_vec3(uint32_t *seed);
t_vec3	random_vec3_range(uint32_t *seed, double min, double max);
t_vec3	random_in_unit_sphere(uint32_t *seed);
t_vec3	random_unit_vector(uint32_t *seed);

t_vec3	reflect(t_vec3 v, t_vec3 n);
t_vec3	refract(t_vec3 uv, t_vec3 n, double etai_over_etat);
double	reflectance(double cosine, double ref_idx);

int		hit_aabb(const t_aabb *box, const t_ray *r, t_interval rayt);
t_aabb	aabb_merge(t_aabb box0, t_aabb box1);
void aabb_pad(t_aabb *box);

#endif