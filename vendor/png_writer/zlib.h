/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zlib.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:25:04 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:25:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ZLIB_H
#define ZLIB_H

#include "inflate.h"
#include "deflate.h"

static unsigned zlib_decompress(unsigned char **out, size_t *outsize,
								const unsigned char *in, size_t insize,
								const LodePNGDecompressSettings *settings)
{
	unsigned error = 0;
	unsigned CMF, FLGS;
	if (insize < 2)
		return 53;
	CMF = in[0];
	FLGS = in[1];
	/* CM (low nibble) must be 8 (deflate) */
	if ((CMF & 0x0F) != 8)
		return 24;
	/* CINFO (window size), max 7 (32K) */
	if ((CMF >> 4) > 7)
		return 25;
	/* FCHECK must make (CMF<<8 | FLGS) % 31 == 0 */
	if (((CMF << 8) + FLGS) % 31 != 0)
		return 24;
	/* FDICT bit not allowed for PNG */
	if (FLGS & 32)
		return 26;

	error = inflate(out, outsize, in + 2, insize - 2, settings);
	return error;
}

static unsigned zlib_compress(unsigned char **out, size_t *outsize,
							  const unsigned char *in, size_t insize,
							  const LodePNGCompressSettings *settings)
{
	unsigned error;
	unsigned char *deflatedata = 0;
	size_t deflatesize = 0;
	unsigned char header[2];
	unsigned adler = adler32(in, (unsigned)insize);

	/* derive CINFO from windowsize (rounded up to next power-of-two, capped at 32K) */
	unsigned ws = settings ? settings->windowsize : 32768;
	if (ws < 256)
		ws = 256;
	if (ws > 32768)
		ws = 32768;
	unsigned cinfo = 0;
	while (cinfo < 7 && (1u << (cinfo + 8)) < ws)
		++cinfo;

	header[0] = (unsigned char)((cinfo << 4) | 8); /* CM=8 */
	{
		unsigned flevel = 2;		  /* default compression */
		unsigned flg = (flevel << 6); /* FDICT=0 */
		unsigned fcheck = (31 - (((header[0] << 8) | flg) % 31)) % 31;
		header[1] = (unsigned char)(flg | fcheck);
	}

	error = deflate(&deflatedata, &deflatesize, in, insize, settings);

	if (!error)
	{
		ucvector v;
		ucvector_init(&v);

		ucvector_push_back(&v, header[0]);
		ucvector_push_back(&v, header[1]);

		for (size_t i = 0; i < deflatesize; ++i)
			ucvector_push_back(&v, deflatedata[i]);

		ucvector_push_back(&v, (unsigned char)((adler >> 24) & 0xff));
		ucvector_push_back(&v, (unsigned char)((adler >> 16) & 0xff));
		ucvector_push_back(&v, (unsigned char)((adler >> 8) & 0xff));
		ucvector_push_back(&v, (unsigned char)(adler & 0xff));

		*out = v.data;
		*outsize = v.size;
	}

	lodepng_free(deflatedata);
	return error;
}

#endif