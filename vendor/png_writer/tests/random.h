/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 00:59:57 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 16:41:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <limits.h>
#include "settings.h"
#include "types.h"

/* return a random real in [0,1] */
static inline real_t random_real(void)
{
	/* rand() / RAND_MAX returns value in [0,1] */
	return ((real_t)rand() / (real_t)RAND_MAX);
}

/* return a random real in [min,max) */
static inline real_t random_real_interval(real_t min, real_t max)
{
	return (min + (max - min) * random_real());
}

/* Alias for random_real() */
static inline real_t random_double(void)
{
	return random_real();
}

/* return a random integer in [min,max] (inclusive) */
static inline int random_int(int min, int max)
{
	return (int)random_real_interval((real_t)min, (real_t)(max + 1));
}

#endif