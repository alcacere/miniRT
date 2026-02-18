/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable_list.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 19:34:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 23:22:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <stdlib.h>
#include <string.h>
#include "interval.h"
#include "hittable.h"
#include "aabb.h"
#include "sphere.h"

/* Wrapper stores object pointer, ownership and two per-type callbacks */

typedef struct s_hittable_list
{
	t_hittable_wrapper *objects;
	size_t count;
	size_t capacity;
	t_aabb bbox;
} t_hittable_list;

/* Initialize an empty list */
static inline void hittable_list_init(t_hittable_list *list)
{
	list->objects = NULL;
	list->count = 0;
	list->capacity = 0;
	list->bbox = aabb_empty();
}

/* Free internal storage and reset (frees owned objects) */
static inline void hittable_list_clear(t_hittable_list *list)
{
	if (!list)
		return;
	for (size_t i = 0; i < list->count; ++i)
		if (list->objects[i].owned && list->objects[i].object)
			free(list->objects[i].object);
	free(list->objects);
	list->objects = NULL;
	list->count = 0;
	list->capacity = 0;
	list->bbox = aabb_empty();
}

/* Internal helper to grow/append a wrapper */
static inline bool hittable_list_add_wrapper(t_hittable_list *list, const t_hittable_wrapper *wrap)
{
	if (list->count + 1 > list->capacity)
	{
		size_t newcap = (list->capacity == 0) ? 4 : list->capacity * 2;
		t_hittable_wrapper *newarr = (t_hittable_wrapper *)realloc(list->objects, newcap * sizeof(t_hittable_wrapper));
		if (!newarr)
			return false;
		list->objects = newarr;
		list->capacity = newcap;
	}
	list->objects[list->count++] = *wrap;

	/* Merge the new object's bounding box into the list's bounding box */
	list->bbox = aabb_merge(&list->bbox, &wrap->bbox);

	return true;
}

/* Convenience: add a sphere owned by the list (copy) */
static inline bool hittable_list_add_sphere(t_hittable_list *list, const t_sphere *s)
{
	t_sphere *copy = (t_sphere *)malloc(sizeof(t_sphere));
	if (!copy)
		return false;
	*copy = *s;
	t_hittable_wrapper wrap = {
		.object = copy,
		.owned = true,
		.set_current = set_current_sphere,
		.hit_noobj = sphere_hit_noobj,
		.bbox = s->bbox};
	return hittable_list_add_wrapper(list, &wrap);
}

/* Append a pre-built wrapper (non-owned object). */
static inline bool hittable_list_add_nonowned(t_hittable_list *list, void *obj, t_set_current_fn set_current, t_hit_noobj_fn hit_noobj, const t_aabb *bbox)
{
	t_hittable_wrapper wrap = {
		.object = obj,
		.owned = false,
		.set_current = set_current,
		.hit_noobj = hit_noobj,
		.bbox = *bbox};
	return hittable_list_add_wrapper(list, &wrap);
}

/* Get the bounding box of the entire list */
static inline t_aabb hittable_list_bounding_box(const t_hittable_list *list)
{
	if (!list)
		return aabb_empty();
	return list->bbox;
}

/* Iterate wrappers: bind object, call hit_noobj, track closest hit */
static inline bool hittable_list_hit(const t_hittable_list *list, const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	bool hit_anything = false;
	real_t closest_so_far = (real_t)rayt.max;
	t_hit_record temp_rec;

	for (size_t i = 0; i < list->count; ++i)
	{
		const t_hittable_wrapper *w = &list->objects[i];
		if (!w->set_current || !w->hit_noobj)
			continue;
		w->set_current(w->object);
		/* pass a t_interval [rayt.min, closest_so_far] to the per-object callback */
		if (w->hit_noobj(r, interval(rayt.min, (real_t)closest_so_far), &temp_rec))
		{
			hit_anything = true;
			closest_so_far = (real_t)temp_rec.t;
			if (rec)
				*rec = temp_rec;
		}
	}
	return hit_anything;
}

static __thread const t_hittable_list *g_current_list = NULL;
static inline void set_current_hlist(const void *obj) { g_current_list = (const t_hittable_list *)obj; }
static inline bool hittable_list_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_list)
		return false;
	return hittable_list_hit(g_current_list, r, rayt, rec);
}

static inline t_hittable_wrapper hittable_list_wrapper(const t_hittable_list *list)
{
	t_hittable_wrapper w = {
		.object = (void *)list,
		.owned = false,
		.set_current = set_current_hlist,
		.hit_noobj = hittable_list_hit_noobj,
		.bbox = list ? list->bbox : aabb_empty()};
	return w;
}

#endif