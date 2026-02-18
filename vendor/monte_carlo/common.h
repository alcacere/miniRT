/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 20:00:51 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 15:57:56 by dlesieur         ###   ########.fr       */
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
#include "quad.h"

/* Texture FIRST (defines t_texture before material.h needs it) */
#include "texture.h"

/* Then perlin-related items */
#include "perlin.h"
#include "noise_texture.h"

/* Materials after textures are fully defined */
#include "material.h"

/* color and camera after all dependencies */
#include "color.h"
#include "camera.h"

#endif