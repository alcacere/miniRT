/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rgba.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:43:00 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:43:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "colors.h"
#include "bit.h"

static unsigned rgba8ToPixel(unsigned char *out, size_t i,
							 const LodePNGColorMode *mode, ColorTree *tree,
							 unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (mode->colortype == LCT_GREY)
	{
		unsigned char gray = r;
		if (mode->bitdepth == 8)
			out[i] = gray;
		else if (mode->bitdepth == 16)
			out[i * 2 + 0] = out[i * 2 + 1] = gray;
		else
		{

			gray = (gray >> (8 - mode->bitdepth)) & ((1 << mode->bitdepth) - 1);
			addColorBits(out, i, mode->bitdepth, gray);
		}
	}
	else if (mode->colortype == LCT_RGB)
	{
		if (mode->bitdepth == 8)
		{
			out[i * 3 + 0] = r;
			out[i * 3 + 1] = g;
			out[i * 3 + 2] = b;
		}
		else
		{
			out[i * 6 + 0] = out[i * 6 + 1] = r;
			out[i * 6 + 2] = out[i * 6 + 3] = g;
			out[i * 6 + 4] = out[i * 6 + 5] = b;
		}
	}
	else if (mode->colortype == LCT_PALETTE)
	{
		int index = color_tree_get(tree, r, g, b, a);
		if (index < 0)
			return 82;
		if (mode->bitdepth == 8)
			out[i] = index;
		else
			addColorBits(out, i, mode->bitdepth, (unsigned)index);
	}
	else if (mode->colortype == LCT_GREY_ALPHA)
	{
		unsigned char gray = r;
		if (mode->bitdepth == 8)
		{
			out[i * 2 + 0] = gray;
			out[i * 2 + 1] = a;
		}
		else if (mode->bitdepth == 16)
		{
			out[i * 4 + 0] = out[i * 4 + 1] = gray;
			out[i * 4 + 2] = out[i * 4 + 3] = a;
		}
	}
	else if (mode->colortype == LCT_RGBA)
	{
		if (mode->bitdepth == 8)
		{
			out[i * 4 + 0] = r;
			out[i * 4 + 1] = g;
			out[i * 4 + 2] = b;
			out[i * 4 + 3] = a;
		}
		else
		{
			out[i * 8 + 0] = out[i * 8 + 1] = r;
			out[i * 8 + 2] = out[i * 8 + 3] = g;
			out[i * 8 + 4] = out[i * 8 + 5] = b;
			out[i * 8 + 6] = out[i * 8 + 7] = a;
		}
	}

	return 0;
}

static void rgba16ToPixel(unsigned char *out, size_t i,
						  const LodePNGColorMode *mode,
						  unsigned short r, unsigned short g, unsigned short b, unsigned short a)
{
	if (mode->colortype == LCT_GREY)
	{
		unsigned short gray = r;
		out[i * 2 + 0] = (gray >> 8) & 255;
		out[i * 2 + 1] = gray & 255;
	}
	else if (mode->colortype == LCT_RGB)
	{
		out[i * 6 + 0] = (r >> 8) & 255;
		out[i * 6 + 1] = r & 255;
		out[i * 6 + 2] = (g >> 8) & 255;
		out[i * 6 + 3] = g & 255;
		out[i * 6 + 4] = (b >> 8) & 255;
		out[i * 6 + 5] = b & 255;
	}
	else if (mode->colortype == LCT_GREY_ALPHA)
	{
		unsigned short gray = r;
		out[i * 4 + 0] = (gray >> 8) & 255;
		out[i * 4 + 1] = gray & 255;
		out[i * 4 + 2] = (a >> 8) & 255;
		out[i * 4 + 3] = a & 255;
	}
	else if (mode->colortype == LCT_RGBA)
	{
		out[i * 8 + 0] = (r >> 8) & 255;
		out[i * 8 + 1] = r & 255;
		out[i * 8 + 2] = (g >> 8) & 255;
		out[i * 8 + 3] = g & 255;
		out[i * 8 + 4] = (b >> 8) & 255;
		out[i * 8 + 5] = b & 255;
		out[i * 8 + 6] = (a >> 8) & 255;
		out[i * 8 + 7] = a & 255;
	}
}

