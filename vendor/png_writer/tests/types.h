/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:47:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 19:33:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_TESTS_TYPES_H
#define RT_TESTS_TYPES_H

#include <stdbool.h>

/* Define real_t before any header that needs it */
#ifndef FLOAT_TYPE
typedef double real_t;
#else
typedef float real_t;
#endif

/* Now include the vector type which depends on real_t */
#include "vector.h"

/* Simple aliases */
typedef bool t_hittable;
typedef t_vec3 t_color;
typedef t_vec3 t_point3;

#endif /* RT_TESTS_TYPES_H */