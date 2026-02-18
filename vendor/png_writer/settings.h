/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:26:56 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:26:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PNGWRITER_SETTINGS_H
#define PNGWRITER_SETTINGS_H

#include "types.h" /* for LodePNG*Settings structs */

/* Avoid redefinition if you already have these elsewhere */
#ifndef LODEPNG_SETTINGS_INIT_FUNCS_DEFINED
#define LODEPNG_SETTINGS_INIT_FUNCS_DEFINED

static inline void lodepng_compress_settings_init(LodePNGCompressSettings *s)
{
	/* keep defaults conservative; encoder can override */
	s->btype = 2;
	s->use_lz77 = 1;
	s->windowsize = 2048;
	s->minmatch = 3;
	s->nicematch = 128;
	s->lazymatching = 1;
	s->custom_zlib = 0;
	s->custom_deflate = 0;
	s->custom_context = 0;
}

static inline void lodepng_decompress_settings_init(LodePNGDecompressSettings *s)
{
	s->ignore_adler32 = 0;
	s->custom_zlib = 0;
	s->custom_inflate = 0;
	s->custom_context = 0;
}

static inline void lodepng_decoder_settings_init(LodePNGDecoderSettings *settings)
{
	settings->color_convert = 1;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
	settings->read_text_chunks = 1;
	settings->remember_unknown_chunks = 0;
#endif
	settings->ignore_crc = 0;
	settings->ignore_critical = 0;
	settings->ignore_end = 0;
	lodepng_decompress_settings_init(&settings->zlibsettings);
}

static inline void lodepng_color_profile_init(LodePNGColorProfile *p)
{
	p->colored = 0;
	p->alpha = 0;
	p->key = 0;
	p->key_r = p->key_g = p->key_b = 0;
	p->bits = 1;
	p->numpixels = 0;
	p->numcolors = 0;
}

static inline void lodepng_encoder_settings_init(LodePNGEncoderSettings *s)
{
	lodepng_compress_settings_init(&s->zlibsettings);
	s->filter_palette_zero = 1;
	s->filter_strategy = LFS_MINSUM;
	s->auto_convert = 1;
	s->force_palette = 0;
	s->text_compression = 1;
	s->add_id = 0;
}

#endif /* LODEPNG_SETTINGS_INIT_FUNCS_DEFINED */

#endif /* PNGWRITER_SETTINGS_H */