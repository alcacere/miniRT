/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auto_choose_color.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:45:34 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:45:34 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "types.h"
#include "colors.h"
#include "settings.h"

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS

#endif

/* forward declaration from color_profile.c */
unsigned lodepng_get_color_profile(LodePNGColorProfile *profile,
								   const unsigned char *in, unsigned w, unsigned h,
								   const LodePNGColorMode *mode_in);

/*Autochoose color model given the computed profile. mode_in is to copy palette order from
when relevant.*/
static unsigned auto_choose_color_from_profile(LodePNGColorMode *mode_out,
											   const LodePNGColorMode *mode_in,
											   const LodePNGColorProfile *prof)
{
	unsigned error = 0;
	unsigned palettebits, palette_ok;
	size_t i, n;
	size_t numpixels = prof->numpixels;

	unsigned alpha = prof->alpha;
	unsigned key = prof->key;
	unsigned bits = prof->bits;

	mode_out->key_defined = 0;

	if (key && numpixels <= 16)
	{
		alpha = 1;
		key = 0;
		if (bits < 8)
			bits = 8;
	}
	n = prof->numcolors;
	palettebits = n <= 2 ? 1 : (n <= 4 ? 2 : (n <= 16 ? 4 : 8));
	palette_ok = n <= 256 && bits <= 8;
	if (numpixels < n * 2)
		palette_ok = 0;
	if (!prof->colored && bits <= palettebits)
		palette_ok = 0;

	if (palette_ok)
	{
		const unsigned char *p = prof->palette;
		lodepng_palette_clear(mode_out);
		for (i = 0; i != prof->numcolors; ++i)
		{
			error = lodepng_palette_add(mode_out, p[i * 4 + 0], p[i * 4 + 1], p[i * 4 + 2], p[i * 4 + 3]);
			if (error)
				break;
		}

		mode_out->colortype = LCT_PALETTE;
		mode_out->bitdepth = palettebits;

		if (mode_in->colortype == LCT_PALETTE && mode_in->palettesize >= mode_out->palettesize && mode_in->bitdepth == mode_out->bitdepth)
		{

			lodepng_color_mode_cleanup(mode_out);
			lodepng_color_mode_copy(mode_out, mode_in);
		}
	}
	else
	{
		mode_out->bitdepth = bits;
		mode_out->colortype = alpha ? (prof->colored ? LCT_RGBA : LCT_GREY_ALPHA)
									: (prof->colored ? LCT_RGB : LCT_GREY);

		if (key)
		{
			unsigned mask = (1u << mode_out->bitdepth) - 1u;
			mode_out->key_r = prof->key_r & mask;
			mode_out->key_g = prof->key_g & mask;
			mode_out->key_b = prof->key_b & mask;
			mode_out->key_defined = 1;
		}
	}

	return error;
}

/*Automatically chooses color type that gives smallest amount of bits in the
output image, e.g. gray if there are only grayscale pixels, palette if there
are less than 256 colors, color key if only single transparent color, ...
Updates values of mode with a potentially smaller color model. mode_out should
contain the user chosen color model, but will be overwritten with the new chosen one.*/
unsigned lodepng_auto_choose_color(LodePNGColorMode *mode_out,
								   const unsigned char *image, unsigned w, unsigned h,
								   const LodePNGColorMode *mode_in)
{
	unsigned error = 0;
	LodePNGColorProfile prof;
	lodepng_color_profile_init(&prof);
	error = lodepng_get_color_profile(&prof, image, w, h, mode_in);
	if (error)
		return error;
	return auto_choose_color_from_profile(mode_out, mode_in, &prof);
}
