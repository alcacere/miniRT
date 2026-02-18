/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ucvector.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:14:47 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:14:47 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UCVECTOR_H
#define UCVECTOR_H
#include <stddef.h>
#include "types.h"
#include "utils.h"

#ifndef UCVECTOR_DEFINED
#define UCVECTOR_DEFINED
typedef struct s_ucvector
{
	unsigned char *data;
	size_t size;
	size_t allocsize;
} ucvector;
#endif

static inline void ucvector_init(ucvector *v)
{
	v->data = NULL;
	v->size = 0;
	v->allocsize = 0;
}
static inline void ucvector_init_buffer(ucvector *v, unsigned char *d, size_t s)
{
	v->data = d;
	v->size = s;
	v->allocsize = s;
}
static inline int ucvector_resize(ucvector *v, size_t s)
{
	if (s > v->allocsize)
	{
		size_t n = s ? s : 1;
		unsigned char *p = (unsigned char *)lodepng_realloc(v->data, n);
		if (!p)
			return 0;
		v->data = p;
		v->allocsize = n;
	}
	v->size = s;
	return 1;
}
static inline int ucvector_reserve(ucvector *v, size_t s) { return s <= v->allocsize || ucvector_resize(v, s); }
static inline int ucvector_push_back(ucvector *v, unsigned char c)
{
	if (v->size + 1 > v->allocsize && !ucvector_resize(v, v->size + 1))
		return 0;
	v->data[v->size++] = c;
	return 1;
}
static inline void ucvector_cleanup(ucvector *v)
{
	lodepng_free(v->data);
	v->data = NULL;
	v->size = v->allocsize = 0;
}
#endif
