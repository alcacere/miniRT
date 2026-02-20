#include "objects.h"

int	hit_list(const void *obj, const t_ray *r, 
	t_interval rayt, t_hit_record *rec)
{
	t_hittable_list	*list;
	t_hit_record	temp_rec;
	int				hit_anything;
	double			closest_so_far;
	int				i;

	list = (t_hittable_list *)obj;
	hit_anything = 0;
	closest_so_far = rayt.max;
	i = 0;
	while (i < list->count)
	{
		if (list->elements[i]->hit(list->elements[i]->object, r, 
			interval_create(rayt.min, closest_so_far), &temp_rec))
		{
			hit_anything = 1;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
		}
		i++;
	}
	return (hit_anything);
}