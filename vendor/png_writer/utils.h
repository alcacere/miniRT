/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:15:30 by marvin            #+#    #+#             */
/*   Updated: 2025/12/30 00:15:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>

/* Memory management */
static inline void *lodepng_malloc(size_t size)
{
	return malloc(size);
}

static inline void lodepng_free(void *ptr)
{
	free(ptr);
}

static inline void *lodepng_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

/* Utility macros */
#define LODEPNG_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LODEPNG_MIN(a, b) ((a) < (b) ? (a) : (b))

#define CERROR_TRY_RETURN(error_code) \
	do                                \
	{                                 \
		unsigned error = error_code;  \
		if (error)                    \
			return error;             \
	} while (0)

#define CERROR_RETURN(variable, error_code) \
	do                                      \
	{                                       \
		variable = error_code;              \
		return;                             \
	} while (0)

#define CERROR_RETURN_ERROR(variable, error_code) \
	do                                            \
	{                                             \
		variable = error_code;                    \
		return variable;                          \
	} while (0)

#define CERROR_BREAK(variable, error_code) \
	do                                     \
	{                                      \
		variable = error_code;             \
		break;                             \
	} while (0)

#define ERROR_BREAK(error_code) \
	do                          \
	{                           \
		error = error_code;     \
		break;                  \
	} while (0)

#define READBIT(bitpointer, bitstream) \
	(((bitstream)[((bitpointer) >> 3)] >> (((bitpointer) & 0x7))) & 1)

#endif