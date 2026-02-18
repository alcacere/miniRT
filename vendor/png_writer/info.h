/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   info.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:38:30 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:38:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INFO_H
#define INFO_H

static inline void lodepng_info_init(LodePNGInfo *info)
{
	lodepng_color_mode_init(&info->color);
	info->interlace_method = 0;
	info->compression_method = 0;
	info->filter_method = 0;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
	info->background_defined = 0;
	info->background_r = info->background_g = info->background_b = 0;

	LodePNGText_init(info);
	LodePNGIText_init(info);

	info->time_defined = 0;
	info->phys_defined = 0;

	info->gama_defined = 0;
	info->chrm_defined = 0;
	info->srgb_defined = 0;
	info->iccp_defined = 0;
	info->iccp_name = NULL;
	info->iccp_profile = NULL;

	LodePNGUnknownChunks_init(info);
#endif
}

static inline void lodepng_info_cleanup(LodePNGInfo *info)
{
	lodepng_color_mode_cleanup(&info->color);
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
	LodePNGText_cleanup(info);
	LodePNGIText_cleanup(info);

	lodepng_clear_icc(info);

	LodePNGUnknownChunks_cleanup(info);
#endif
}

static inline unsigned lodepng_info_copy(LodePNGInfo *dest, const LodePNGInfo *source)
{
	lodepng_info_cleanup(dest);
	*dest = *source;
	lodepng_color_mode_init(&dest->color);
	CERROR_TRY_RETURN(lodepng_color_mode_copy(&dest->color, &source->color));

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
	CERROR_TRY_RETURN(LodePNGText_copy(dest, source));
	CERROR_TRY_RETURN(LodePNGIText_copy(dest, source));
	if (source->iccp_defined)
	{
		CERROR_TRY_RETURN(lodepng_assign_icc(dest, source->iccp_name, source->iccp_profile, source->iccp_profile_size));
	}

	LodePNGUnknownChunks_init(dest);
	CERROR_TRY_RETURN(LodePNGUnknownChunks_copy(dest, source));
#endif
	return 0;
}

unsigned lodepng_inspect(unsigned *w, unsigned *h, LodePNGState *state,
						 const unsigned char *in, size_t insize);

#endif