/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bpm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:05:28 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:05:28 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BPM_H
#define BPM_H
#include <stddef.h>
#include "utils.h"

typedef struct s_bpm_node
{
	int weight;
	unsigned index;
	struct s_bpm_node *tail;
	int in_use;
} BPMNode;

typedef struct s_bpm_lists
{
	unsigned listsize;
	unsigned memsize;
	unsigned numfree;
	unsigned nextfree;
	BPMNode *memory;
	BPMNode **freelist;
	BPMNode **chains0;
	BPMNode **chains1;
} BPMLists;

static inline BPMNode *bpmnode_create(BPMLists *lists, int weight, unsigned index, BPMNode *tail)
{
	unsigned i;
	BPMNode *result;

	if (lists->nextfree >= lists->numfree)
	{

		for (i = 0; i != lists->memsize; ++i)
			lists->memory[i].in_use = 0;
		for (i = 0; i != lists->listsize; ++i)
		{
			BPMNode *node;
			for (node = lists->chains0[i]; node != 0; node = node->tail)
				node->in_use = 1;
			for (node = lists->chains1[i]; node != 0; node = node->tail)
				node->in_use = 1;
		}

		lists->numfree = 0;
		for (i = 0; i != lists->memsize; ++i)
		{
			if (!lists->memory[i].in_use)
				lists->freelist[lists->numfree++] = &lists->memory[i];
		}
		lists->nextfree = 0;
	}

	result = lists->freelist[lists->nextfree++];
	result->weight = weight;
	result->index = index;
	result->tail = tail;
	return result;
}

static inline void bpmnode_sort(BPMNode *leaves, size_t num)
{
	BPMNode *mem = (BPMNode *)lodepng_malloc(sizeof(*leaves) * num);
	size_t width, counter = 0;
	for (width = 1; width < num; width *= 2)
	{
		BPMNode *a = (counter & 1) ? mem : leaves;
		BPMNode *b = (counter & 1) ? leaves : mem;
		size_t p;
		for (p = 0; p < num; p += 2 * width)
		{
			size_t q = (p + width > num) ? num : (p + width);
			size_t r = (p + 2 * width > num) ? num : (p + 2 * width);
			size_t i = p, j = q, k;
			for (k = p; k < r; k++)
			{
				if (i < q && (j >= r || a[i].weight <= a[j].weight))
					b[k] = a[i++];
				else
					b[k] = a[j++];
			}
		}
		counter++;
	}
	if (counter & 1)
		memcpy(leaves, mem, sizeof(*leaves) * num);
	lodepng_free(mem);
}

static inline void boundaryPM(BPMLists *lists, BPMNode *leaves, size_t numpresent, int c, int num)
{
	unsigned lastindex = lists->chains1[c]->index;

	if (c == 0)
	{
		if (lastindex >= numpresent)
			return;
		lists->chains0[c] = lists->chains1[c];
		lists->chains1[c] = bpmnode_create(lists, leaves[lastindex].weight, lastindex + 1, 0);
	}
	else
	{

		int sum = lists->chains0[c - 1]->weight + lists->chains1[c - 1]->weight;
		lists->chains0[c] = lists->chains1[c];
		if (lastindex < numpresent && sum > leaves[lastindex].weight)
		{
			lists->chains1[c] = bpmnode_create(lists, leaves[lastindex].weight, lastindex + 1, lists->chains1[c]->tail);
			return;
		}
		lists->chains1[c] = bpmnode_create(lists, sum, lastindex, lists->chains1[c - 1]);
		/*in the end we are only interested in the chain of the last list, so no
		need to recurse if we're at the last one (this gives measurable speedup)*/
		if (num + 1 < (int)(2 * numpresent - 2))
		{
			boundaryPM(lists, leaves, numpresent, c - 1, num);
			boundaryPM(lists, leaves, numpresent, c - 1, num);
		}
	}
}

#endif