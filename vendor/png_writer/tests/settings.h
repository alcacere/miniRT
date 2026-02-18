/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:45:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 14:54:31 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * Variables
 */
#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#undef FLOAT_TYPE

/* Constant */
#define X 0
#define Y 1
#define Z 2
#define TOT_COORD3 3
#define W 3
#define TOT_COORD4 4

/* mathematical constants */
#ifndef PI
#define PI 3.1415926535897932385
#endif

/* Ensure math definitions are available before testing/defining INFINITY */
#include <math.h>

/* Define INFINITY only if not already defined by system headers */
#ifndef INFINITY
#if defined(HUGE_VAL)
#define INFINITY HUGE_VAL
#else
#define INFINITY (1.0 / 0.0)
#endif
#endif

/* Utility function: convert degrees to radians (requires PI and math.h above) */
#include "types.h"
static inline real_t degrees_to_radians(real_t degrees) { return (degrees * PI / 180.0); }

#endif /* SETTINGS_H */