/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   adam7.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:50:26 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:50:26 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ADAM7_H
#define ADAM7_H

static const unsigned ADAM7_IX[7] = {0, 4, 0, 2, 0, 1, 0};
static const unsigned ADAM7_IY[7] = {0, 0, 4, 0, 2, 0, 1};
static const unsigned ADAM7_DX[7] = {8, 8, 4, 4, 2, 2, 1};
static const unsigned ADAM7_DY[7] = {8, 8, 8, 4, 4, 2, 2};

/*
Outputs various dimensions and positions in the image related to the Adam7 reduced images.
passw: output containing the width of the 7 passes
passh: output containing the height of the 7 passes
filter_passstart: output containing the index of the start and end of each
 reduced image with filter bytes
padded_passstart output containing the index of the start and end of each
 reduced image when without filter bytes but with padded scanlines
passstart: output containing the index of the start and end of each reduced
 image without padding between scanlines, but still padding between the images
w, h: width and height of non-interlaced image
bpp: bits per pixel
"padded" is only relevant if bpp is less than 8 and a scanline or image does not
 end at a full byte
*/
static void Adam7_getpassvalues(unsigned passw[7], unsigned passh[7], size_t filter_passstart[8],
								size_t padded_passstart[8], size_t passstart[8], unsigned w, unsigned h, unsigned bpp)
{

	unsigned i;

	for (i = 0; i != 7; ++i)
	{
		passw[i] = (w + ADAM7_DX[i] - ADAM7_IX[i] - 1) / ADAM7_DX[i];
		passh[i] = (h + ADAM7_DY[i] - ADAM7_IY[i] - 1) / ADAM7_DY[i];
		if (passw[i] == 0)
			passh[i] = 0;
		if (passh[i] == 0)
			passw[i] = 0;
	}

	filter_passstart[0] = padded_passstart[0] = passstart[0] = 0;
	for (i = 0; i != 7; ++i)
	{

		filter_passstart[i + 1] = filter_passstart[i] + ((passw[i] && passh[i]) ? passh[i] * (1 + (passw[i] * bpp + 7) / 8) : 0);

		padded_passstart[i + 1] = padded_passstart[i] + passh[i] * ((passw[i] * bpp + 7) / 8);

		passstart[i + 1] = passstart[i] + (passh[i] * passw[i] * bpp + 7) / 8;
	}
}

/*
in: Adam7 interlaced image, with no padding bits between scanlines, but between
 reduced images so that each reduced image starts at a byte.
out: the same pixels, but re-ordered so that they're now a non-interlaced image with size w*h
bpp: bits per pixel
out has the following size in bits: w * h * bpp.
in is possibly bigger due to padding bits between reduced images.
out must be big enough AND must be 0 everywhere if bpp < 8 in the current implementation
(because that's likely a little bit faster)
NOTE: comments about padding bits are only relevant if bpp < 8
*/
static void Adam7_deinterlace(unsigned char *out, const unsigned char *in, unsigned w, unsigned h, unsigned bpp)
{
	unsigned passw[7], passh[7];
	size_t filter_passstart[8], padded_passstart[8], passstart[8];
	unsigned i;

	Adam7_getpassvalues(passw, passh, filter_passstart, padded_passstart, passstart, w, h, bpp);

	if (bpp >= 8)
	{
		for (i = 0; i != 7; ++i)
		{
			unsigned x, y, b;
			size_t bytewidth = bpp / 8;
			for (y = 0; y < passh[i]; ++y)
				for (x = 0; x < passw[i]; ++x)
				{
					size_t pixelinstart = passstart[i] + (y * passw[i] + x) * bytewidth;
					size_t pixeloutstart = ((ADAM7_IY[i] + y * ADAM7_DY[i]) * w + ADAM7_IX[i] + x * ADAM7_DX[i]) * bytewidth;
					for (b = 0; b < bytewidth; ++b)
					{
						out[pixeloutstart + b] = in[pixelinstart + b];
					}
				}
		}
	}
	else
	{
		for (i = 0; i != 7; ++i)
		{
			unsigned x, y, b;
			unsigned ilinebits = bpp * passw[i];
			unsigned olinebits = bpp * w;
			size_t obp, ibp;
			for (y = 0; y < passh[i]; ++y)
				for (x = 0; x < passw[i]; ++x)
				{
					ibp = (8 * passstart[i]) + (y * ilinebits + x * bpp);
					obp = (ADAM7_IY[i] + y * ADAM7_DY[i]) * olinebits + (ADAM7_IX[i] + x * ADAM7_DX[i]) * bpp;
					for (b = 0; b < bpp; ++b)
					{
						unsigned char bit = readBitFromReversedStream(&ibp, in);

						setBitOfReversedStream0(&obp, out, bit);
					}
				}
		}
	}
}

/*
in: non-interlaced image with size w*h
out: the same pixels, but re-ordered according to PNG's Adam7 interlacing, with
 no padding bits between scanlines, but between reduced images so that each
 reduced image starts at a byte.
bpp: bits per pixel
there are no padding bits, not between scanlines, not between reduced images
in has the following size in bits: w * h * bpp.
out is possibly bigger due to padding bits between reduced images
NOTE: comments about padding bits are only relevant if bpp < 8
*/
static void Adam7_interlace(unsigned char *out, const unsigned char *in, unsigned w, unsigned h, unsigned bpp)
{
	unsigned passw[7], passh[7];
	size_t filter_passstart[8], padded_passstart[8], passstart[8];
	unsigned i;

	Adam7_getpassvalues(passw, passh, filter_passstart, padded_passstart, passstart, w, h, bpp);

	if (bpp >= 8)
	{
		for (i = 0; i != 7; ++i)
		{
			unsigned x, y, b;
			size_t bytewidth = bpp / 8;
			for (y = 0; y < passh[i]; ++y)
				for (x = 0; x < passw[i]; ++x)
				{
					size_t pixelinstart = ((ADAM7_IY[i] + y * ADAM7_DY[i]) * w + ADAM7_IX[i] + x * ADAM7_DX[i]) * bytewidth;
					size_t pixeloutstart = passstart[i] + (y * passw[i] + x) * bytewidth;
					for (b = 0; b < bytewidth; ++b)
					{
						out[pixeloutstart + b] = in[pixelinstart + b];
					}
				}
		}
	}
	else
	{
		for (i = 0; i != 7; ++i)
		{
			unsigned x, y, b;
			unsigned ilinebits = bpp * passw[i];
			unsigned olinebits = bpp * w;
			size_t obp, ibp;
			for (y = 0; y < passh[i]; ++y)
				for (x = 0; x < passw[i]; ++x)
				{
					ibp = (ADAM7_IY[i] + y * ADAM7_DY[i]) * olinebits + (ADAM7_IX[i] + x * ADAM7_DX[i]) * bpp;
					obp = (8 * passstart[i]) + (y * ilinebits + x * bpp);
					for (b = 0; b < bpp; ++b)
					{
						unsigned char bit = readBitFromReversedStream(&ibp, in);
						setBitOfReversedStream(&obp, out, bit);
					}
				}
		}
	}
}

#endif