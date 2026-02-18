/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inspect.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:52:04 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:52:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "all.h"

unsigned lodepng_inspect(unsigned *w, unsigned *h, LodePNGState *state,
						 const unsigned char *in, size_t insize)
{
	unsigned width, height;
	LodePNGInfo *info = &state->info_png;
	if (insize == 0 || in == 0)
	{
		CERROR_RETURN_ERROR(state->error, 48);
	}
	if (insize < 33)
	{
		CERROR_RETURN_ERROR(state->error, 27);
	}

	lodepng_info_cleanup(info);
	lodepng_info_init(info);

	if (in[0] != 137 || in[1] != 80 || in[2] != 78 || in[3] != 71 || in[4] != 13 || in[5] != 10 || in[6] != 26 || in[7] != 10)
	{
		CERROR_RETURN_ERROR(state->error, 28);
	}
	if (lodepng_chunk_length(in + 8) != 13)
	{
		CERROR_RETURN_ERROR(state->error, 94);
	}
	if (!lodepng_chunk_type_equals(in + 8, "IHDR"))
	{
		CERROR_RETURN_ERROR(state->error, 29);
	}

	width = lodepng_read32bitInt(&in[16]);
	height = lodepng_read32bitInt(&in[20]);
	info->color.bitdepth = in[24];
	info->color.colortype = (LodePNGColorType)in[25];
	info->compression_method = in[26];
	info->filter_method = in[27];
	info->interlace_method = in[28];

	if (width == 0 || height == 0)
	{
		CERROR_RETURN_ERROR(state->error, 93);
	}

	if (w)
		*w = width;
	if (h)
		*h = height;

	if (!state->decoder.ignore_crc)
	{
		unsigned CRC = lodepng_read32bitInt(&in[29]);
		unsigned checksum = lodepng_crc32(&in[12], 17);
		if (CRC != checksum)
		{
			CERROR_RETURN_ERROR(state->error, 57);
		}
	}

	if (info->compression_method != 0)
		CERROR_RETURN_ERROR(state->error, 32);

	if (info->filter_method != 0)
		CERROR_RETURN_ERROR(state->error, 33);

	if (info->interlace_method > 1)
		CERROR_RETURN_ERROR(state->error, 34);

	state->error = checkColorValidity(info->color.colortype, info->color.bitdepth);
	return state->error;
}
