#ifndef MINIRT_H
# define MINIRT_H

# include "structures.h"
# include "macros.h"
# include <math.h>

/* math/vec3_basic.c */
t_vec3		vec3_init(double x, double y, double z);
t_vec3		vec3_add(t_vec3 v1, t_vec3 v2);
t_vec3		vec3_sub(t_vec3 v1, t_vec3 v2);
t_vec3		vec3_scale(t_vec3 v, double scale);
t_vec3		vec3_mul(t_vec3 v1, t_vec3 v2);

/* math/vec3_algebra.c */
double		vec3_dot(t_vec3 v1, t_vec3 v2);
t_vec3		vec3_cross(t_vec3 v1, t_vec3 v2);
double		vec3_length(t_vec3 v);
t_vec3		vec3_normalize(t_vec3 v);

/* math/matrix_ops.c */
void		matrix4_set_identity(t_matrix4 *mat);
t_matrix4	matrix4_mul(const t_matrix4 *a, const t_matrix4 *b);
t_vec3		matrix4_mul_vec3(const t_matrix4 *mat, const t_vec3 *v);
t_point3	matrix4_mul_point3(const t_matrix4 *mat, const t_point3 *p);

/* list utils and cleanup */
void	object_add_back(t_object **head, t_object *node);
void	object_free(t_object *head);

#endif
