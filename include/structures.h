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

typedef struct s_material
{
	t_color	color;
	double	diffuse;
	double	specular;
	double	shininess;
	int		is_checkerboard; 
}	t_material;

typedef struct s_sphere
{
	t_point3	center;
	double		radius;
}	t_sphere;

typedef struct s_plane
{
	t_point3	point;
	t_normal	normal;
}	t_plane;

typedef struct s_cylinder
{
	t_point3	center;
	t_normal	axis;
	double		radius;
	double		height;
}	t_cylinder;

typedef struct s_cone
{
	t_point3	center;
	t_normal	axis;
	double		radius;
	double		height;
}	t_cone;

typedef enum e_obj_type
{
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER,
	OBJ_CONE
}	t_obj_type;

typedef struct s_object
{
	t_obj_type		type;
	void			*shape;
	t_material		material;
	struct s_object	*next;
}	t_object;

typedef struct s_ambient
{
	double	ratio;
	t_color	color;
}	t_ambient;

typedef struct s_light
{
	t_point3		position;
	double			brightness;
	t_color			color;
	struct s_light	*next;
}	t_light;

typedef struct s_camera
{
	t_point3	origin;
	t_vec3		direction;
	double		fov;
	t_vec3		horizontal;
	t_vec3		vertical;
	t_point3	lower_left_corner;
	t_vec3		u;
	t_vec3		v;
	t_vec3		w; 
}	t_camera;

typedef struct s_scene
{
	t_ambient	ambient;
	t_camera	camera;
	t_light		*lights;
	t_object	*objects; 
}	t_scene;

typedef struct s_minirt
{
	void	*mlx;
	void	*win;
	t_scene	scene;
}	t_minirt;

#endif
