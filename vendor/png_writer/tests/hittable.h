/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 19:37:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 17:09:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
#define HITTABLE_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "aabb.h"
#include <stdbool.h>

/* Forward declaration of material to avoid circular dependency */
typedef struct s_material t_material;

/* Hit record: store intersection point, normal, material and t. */
typedef struct s_hit_record
{
	t_vec3 p;
	t_vec3 normal;
	real_t t;
	bool front_face;
	t_vec3 albedo;	 /* per-hit surface color */
	// in the future we need to compute (u,v) texture coordinates for a given point on each type of hittable
	real_t		u;
	real_t		v;
	t_material *mat; /* pointer to material that determines scattering behavior */
} t_hit_record;

/* set_face_normal: outward_normal is assumed unit length. */
static inline void set_face_normal(t_hit_record *hit, const t_ray *r, const t_vec3 *outward_normal)
{
	bool front = (dot(&r->dir, outward_normal) < 0.0);
	hit->front_face = front;
	if (front)
		hit->normal = *outward_normal;
	else
		hit->normal = vec3_neg(outward_normal);
}

#endif