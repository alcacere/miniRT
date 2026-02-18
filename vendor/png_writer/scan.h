/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scan.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:57:40 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:57:40 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCAN_H
#define SCAN_H
#include <stddef.h>
#include "types.h"
#include "padding.h"
#include "bit.h"
#include "colors.h"

/* forward decl from filter.h */
static unsigned unfilter(unsigned char *out, const unsigned char *in, unsigned w, unsigned h, unsigned bpp);

/*out must be buffer big enough to contain full image, and in must contain the full decompressed data from
the IDAT chunks (with filter index bytes and possible padding bits)
return value is error*/
static unsigned postProcessScanlines(unsigned char *out, unsigned char *in,
									 unsigned w, unsigned h, const LodePNGInfo *info_png)
{
	/* Only non-interlaced supported here */
	if (info_png->interlace_method != 0)
		return 34; /* illegal/interlace not supported in this stub */

	/* in: each row starts with 1 filter byte */
	unsigned bpp = lodepng_get_bpp(&info_png->color);
	return unfilter(out, in, w, h, bpp);
}

static unsigned readChunk_PLTE(LodePNGColorMode *color, const unsigned char *data, size_t chunkLength)
{
	if (chunkLength % 3 != 0)
		return 38;
	color->palettesize = chunkLength / 3;
	color->palette = (unsigned char *)lodepng_malloc(4 * color->palettesize);
	if (!color->palette && color->palettesize)
		return 83;
	for (size_t i = 0; i < color->palettesize; ++i)
	{
		color->palette[4 * i + 0] = data[3 * i + 0];
		color->palette[4 * i + 1] = data[3 * i + 1];
		color->palette[4 * i + 2] = data[3 * i + 2];
		color->palette[4 * i + 3] = 255;
	}
	return 0;
}

static unsigned readChunk_tRNS(LodePNGColorMode *color, const unsigned char *data, size_t chunkLength)
{
	(void)color;
	(void)data;
	(void)chunkLength;
	return 0;
}

static inline size_t lodepng_get_raw_size_idat(unsigned w, unsigned h, const LodePNGColorMode *color)
{
	size_t linebytes = ((size_t)w * lodepng_get_bpp(color) + 7) / 8;
	return h * (linebytes + 1);
}

static void preProcessScanlines(unsigned char **out, size_t *outsize,
								const unsigned char *in, unsigned w, unsigned h,
								const LodePNGInfo *info, const LodePNGEncoderSettings *settings)
{
	(void)settings;
	*outsize = lodepng_get_raw_size(w, h, &info->color);
	*out = (unsigned char *)lodepng_malloc(*outsize);
	if (*out && *outsize)
		memcpy(*out, in, *outsize);
}

#endif