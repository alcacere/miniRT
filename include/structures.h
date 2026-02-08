#ifndef STRUCTURES_H
# define STRUCTURES_H

typedef struct s_vec3
{
	double	x;
	double	y;
	double	z;
}	t_vec3;

typedef t_vec3	t_point3;
typedef t_vec3	t_normal;
typedef t_vec3	t_color;

typedef struct s_matrix4
{
	double	elements[4][4];
}	t_matrix4;

typedef struct s_ray
{
	t_point3	origin;
	t_vec3		direction;
}	t_ray;

typedef struct s_posnorm
{
	t_point3	position;
	t_normal	normal;
}	t_posnorm;

#endif
