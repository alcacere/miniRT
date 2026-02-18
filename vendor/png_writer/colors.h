/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:33:56 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:33:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLORS_H
#define COLORS_H

#include <stddef.h>
#include <stdlib.h>
#include "types.h"
#include "utils.h"

/**
 * One node of a color tree
 * This is the data structure used to count the number of unique colors and
 * to get a palette index for a color. It's like an octree, but because the
 * alpha channel is used too, each node has 16 instead of 8 children
 */
/* ColorTree typedef is in types.h */

static unsigned checkColorValidity(LodePNGColorType colortype, unsigned bd)
{
	switch (colortype)
	{
	case 0:
		if (!(bd == 1 || bd == 2 || bd == 4 || bd == 8 || bd == 16))
			return 37;
		break;
	case 2:
		if (!(bd == 8 || bd == 16))
			return 37;
		break;
	case 3:
		if (!(bd == 1 || bd == 2 || bd == 4 || bd == 8))
			return 37;
		break;
	case 4:
		if (!(bd == 8 || bd == 16))
			return 37;
		break;
	case 6:
		if (!(bd == 8 || bd == 16))
			return 37;
		break;
	default:
		return 31;
	}
	return 0;
}

static unsigned getNumColorChannels(LodePNGColorType colortype)
{
	switch (colortype)
	{
	case 0:
		return 1;
	case 2:
		return 3;
	case 3:
		return 1;
	case 4:
		return 2;
	case 6:
		return 4;
	}
	return 0;
}

static unsigned lodepng_get_bpp_lct(LodePNGColorType colortype, unsigned bitdepth)
{

	return getNumColorChannels(colortype) * bitdepth;
}

static inline void lodepng_palette_clear(LodePNGColorMode *info)
{
	if (info->palette)
		lodepng_free(info->palette);
	info->palette = 0;
	info->palettesize = 0;
}

static inline void lodepng_color_mode_init(LodePNGColorMode *info)
{
	info->key_defined = 0;
	info->key_r = info->key_g = 0;
	info->colortype = LCT_RGBA;
	info->bitdepth = 8;
	info->palette = 0;
	info->palettesize = 0;
}

static inline void lodepng_color_mode_cleanup(LodePNGColorMode *info)
{
	lodepng_palette_clear(info);
}

static inline unsigned lodepng_color_mode_copy(LodePNGColorMode *dest, const LodePNGColorMode *source)
{
	size_t i;
	lodepng_color_mode_cleanup(dest);
	*dest = *source;
	if (source->palette)
	{
		dest->palette = (unsigned char *)lodepng_malloc(1024);
		if (!dest->palette && source->palettesize)
			return 83;
		for (i = 0; i != source->palettesize * 4; ++i)
			dest->palette[i] = source->palette[i];
	}
	return 0;
}

static inline LodePNGColorMode lodepng_color_mode_make(LodePNGColorType colortype, unsigned bitdepth)
{
	LodePNGColorMode result;
	lodepng_color_mode_init(&result);
	result.colortype = colortype;
	result.bitdepth = bitdepth;
	return result;
}

static inline int lodepng_color_mode_equal(const LodePNGColorMode *a, const LodePNGColorMode *b)
{
	size_t i;
	if (a->colortype != b->colortype)
		return 0;
	if (a->bitdepth != b->bitdepth)
		return 0;
	if (a->key_defined != b->key_defined)
		return 0;
	if (a->key_defined)
	{
		if (a->key_r != b->key_r)
			return 0;
		if (a->key_g != b->key_g)
			return 0;
		if (a->key_b != b->key_b)
			return 0;
	}
	if (a->palettesize != b->palettesize)
		return 0;
	for (i = 0; i != a->palettesize * 4; ++i)
	{
		if (a->palette[i] != b->palette[i])
			return 0;
	}
	return 1;
}

static inline unsigned lodepng_palette_add(LodePNGColorMode *info,
										   unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	unsigned char *data;
	/*the same resize technique as C++ std::vectors is used, and here it's made so that for a palette with
	the max of 256 colors, it'll have the exact alloc size*/
	if (!info->palette)
	{

		data = (unsigned char *)lodepng_realloc(info->palette, 1024);
		if (!data)
			return 83;
		else
			info->palette = data;
	}
	info->palette[4 * info->palettesize + 0] = r;
	info->palette[4 * info->palettesize + 1] = g;
	info->palette[4 * info->palettesize + 2] = b;
	info->palette[4 * info->palettesize + 3] = a;
	++info->palettesize;
	return 0;
}

static inline unsigned lodepng_get_bpp(const LodePNGColorMode *info)
{
	return lodepng_get_bpp_lct(info->colortype, info->bitdepth);
}

static inline unsigned lodepng_get_channels(const LodePNGColorMode *info)
{
	return getNumColorChannels(info->colortype);
}

