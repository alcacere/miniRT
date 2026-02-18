/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color_profile.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:44:44 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:44:44 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "colors.h"
#include "bit.h"

/* Forward declarations */
void getPixelColorRGBA16(unsigned short *r, unsigned short *g, unsigned short *b, unsigned short *a,
						 const unsigned char *in, size_t i, const LodePNGColorMode *mode);
void getPixelColorRGBA8(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a,
						const unsigned char *in, size_t i, const LodePNGColorMode *mode);

static int color_tree_has(ColorTree *tree, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return color_tree_get(tree, r, g, b, a) >= 0;
}

unsigned lodepng_get_color_profile(LodePNGColorProfile *profile,
								   const unsigned char *in, unsigned w, unsigned h,
								   const LodePNGColorMode *mode_in)
{
	unsigned error = 0;
	size_t i;
	ColorTree tree;
	size_t numpixels = (size_t)w * (size_t)h;

	unsigned colored_done = lodepng_is_greyscale_type(mode_in) ? 1 : 0;
	unsigned alpha_done = lodepng_can_have_alpha(mode_in) ? 0 : 1;
	unsigned numcolors_done = 0;
	unsigned bpp = lodepng_get_bpp(mode_in);
	unsigned bits_done = (profile->bits == 1 && bpp == 1) ? 1 : 0;
	unsigned sixteen = 0;
	unsigned maxnumcolors = 257;
	if (bpp <= 8)
		maxnumcolors = LODEPNG_MIN(257, profile->numcolors + (1u << bpp));

	profile->numpixels += numpixels;

	color_tree_init(&tree);

	/*If the profile was already filled in from previous data, fill its palette in tree
	and mark things as done already if we know they are the most expensive case already*/
	if (profile->alpha)
		alpha_done = 1;
	if (profile->colored)
		colored_done = 1;
	if (profile->bits == 16)
		numcolors_done = 1;
	if (profile->bits >= bpp)
		bits_done = 1;
	if (profile->numcolors >= maxnumcolors)
		numcolors_done = 1;

	if (!numcolors_done)
	{
		for (i = 0; i < profile->numcolors; i++)
		{
			const unsigned char *color = &profile->palette[i * 4];
			color_tree_add(&tree, color[0], color[1], color[2], color[3], i);
		}
	}

	if (mode_in->bitdepth == 16 && !sixteen)
	{
		unsigned short r, g, b, a;
		for (i = 0; i != numpixels; ++i)
		{
			getPixelColorRGBA16(&r, &g, &b, &a, in, i, mode_in);
			if ((r & 255) != ((r >> 8) & 255) || (g & 255) != ((g >> 8) & 255) ||
				(b & 255) != ((b >> 8) & 255) || (a & 255) != ((a >> 8) & 255))
			{
				profile->bits = 16;
				sixteen = 1;
				bits_done = 1;
				numcolors_done = 1;
				break;
			}
		}
	}

	if (sixteen)
	{
		unsigned short r = 0, g = 0, b = 0, a = 0;

		for (i = 0; i != numpixels; ++i)
		{
			getPixelColorRGBA16(&r, &g, &b, &a, in, i, mode_in);

			if (!colored_done && (r != g || r != b))
			{
				profile->colored = 1;
				colored_done = 1;
			}

			if (!alpha_done)
			{
				unsigned matchkey = (r == profile->key_r && g == profile->key_g && b == profile->key_b);
				if (a != 65535 && (a != 0 || (profile->key && !matchkey)))
				{
					profile->alpha = 1;
					profile->key = 0;
					alpha_done = 1;
				}
				else if (a == 0 && !profile->alpha && !profile->key)
				{
					profile->key = 1;
					profile->key_r = r;
					profile->key_g = g;
					profile->key_b = b;
				}
				else if (a == 65535 && profile->key && matchkey)
				{

					profile->alpha = 1;
					profile->key = 0;
					alpha_done = 1;
				}
			}
			if (alpha_done && numcolors_done && colored_done && bits_done)
				break;
		}

		if (profile->key && !profile->alpha)
		{
			for (i = 0; i != numpixels; ++i)
			{
				getPixelColorRGBA16(&r, &g, &b, &a, in, i, mode_in);
				if (a != 0 && r == profile->key_r && g == profile->key_g && b == profile->key_b)
				{

					profile->alpha = 1;
					profile->key = 0;
					alpha_done = 1;
				}
			}
		}
	}
	else
	{
		unsigned char r = 0, g = 0, b = 0, a = 0;
		for (i = 0; i != numpixels; ++i)
		{
			getPixelColorRGBA8(&r, &g, &b, &a, in, i, mode_in);

			if (!bits_done && profile->bits < 8)
			{

				unsigned bits = getValueRequiredBits(r);
				if (bits > profile->bits)
					profile->bits = bits;
			}
			bits_done = (profile->bits >= bpp);

			if (!colored_done && (r != g || r != b))
			{
				profile->colored = 1;
				colored_done = 1;
				if (profile->bits < 8)
					profile->bits = 8;
			}

			if (!alpha_done)
			{
				unsigned matchkey = (r == profile->key_r && g == profile->key_g && b == profile->key_b);
				if (a != 255 && (a != 0 || (profile->key && !matchkey)))
				{
					profile->alpha = 1;
					profile->key = 0;
					alpha_done = 1;
					if (profile->bits < 8)
						profile->bits = 8;
				}
				else if (a == 0 && !profile->alpha && !profile->key)
				{
					profile->key = 1;
					profile->key_r = r;
					profile->key_g = g;
					profile->key_b = b;
				}
				else if (a == 255 && profile->key && matchkey)
				{

					profile->alpha = 1;
					profile->key = 0;
					alpha_done = 1;
					if (profile->bits < 8)
						profile->bits = 8;
				}
			}

			if (!numcolors_done)
			{
				if (!color_tree_has(&tree, r, g, b, a))
				{
					color_tree_add(&tree, r, g, b, a, profile->numcolors);
					if (profile->numcolors < 256)
					{
						unsigned char *p = profile->palette;
						unsigned n = profile->numcolors;
						p[n * 4 + 0] = r;
						p[n * 4 + 1] = g;
						p[n * 4 + 2] = b;
						p[n * 4 + 3] = a;
					}
					++profile->numcolors;
					numcolors_done = profile->numcolors >= maxnumcolors;
				}
			}

			if (alpha_done && numcolors_done && colored_done && bits_done)
				break;
		}

		if (profile->key && !profile->alpha)
		{
			for (i = 0; i != numpixels; ++i)
			{
				getPixelColorRGBA8(&r, &g, &b, &a, in, i, mode_in);
				if (a != 0 && r == profile->key_r && g == profile->key_g && b == profile->key_b)
				{

					profile->alpha = 1;
					profile->key = 0;
					alpha_done = 1;
					if (profile->bits < 8)
						profile->bits = 8;
				}
			}
		}

		profile->key_r += (profile->key_r << 8);
		profile->key_g += (profile->key_g << 8);
		profile->key_b += (profile->key_b << 8);
	}

	color_tree_cleanup(&tree);
	return error;
}

/*Adds a single color to the color profile. The profile must already have been inited. The color must be given as 16-bit
(with 2 bytes repeating for 8-bit and 65535 for opaque alpha channel). This function is expensive, do not call it for
all pixels of an image but only for a few additional values. */
static unsigned lodepng_color_profile_add(LodePNGColorProfile *profile,
										  unsigned r, unsigned g, unsigned b, unsigned a)
{
	unsigned error = 0;
	unsigned char image[8];
	LodePNGColorMode mode;
	lodepng_color_mode_init(&mode);
	image[0] = r >> 8;
	image[1] = r;
	image[2] = g >> 8;
	image[3] = g;
	image[4] = b >> 8;
	image[5] = b;
	image[6] = a >> 8;
	image[7] = a;
	mode.bitdepth = 16;
	mode.colortype = LCT_RGBA;
	error = lodepng_get_color_profile(profile, image, 1, 1, &mode);
	lodepng_color_mode_cleanup(&mode);
	return error;
}