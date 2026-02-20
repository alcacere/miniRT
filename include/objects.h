#ifndef OBJECTS_H
# define OBJECTS_H

# include "hittable.h"
# include "structures.h"

typedef struct s_hittable_list
{
	t_hittable	**elements;
	int			count;
}	t_hittable_list;

t_hittable	*build_world(t_object *objects);

int			hit_list(const void *obj, const t_ray *r, \
						t_interval rayt, t_hit_record *rec);

t_hittable	*create_hittable_sphere(t_object *obj);
t_hittable	*create_hittable_plane(t_object *obj);
t_hittable	*create_hittable_cylinder(t_object *obj);

#endif