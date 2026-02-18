/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uivector.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:44:32 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:44:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UIVECTOR_H
#define UIVECTOR_H
#include <stddef.h>
#include "types.h"
#include "utils.h"

#ifndef UIVECTOR_DEFINED
#define UIVECTOR_DEFINED
typedef struct s_uivector
{
	unsigned *data;
	size_t size;
	size_t allocsize;
} uivector;
#endif

static inline void uivector_init(uivector *v)
{
	v->data = NULL;
	v->size = 0;
	v->allocsize = 0;
}
static inline int uivector_resize(uivector *v, size_t s)
{
	if (s > v->allocsize)
	{
		size_t n = s ? s : 1;
		unsigned *p = (unsigned *)lodepng_realloc(v->data, n * sizeof(unsigned));
		if (!p)
			return 0;
		v->data = p;
		v->allocsize = n;
	}
	v->size = s;
	return 1;
}
static inline int uivector_resizev(uivector *v, size_t s, unsigned val)
{
	size_t i;
	if (!uivector_resize(v, s))
		return 0;
	for (i = 0; i < s; ++i)
		v->data[i] = val;
	return 1;
}
static inline int uivector_push_back(uivector *v, unsigned val)
{
	if (v->size + 1 > v->allocsize && !uivector_resize(v, v->size + 1))
		return 0;
	v->data[v->size++] = val;
	return 1;
}
static inline void uivector_cleanup(uivector *v)
{
	lodepng_free(v->data);
	v->data = NULL;
	v->size = v->allocsize = 0;
}
#endif