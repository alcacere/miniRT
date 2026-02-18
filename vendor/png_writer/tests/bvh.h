/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 16:38:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 16:55:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_H
#define BVH_H

#include "types.h"
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "interval.h"
#include "random.h"
#include <stdlib.h>
#include <string.h>

/* Forward declaration */
typedef struct s_bvh_node t_bvh_node;

/* BVH node structure: stores children as wrappers and bounding box */
typedef struct s_bvh_node
{
	t_hittable_wrapper left;
	t_hittable_wrapper right;
	t_aabb bbox;
} t_bvh_node;

/* Comparator function type for qsort */
typedef int (*t_comparator_fn)(const void *a, const void *b);

/* Generic box comparator: compares interval minimums along given axis */
static inline int bvh_box_compare(const void *a, const void *b, int axis_index)
{
	const t_hittable_wrapper *wa = (const t_hittable_wrapper *)a;
	const t_hittable_wrapper *wb = (const t_hittable_wrapper *)b;

	real_t a_min = aabb_axis_interval(&wa->bbox, axis_index)->min;
	real_t b_min = aabb_axis_interval(&wb->bbox, axis_index)->min;

	if (a_min < b_min)
		return -1;
	else if (a_min > b_min)
		return 1;
	return 0;
}

/* Comparator: compare x-axis interval minimums */
static inline int bvh_box_x_compare(const void *a, const void *b)
{
	return bvh_box_compare(a, b, 0);
}

/* Comparator: compare y-axis interval minimums */
static inline int bvh_box_y_compare(const void *a, const void *b)
{
	return bvh_box_compare(a, b, 1);
}

/* Comparator: compare z-axis interval minimums */
static inline int bvh_box_z_compare(const void *a, const void *b)
{
	return bvh_box_compare(a, b, 2);
}

/* Set current BVH node for callback dispatch */
static const t_bvh_node *g_current_bvh = NULL;
static inline void set_current_bvh(const void *obj)
{
	g_current_bvh = (const t_bvh_node *)obj;
}

/* BVH node hit function: early AABB rejection, then recurse to children */
static inline bool bvh_node_hit(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_bvh_node *node = g_current_bvh;
	if (!node)
		return false;

	/* Early rejection: ray doesn't hit bounding box */
	t_interval ray_t_copy = rayt;
	if (!aabb_hit(&node->bbox, r, &ray_t_copy))
		return false;

	/* Test left child */
	bool hit_left = false;
	t_hit_record temp_rec;
	if (node->left.hit_noobj && node->left.set_current)
	{
		node->left.set_current(node->left.object);
		hit_left = node->left.hit_noobj(r, rayt, &temp_rec);
		if (hit_left)
		{
			*rec = temp_rec;
			rayt.max = temp_rec.t;
		}
	}

	/* Test right child with updated interval */
	bool hit_right = false;
	if (node->right.hit_noobj && node->right.set_current)
	{
		node->right.set_current(node->right.object);
		hit_right = node->right.hit_noobj(r, rayt, &temp_rec);
		if (hit_right)
			*rec = temp_rec;
	}

	return hit_left || hit_right;
}

/* Recursive BVH construction from sorted object array */
static inline t_bvh_node *bvh_node_build(t_hittable_wrapper *objects, size_t start, size_t end)
{
	if (!objects || start >= end)
		return NULL;

	t_bvh_node *node = (t_bvh_node *)malloc(sizeof(t_bvh_node));
	if (!node)
		return NULL;

	size_t object_span = end - start;

	/* Build bounding box of the span of objects */
	t_aabb span_bbox = aabb_empty();
	for (size_t i = start; i < end; ++i)
		span_bbox = aabb_merge(&span_bbox, &objects[i].bbox);

	/* Choose axis with longest extent */
	int axis = aabb_longest_axis(&span_bbox);
	t_comparator_fn comparator;
	if (axis == 0)
		comparator = bvh_box_x_compare;
	else if (axis == 1)
		comparator = bvh_box_y_compare;
	else
		comparator = bvh_box_z_compare;

	/* Base case: single object (leaf node) */
	if (object_span == 1)
	{
		node->left = objects[start];
		node->right.object = NULL;
		node->right.owned = false;
		node->right.set_current = NULL;
		node->right.hit_noobj = NULL;
		node->bbox = objects[start].bbox;
		return node;
	}

	/* Base case: two objects */
	if (object_span == 2)
	{
		/* Sort to maintain consistency */
		if (comparator(&objects[start], &objects[start + 1]) > 0)
		{
			t_hittable_wrapper tmp = objects[start];
			objects[start] = objects[start + 1];
			objects[start + 1] = tmp;
		}
		node->left = objects[start];
		node->right = objects[start + 1];
		node->bbox = aabb_merge(&objects[start].bbox, &objects[start + 1].bbox);
		return node;
	}

	/* Recursive case: sort and split */
	qsort(&objects[start], object_span, sizeof(t_hittable_wrapper), comparator);
	size_t mid = start + object_span / 2;

	t_bvh_node *left_node = bvh_node_build(objects, start, mid);
	t_bvh_node *right_node = bvh_node_build(objects, mid, end);

	if (!left_node || !right_node)
	{
		free(node);
		if (left_node)
			free(left_node);
		if (right_node)
			free(right_node);
		return NULL;
	}

	/* Create wrappers for child nodes */
	node->left.object = (void *)left_node;
	node->left.owned = true;
	node->left.set_current = set_current_bvh;
	node->left.hit_noobj = bvh_node_hit;
	node->left.bbox = left_node->bbox;

	node->right.object = (void *)right_node;
	node->right.owned = true;
	node->right.set_current = set_current_bvh;
	node->right.hit_noobj = bvh_node_hit;
	node->right.bbox = right_node->bbox;

	/* Compute bounding box as merge of children */
	node->bbox = aabb_merge(&left_node->bbox, &right_node->bbox);

	return node;
}

/* Create BVH from hittable list */
static inline t_bvh_node *bvh_node_create(t_hittable_list *world)
{
	if (!world || world->count == 0)
		return NULL;

	/* Copy wrappers to avoid modifying original list */
	t_hittable_wrapper *objects = (t_hittable_wrapper *)malloc(world->count * sizeof(t_hittable_wrapper));
	if (!objects)
		return NULL;

	memcpy(objects, world->objects, world->count * sizeof(t_hittable_wrapper));

	/* Prevent BVH from owning primitives (world retains ownership) */
	for (size_t i = 0; i < world->count; ++i)
		objects[i].owned = false;

	/* Build BVH tree */
	t_bvh_node *root = bvh_node_build(objects, 0, world->count);

	free(objects);
	return root;
}

/* Recursively free BVH tree */
static inline void bvh_node_destroy(t_bvh_node *node)
{
	if (!node)
		return;

	/* Recursively free children */
	if (node->left.object && node->left.owned)
	{
		if (node->left.set_current == set_current_bvh)
			bvh_node_destroy((t_bvh_node *)node->left.object);
		else
			free(node->left.object);
	}

	if (node->right.object && node->right.owned)
	{
		if (node->right.set_current == set_current_bvh)
			bvh_node_destroy((t_bvh_node *)node->right.object);
		else
			free(node->right.object);
	}

	free(node);
}

#endif