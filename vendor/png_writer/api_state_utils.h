/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   api_state_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:50 by marvin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:50 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef API_STATE_UTILS_H
#define API_STATE_UTILS_H

#include "settings.h" /* ensure lodepng_*_settings_init are visible here */

static inline void lodepng_state_init(LodePNGState *state)
{
	lodepng_decoder_settings_init(&state->decoder);
	lodepng_encoder_settings_init(&state->encoder);
	lodepng_color_mode_init(&state->info_raw);
	lodepng_info_init(&state->info_png);
	state->error = 0;
}

static inline void lodepng_state_cleanup(LodePNGState *state)
{
	lodepng_color_mode_cleanup(&state->info_raw);
	lodepng_info_cleanup(&state->info_png);
}

static inline void lodepng_state_copy(LodePNGState *dest, const LodePNGState *source)
{
	lodepng_state_cleanup(dest);
	*dest = *source;
	lodepng_color_mode_init(&dest->info_raw);
	lodepng_info_init(&dest->info_png);
	dest->error = lodepng_color_mode_copy(&dest->info_raw, &source->info_raw);
	if (dest->error)
		return;
	dest->error = lodepng_info_copy(&dest->info_png, &source->info_png);
	if (dest->error)
		return;
}

#endif