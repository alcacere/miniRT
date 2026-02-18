/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:47:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/05 00:11:25 by dlesieur         ###   ########.fr       */
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

/* Forward declare t_vec3 for aliases below */
typedef struct s_vec3 t_vec3;

/* Simple aliases - these are just semantic distinctions, same underlying type */
typedef bool t_hittable;
typedef t_vec3 t_color;
typedef t_vec3 t_point3;

#endif /* RT_TESTS_TYPES_H */