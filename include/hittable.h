#ifndef HITTABLE_H
# define HITTABLE_H

# include "minirt.h"

typedef struct s_hit_record	t_hit_record;
typedef struct s_material	t_material;

struct s_hit_record
{
	t_vec3		p;
	t_vec3		normal;
	double		t;
	int			front_face;
	t_vec3		albedo;
	double		u;
	double		v;
	t_material	*mat;
};

typedef int	(*t_hit_fn)(const void *obj, const t_ray *r, \
						t_interval rayt, t_hit_record *rec);

typedef struct s_hittable
{
	void		*object;
	t_hit_fn	hit;
}	t_hittable;

#endif