void getPixelColorRGBA16(unsigned short *r, unsigned short *g, unsigned short *b, unsigned short *a,
						 const unsigned char *in, size_t i, const LodePNGColorMode *mode)
{
	if (mode->colortype == LCT_GREY)
	{
		*r = *g = *b = 256u * in[i * 2 + 0] + in[i * 2 + 1];
		*a = (mode->key_defined && *r == mode->key_r) ? 0 : 65535;
	}
	else if (mode->colortype == LCT_RGB)
	{
		*r = 256u * in[i * 6 + 0] + in[i * 6 + 1];
		*g = 256u * in[i * 6 + 2] + in[i * 6 + 3];
		*b = 256u * in[i * 6 + 4] + in[i * 6 + 5];
		*a = (mode->key_defined && *r == mode->key_r && *g == mode->key_g && *b == mode->key_b) ? 0 : 65535;
	}
	else if (mode->colortype == LCT_GREY_ALPHA)
	{
		*r = *g = *b = 256u * in[i * 4 + 0] + in[i * 4 + 1];
		*a = 256u * in[i * 4 + 2] + in[i * 4 + 3];
	}
	else
	{
		*r = 256u * in[i * 8 + 0] + in[i * 8 + 1];
		*g = 256u * in[i * 8 + 2] + in[i * 8 + 3];
		*b = 256u * in[i * 8 + 4] + in[i * 8 + 5];
		*a = 256u * in[i * 8 + 6] + in[i * 8 + 7];
	}
}

void getPixelColorRGBA8(unsigned char *r, unsigned char *g,
						unsigned char *b, unsigned char *a,
						const unsigned char *in, size_t i,
						const LodePNGColorMode *mode)
{
	if (mode->colortype == LCT_GREY)
	{
		if (mode->bitdepth == 8)
		{
			*r = *g = *b = in[i];
			if (mode->key_defined && *r == mode->key_r)
				*a = 0;
			else
				*a = 255;
		}
		else if (mode->bitdepth == 16)
		{
			*r = *g = *b = in[i * 2 + 0];
			if (mode->key_defined && 256U * in[i * 2 + 0] + in[i * 2 + 1] == mode->key_r)
				*a = 0;
			else
				*a = 255;
		}
		else
		{
			unsigned highest = ((1U << mode->bitdepth) - 1U);
			size_t j = i * mode->bitdepth;
			unsigned value = readBitsFromReversedStream(&j, in, mode->bitdepth);
			*r = *g = *b = (value * 255) / highest;
			if (mode->key_defined && value == mode->key_r)
				*a = 0;
			else
				*a = 255;
		}
	}
	else if (mode->colortype == LCT_RGB)
	{
		if (mode->bitdepth == 8)
		{
			*r = in[i * 3 + 0];
			*g = in[i * 3 + 1];
			*b = in[i * 3 + 2];
			if (mode->key_defined && *r == mode->key_r && *g == mode->key_g && *b == mode->key_b)
				*a = 0;
			else
				*a = 255;
		}
		else
		{
			*r = in[i * 6 + 0];
			*g = in[i * 6 + 2];
			*b = in[i * 6 + 4];
			if (mode->key_defined && 256U * in[i * 6 + 0] + in[i * 6 + 1] == mode->key_r && 256U * in[i * 6 + 2] + in[i * 6 + 3] == mode->key_g && 256U * in[i * 6 + 4] + in[i * 6 + 5] == mode->key_b)
				*a = 0;
			else
				*a = 255;
		}
	}
	else if (mode->colortype == LCT_PALETTE)
	{
		unsigned index;
		if (mode->bitdepth == 8)
			index = in[i];
		else
		{
			size_t j = i * mode->bitdepth;
			index = readBitsFromReversedStream(&j, in, mode->bitdepth);
		}

		if (index >= mode->palettesize)
		{
			/*This is an error according to the PNG spec, but common PNG decoders make it black instead.
			Done here too, slightly faster due to no error handling needed.*/
			*r = *g = *b = 0;
			*a = 255;
		}
		else
		{
			*r = mode->palette[index * 4 + 0];
			*g = mode->palette[index * 4 + 1];
			*b = mode->palette[index * 4 + 2];
			*a = mode->palette[index * 4 + 3];
		}
	}
	else if (mode->colortype == LCT_GREY_ALPHA)
	{
		if (mode->bitdepth == 8)
		{
			*r = *g = *b = in[i * 2 + 0];
			*a = in[i * 2 + 1];
		}
		else
		{
			*r = *g = *b = in[i * 4 + 0];
			*a = in[i * 4 + 2];
		}
	}
	else if (mode->colortype == LCT_RGBA)
	{
		if (mode->bitdepth == 8)
		{
			*r = in[i * 4 + 0];
			*g = in[i * 4 + 1];
			*b = in[i * 4 + 2];
			*a = in[i * 4 + 3];
		}
		else
		{
			*r = in[i * 8 + 0];
			*g = in[i * 8 + 2];
			*b = in[i * 8 + 4];
			*a = in[i * 8 + 6];
		}
	}
}

