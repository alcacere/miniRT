/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   png_codec.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 17:30:48 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 17:30:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PNG_CODEC_H
#define PNG_CODEC_H

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285
#define NUM_DEFLATE_CODE_SYMBOLS 288
#define NUM_DISTANCE_SYMBOLS 32
#define NUM_CODE_LENGTH_CODES 19
#define LODEPNG_VERSION_STRING "20190210"
#define MAX_SUPPORTED_DEFLATE_LENGTH 258
#define HASH_NUM_VALUES 65536
#define HASH_BIT_MASK 65535
#define DEFAULT_WINDOWS 2048

typedef struct s_ctx
{
	unsigned char **out;
	size_t *outsize;
	const unsigned char *image;
	unsigned w;
	unsigned h;
	LodePNGColorType color_type;
	unsigned bit_depth;
} t_ctx;

#endif
