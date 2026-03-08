/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:13:02 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 16:13:04 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
# define HITTABLE_H

# include "minirt.h"

typedef struct s_hit_record	t_hit_record;
typedef struct s_material	t_material;
typedef struct s_hittable	t_hittable;
typedef int					(*t_hit_fn)(const void *obj, const t_ray *r,
										t_interval rayt, t_hit_record *rec);

struct s_hit_record
{
	t_vec3		p;
	t_vec3		normal;
	double		t;
	int			front_face;
	double		u;
	double		v;
	t_material	*mat;
};

struct s_hittable
{
	void		*object;
	t_hit_fn	hit;
	t_aabb		bbox;
};

#endif
