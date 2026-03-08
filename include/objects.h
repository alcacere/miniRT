/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   objects.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:13:12 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 16:13:13 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECTS_H
# define OBJECTS_H

# include "hittable.h"
# include "structures.h"

typedef struct s_cone_data
{
	t_vec3	tip;
	t_vec3	axis;
	double	k2;
	double	a;
	double	half_b;
	double	c;
	double	m;
	double	disc;
	double	r1;
	double	r2;
	double	root;
	double	t_min;
	double	t_max;
}	t_cone_data;

typedef struct s_mt_data
{
	t_vec3	edge1;
	t_vec3	edge2;
	t_vec3	h;
	t_vec3	s;
	t_vec3	q;
	double	a;
	double	f;
	double	u;
	double	v;
}	t_mt_data;

typedef struct s_hittable_list
{
	t_hittable	**elements;
	int			count;
}	t_hittable_list;

t_hittable	*build_world(t_object *objects);

int			hit_list(const void *obj, const t_ray *r,
				t_interval rayt, t_hit_record *rec);

t_hittable	*create_hittable_sphere(t_object *obj);
t_hittable	*create_hittable_plane(t_object *obj);
t_hittable	*create_hittable_cylinder(t_object *obj);
t_hittable	*create_hittable_triangle(t_object *obj);
t_hittable	*create_hittable_cone(t_object *obj);

#endif
