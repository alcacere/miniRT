/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:50:30 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:50:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STR_H
#define STR_H
#include <stdlib.h>
#include <string.h>

static inline void string_cleanup(char **out)
{
	if (out && *out)
	{
		free(*out);
		*out = NULL;
	}
}

static inline char *alloc_string(const char *in)
{
	if (!in)
		return NULL;
	size_t len = strlen(in) + 1;
	char *s = (char *)malloc(len);
	if (!s)
		return NULL;
	memcpy(s, in, len);
	return s;
}
#endif