/*Similar to getPixelColorRGBA8, but with all the for loops inside of the color
mode test cases, optimized to convert the colors much faster, when converting
to RGBA or RGB with 8 bit per cannel. buffer must be RGBA or RGB output with
enough memory, if has_alpha is true the output is RGBA. mode has the color mode
of the input buffer.*/
static void getPixelColorsRGBA8(unsigned char *buffer, size_t numpixels,
								unsigned has_alpha, const unsigned char *in,
								const LodePNGColorMode *mode)
{
	unsigned num_channels = has_alpha ? 4 : 3;
	size_t i;
	if (mode->colortype == LCT_GREY)
	{
		if (mode->bitdepth == 8)
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = buffer[1] = buffer[2] = in[i];
				if (has_alpha)
					buffer[3] = mode->key_defined && in[i] == mode->key_r ? 0 : 255;
			}
		}
		else if (mode->bitdepth == 16)
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = buffer[1] = buffer[2] = in[i * 2];
				if (has_alpha)
					buffer[3] = mode->key_defined && 256U * in[i * 2 + 0] + in[i * 2 + 1] == mode->key_r ? 0 : 255;
			}
		}
		else
		{
			unsigned highest = ((1U << mode->bitdepth) - 1U);
			size_t j = 0;
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				unsigned value = readBitsFromReversedStream(&j, in, mode->bitdepth);
				buffer[0] = buffer[1] = buffer[2] = (value * 255) / highest;
				if (has_alpha)
					buffer[3] = mode->key_defined && value == mode->key_r ? 0 : 255;
			}
		}
	}
	else if (mode->colortype == LCT_RGB)
	{
		if (mode->bitdepth == 8)
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = in[i * 3 + 0];
				buffer[1] = in[i * 3 + 1];
				buffer[2] = in[i * 3 + 2];
				if (has_alpha)
					buffer[3] = mode->key_defined && buffer[0] == mode->key_r && buffer[1] == mode->key_g && buffer[2] == mode->key_b ? 0 : 255;
			}
		}
		else
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = in[i * 6 + 0];
				buffer[1] = in[i * 6 + 2];
				buffer[2] = in[i * 6 + 4];
				if (has_alpha)
					buffer[3] = mode->key_defined && 256U * in[i * 6 + 0] + in[i * 6 + 1] == mode->key_r && 256U * in[i * 6 + 2] + in[i * 6 + 3] == mode->key_g && 256U * in[i * 6 + 4] + in[i * 6 + 5] == mode->key_b ? 0 : 255;
			}
		}
	}
	else if (mode->colortype == LCT_PALETTE)
	{
		unsigned index;
		size_t j = 0;
		for (i = 0; i != numpixels; ++i, buffer += num_channels)
		{
			if (mode->bitdepth == 8)
				index = in[i];
			else
				index = readBitsFromReversedStream(&j, in, mode->bitdepth);

			if (index >= mode->palettesize)
			{
				/*This is an error according to the PNG spec, but most PNG decoders make it black instead.
				Done here too, slightly faster due to no error handling needed.*/
				buffer[0] = buffer[1] = buffer[2] = 0;
				if (has_alpha)
					buffer[3] = 255;
			}
			else
			{
				buffer[0] = mode->palette[index * 4 + 0];
				buffer[1] = mode->palette[index * 4 + 1];
				buffer[2] = mode->palette[index * 4 + 2];
				if (has_alpha)
					buffer[3] = mode->palette[index * 4 + 3];
			}
		}
	}
	else if (mode->colortype == LCT_GREY_ALPHA)
	{
		if (mode->bitdepth == 8)
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = buffer[1] = buffer[2] = in[i * 2 + 0];
				if (has_alpha)
					buffer[3] = in[i * 2 + 1];
			}
		}
		else
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = buffer[1] = buffer[2] = in[i * 4 + 0];
				if (has_alpha)
					buffer[3] = in[i * 4 + 2];
			}
		}
	}
	else if (mode->colortype == LCT_RGBA)
	{
		if (mode->bitdepth == 8)
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = in[i * 4 + 0];
				buffer[1] = in[i * 4 + 1];
				buffer[2] = in[i * 4 + 2];
				if (has_alpha)
					buffer[3] = in[i * 4 + 3];
			}
		}
		else
		{
			for (i = 0; i != numpixels; ++i, buffer += num_channels)
			{
				buffer[0] = in[i * 8 + 0];
				buffer[1] = in[i * 8 + 2];
				buffer[2] = in[i * 8 + 4];
				if (has_alpha)
					buffer[3] = in[i * 8 + 6];
			}
		}
	}
}

