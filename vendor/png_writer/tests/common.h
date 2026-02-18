/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 20:00:51 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 14:54:31 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_H
#define COMMON_H

#include "settings.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include "random.h"
#include "sphere.h"

/* Include interval early so objects/hittable_list can use t_interval */
#include "interval.h"

/* low-level math types and helpers */
#include "vector.h"
#include "point.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"

/* color depends on hittable_list; include before camera to avoid forward decl mismatch */
#include "color.h"

/* finally include camera which will use write_color / ray_color_world defined above */
#include "camera.h"

#endif