static inline unsigned lodepng_is_greyscale_type(const LodePNGColorMode *info)
{
	return info->colortype == LCT_GREY || info->colortype == LCT_GREY_ALPHA;
}

static inline unsigned lodepng_is_alpha_type(const LodePNGColorMode *info)
{
	return (info->colortype & 4) != 0;
}

static inline unsigned lodepng_is_palette_type(const LodePNGColorMode *info)
{
	return info->colortype == LCT_PALETTE;
}

static inline unsigned lodepng_has_palette_alpha(const LodePNGColorMode *info)
{
	size_t i;
	for (i = 0; i != info->palettesize; ++i)
	{
		if (info->palette[i * 4 + 3] < 255)
			return 1;
	}
	return 0;
}

static inline unsigned lodepng_can_have_alpha(const LodePNGColorMode *info)
{
	return info->key_defined || lodepng_is_alpha_type(info) || lodepng_has_palette_alpha(info);
}

static inline size_t lodepng_get_raw_size_lct(unsigned w, unsigned h, LodePNGColorType colortype, unsigned bitdepth)
{
	size_t bpp = lodepng_get_bpp_lct(colortype, bitdepth);
	size_t n = (size_t)w * (size_t)h;
	return ((n / 8) * bpp) + ((n & 7) * bpp + 7) / 8;
}

static inline size_t lodepng_get_raw_size(unsigned w, unsigned h, const LodePNGColorMode *color)
{
	return lodepng_get_raw_size_lct(w, h, color->colortype, color->bitdepth);
}

static void addColorBits(unsigned char *out, size_t index, unsigned bits, unsigned in)
{
	unsigned m = bits == 1 ? 7 : bits == 2 ? 3
										   : 1;

	unsigned p = index & m;
	in &= (1u << bits) - 1u;
	in = in << (bits * (m - p));
	if (p == 0)
		out[index * bits / 8] = in;
	else
		out[index * bits / 8] |= in;
}

static void color_tree_init(ColorTree *tree)
{
	int i;
	for (i = 0; i != 16; ++i)
		tree->children[i] = 0;
	tree->index = -1;
}

static void color_tree_cleanup(ColorTree *tree)
{
	int i;
	for (i = 0; i != 16; ++i)
	{
		if (tree->children[i])
		{
			color_tree_cleanup(tree->children[i]);
			lodepng_free(tree->children[i]);
		}
	}
}

static int color_tree_get(ColorTree *tree, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	int bit = 0;
	for (bit = 0; bit < 8; ++bit)
	{
		int i = 8 * ((r >> bit) & 1) + 4 * ((g >> bit) & 1) + 2 * ((b >> bit) & 1) + 1 * ((a >> bit) & 1);
		if (!tree->children[i])
			return -1;
		else
			tree = tree->children[i];
	}
	return tree ? tree->index : -1;
}

#ifdef LODEPNG_COMPILE_ENCODER
static int color_tree_has(ColorTree *tree, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return color_tree_get(tree, r, g, b, a) >= 0;
}
#endif

/*color is not allowed to already exist.
Index should be >= 0 (it's signed to be compatible with using -1 for "doesn't exist")*/
static void color_tree_add(ColorTree *tree,
						   unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned index)
{
	int bit;
	for (bit = 0; bit < 8; ++bit)
	{
		int i = 8 * ((r >> bit) & 1) + 4 * ((g >> bit) & 1) + 2 * ((b >> bit) & 1) + 1 * ((a >> bit) & 1);
		if (!tree->children[i])
		{
			tree->children[i] = (ColorTree *)lodepng_malloc(sizeof(ColorTree));
			color_tree_init(tree->children[i]);
		}
		tree = tree->children[i];
	}
	tree->index = (int)index;
}

/* Paeth predictor, per PNG spec. Use int math to avoid overflow artifacts. */
static unsigned char paethPredictor(int a, int b, int c)
{
	int p = a + b - c;
	int pa = p > a ? p - a : a - p;
	int pb = p > b ? p - b : b - p;
	int pc = p > c ? p - c : c - p;

	if (pa <= pb && pa <= pc)
		return (unsigned char)a;
	if (pb <= pc)
		return (unsigned char)b;
	return (unsigned char)c;
}

unsigned lodepng_convert(unsigned char *out, const unsigned char *in,
						 const LodePNGColorMode *mode_out, const LodePNGColorMode *mode_in,
						 unsigned w, unsigned h);
unsigned lodepng_auto_choose_color(LodePNGColorMode *mode_out,
								   const unsigned char *image, unsigned w, unsigned h,
								   const LodePNGColorMode *mode_in);
unsigned getPaletteTranslucency(const unsigned char *palette, size_t palettesize);

#endif