unsigned lodepng_convert(unsigned char *out, const unsigned char *in,
						 const LodePNGColorMode *mode_out, const LodePNGColorMode *mode_in,
						 unsigned w, unsigned h)
{
	size_t i;
	ColorTree tree;
	size_t numpixels = (size_t)w * (size_t)h;
	unsigned error = 0;

	if (lodepng_color_mode_equal(mode_out, mode_in))
	{
		size_t numbytes = lodepng_get_raw_size(w, h, mode_in);
		for (i = 0; i != numbytes; ++i)
			out[i] = in[i];
		return 0;
	}

	if (mode_out->colortype == LCT_PALETTE)
	{
		size_t palettesize = mode_out->palettesize;
		const unsigned char *palette = mode_out->palette;
		size_t palsize = (size_t)1u << mode_out->bitdepth;
		/*if the user specified output palette but did not give the values, assume
		they want the values of the input color type (assuming that one is palette).
		Note that we never create a new palette ourselves.*/
		if (palettesize == 0)
		{
			palettesize = mode_in->palettesize;
			palette = mode_in->palette;
			/*if the input was also palette with same bitdepth, then the color types are also
			equal, so copy literally. This to preserve the exact indices that were in the PNG
			even in case there are duplicate colors in the palette.*/
			if (mode_in->colortype == LCT_PALETTE && mode_in->bitdepth == mode_out->bitdepth)
			{
				size_t numbytes = lodepng_get_raw_size(w, h, mode_in);
				for (i = 0; i != numbytes; ++i)
					out[i] = in[i];
				return 0;
			}
		}
		if (palettesize < palsize)
			palsize = palettesize;
		color_tree_init(&tree);
		for (i = 0; i != palsize; ++i)
		{
			const unsigned char *p = &palette[i * 4];
			color_tree_add(&tree, p[0], p[1], p[2], p[3], (unsigned)i);
		}
	}

	if (mode_in->bitdepth == 16 && mode_out->bitdepth == 16)
	{
		for (i = 0; i != numpixels; ++i)
		{
			unsigned short r = 0, g = 0, b = 0, a = 0;
			getPixelColorRGBA16(&r, &g, &b, &a, in, i, mode_in);
			rgba16ToPixel(out, i, mode_out, r, g, b, a);
		}
	}
	else if (mode_out->bitdepth == 8 && mode_out->colortype == LCT_RGBA)
	{
		getPixelColorsRGBA8(out, numpixels, 1, in, mode_in);
	}
	else if (mode_out->bitdepth == 8 && mode_out->colortype == LCT_RGB)
	{
		getPixelColorsRGBA8(out, numpixels, 0, in, mode_in);
	}
	else
	{
		unsigned char r = 0, g = 0, b = 0, a = 0;
		for (i = 0; i != numpixels; ++i)
		{
			getPixelColorRGBA8(&r, &g, &b, &a, in, i, mode_in);
			error = rgba8ToPixel(out, i, mode_out, &tree, r, g, b, a);
			if (error)
				break;
		}
	}

	if (mode_out->colortype == LCT_PALETTE)
	{
		color_tree_cleanup(&tree);
	}

	return error;
}

/* Converts a single rgb color without alpha from one type to another, color bits truncated to
their bitdepth. In case of single channel (gray or palette), only the r channel is used. Slow
function, do not use to process all pixels of an image. Alpha channel not supported on purpose:
this is for bKGD, supporting alpha may prevent it from finding a color in the palette, from the
specification it looks like bKGD should ignore the alpha values of the palette since it can use
any palette index but doesn't have an alpha channel. Idem with ignoring color key. */
unsigned lodepng_convert_rgb(
	unsigned *r_out, unsigned *g_out, unsigned *b_out,
	unsigned r_in, unsigned g_in, unsigned b_in,
	const LodePNGColorMode *mode_out, const LodePNGColorMode *mode_in)
{
	unsigned r = 0, g = 0, b = 0;
	unsigned mul = 65535 / ((1u << mode_in->bitdepth) - 1u);
	unsigned shift = 16 - mode_out->bitdepth;

	if (mode_in->colortype == LCT_GREY || mode_in->colortype == LCT_GREY_ALPHA)
	{
		r = g = b = r_in * mul;
	}
	else if (mode_in->colortype == LCT_RGB || mode_in->colortype == LCT_RGBA)
	{
		r = r_in * mul;
		g = g_in * mul;
		b = b_in * mul;
	}
	else if (mode_in->colortype == LCT_PALETTE)
	{
		if (r_in >= mode_in->palettesize)
			return 82;
		r = mode_in->palette[r_in * 4 + 0] * 257u;
		g = mode_in->palette[r_in * 4 + 1] * 257u;
		b = mode_in->palette[r_in * 4 + 2] * 257u;
	}
	else
	{
		return 31;
	}

	if (mode_out->colortype == LCT_GREY || mode_out->colortype == LCT_GREY_ALPHA)
	{
		*r_out = r >> shift;
	}
	else if (mode_out->colortype == LCT_RGB || mode_out->colortype == LCT_RGBA)
	{
		*r_out = r >> shift;
		*g_out = g >> shift;
		*b_out = b >> shift;
	}
	else if (mode_out->colortype == LCT_PALETTE)
	{
		unsigned i;

		if ((r >> 8) != (r & 255) || (g >> 8) != (g & 255) || (b >> 8) != (b & 255))
			return 82;
		for (i = 0; i < mode_out->palettesize; i++)
		{
			unsigned j = i * 4;
			if ((r >> 8) == mode_out->palette[j + 0] && (g >> 8) == mode_out->palette[j + 1] &&
				(b >> 8) == mode_out->palette[j + 2])
			{
				*r_out = i;
				return 0;
			}
		}
		return 82;
	}
	else
	{
		return 31;
	}

	return 0;
}