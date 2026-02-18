/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decoder.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:11:19 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:11:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DECODER_H
#define DECODER_H

static inline void decodeGeneric(unsigned char **out, unsigned *w, unsigned *h,
								 LodePNGState *state,
								 const unsigned char *in, size_t insize)
{
	unsigned char IEND = 0;
	const unsigned char *chunk;
	size_t i;
	ucvector idat;
	ucvector scanlines;
	size_t predict;
	size_t outsize = 0;

	unsigned unknown = 0;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
	unsigned critical_pos = 1;
#endif

	*out = 0;
	*w = *h = 0;

	state->error = lodepng_inspect(w, h, state, in, insize);
	if (state->error)
		return;

	if (lodepng_pixel_overflow(*w, *h, &state->info_png.color, &state->info_raw))
	{
		CERROR_RETURN(state->error, 92);
	}

	ucvector_init(&idat);
	chunk = &in[33];

	/*loop through the chunks, ignoring unknown chunks and stopping at IEND chunk.
	IDAT data is put at the start of the in buffer*/
	while (!IEND && !state->error)
	{
		unsigned chunkLength;
		const unsigned char *data;

		if ((size_t)((chunk - in) + 12) > insize || chunk < in)
		{
			if (state->decoder.ignore_end)
				break;
			CERROR_BREAK(state->error, 30);
		}

		chunkLength = lodepng_chunk_length(chunk);

		if (chunkLength > 2147483647)
		{
			if (state->decoder.ignore_end)
				break;
			CERROR_BREAK(state->error, 63);
		}

		if ((size_t)((chunk - in) + chunkLength + 12) > insize || (chunk + chunkLength + 12) < in)
		{
			CERROR_BREAK(state->error, 64);
		}

		data = lodepng_chunk_data_const(chunk);

		unknown = 0;

		if (lodepng_chunk_type_equals(chunk, "IDAT"))
		{
			size_t oldsize = idat.size;
			size_t newsize;
			if (lodepng_addofl(oldsize, chunkLength, &newsize))
				CERROR_BREAK(state->error, 95);
			if (!ucvector_resize(&idat, newsize))
				CERROR_BREAK(state->error, 83);
			for (i = 0; i != chunkLength; ++i)
				idat.data[oldsize + i] = data[i];
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
			critical_pos = 3;
#endif
		}
		else if (lodepng_chunk_type_equals(chunk, "IEND"))
		{

			IEND = 1;
		}
		else if (lodepng_chunk_type_equals(chunk, "PLTE"))
		{

			state->error = readChunk_PLTE(&state->info_png.color, data, chunkLength);
			if (state->error)
				break;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
			critical_pos = 2;
#endif
		}
		else if (lodepng_chunk_type_equals(chunk, "tRNS"))
		{
			/*palette transparency chunk (tRNS). Even though this one is an ancillary chunk , it is still compiled
			in without 'LODEPNG_COMPILE_ANCILLARY_CHUNKS' because it contains essential color information that
			affects the alpha channel of pixels. */
			state->error = readChunk_tRNS(&state->info_png.color, data, chunkLength);
			if (state->error)
				break;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
		}
		else if (lodepng_chunk_type_equals(chunk, "bKGD"))
		{
			state->error = readChunk_bKGD(&state->info_png, data, chunkLength);
			if (state->error)
				break;
		}
		else if (lodepng_chunk_type_equals(chunk, "tEXt"))
		{

			if (state->decoder.read_text_chunks)
			{
				state->error = readChunk_tEXt(&state->info_png, data, chunkLength);
				if (state->error)
					break;
			}
		}
		else if (lodepng_chunk_type_equals(chunk, "zTXt"))
		{

			if (state->decoder.read_text_chunks)
			{
				state->error = readChunk_zTXt(&state->info_png, &state->decoder.zlibsettings, data, chunkLength);
				if (state->error)
					break;
			}
		}
		else if (lodepng_chunk_type_equals(chunk, "iTXt"))
		{

			if (state->decoder.read_text_chunks)
			{
				state->error = readChunk_iTXt(&state->info_png, &state->decoder.zlibsettings, data, chunkLength);
				if (state->error)
					break;
			}
		}
		else if (lodepng_chunk_type_equals(chunk, "tIME"))
		{
			state->error = readChunk_tIME(&state->info_png, data, chunkLength);
			if (state->error)
				break;
		}
		else if (lodepng_chunk_type_equals(chunk, "pHYs"))
		{
			state->error = readChunk_pHYs(&state->info_png, data, chunkLength);
			if (state->error)
				break;
		}
		else if (lodepng_chunk_type_equals(chunk, "gAMA"))
		{
			state->error = readChunk_gAMA(&state->info_png, data, chunkLength);
			if (state->error)
				break;
		}
		else if (lodepng_chunk_type_equals(chunk, "cHRM"))
		{
			state->error = readChunk_cHRM(&state->info_png, data, chunkLength);
			if (state->error)
				break;
		}
		else if (lodepng_chunk_type_equals(chunk, "sRGB"))
		{
			state->error = readChunk_sRGB(&state->info_png, data, chunkLength);
			if (state->error)
				break;
		}
		else if (lodepng_chunk_type_equals(chunk, "iCCP"))
		{
			state->error = readChunk_iCCP(&state->info_png, &state->decoder.zlibsettings, data, chunkLength);
			if (state->error)
				break;
#endif
		}
		else
		{

			if (!state->decoder.ignore_critical && !lodepng_chunk_ancillary(chunk))
			{
				CERROR_BREAK(state->error, 69);
			}

			unknown = 1;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
			if (state->decoder.remember_unknown_chunks)
			{
				state->error = lodepng_chunk_append(&state->info_png.unknown_chunks_data[critical_pos - 1],
													&state->info_png.unknown_chunks_size[critical_pos - 1], chunk);
				if (state->error)
					break;
			}
#endif
		}

		if (!state->decoder.ignore_crc && !unknown)
		{
			if (lodepng_chunk_check_crc(chunk))
				CERROR_BREAK(state->error, 57);
		}

		if (!IEND)
			chunk = lodepng_chunk_next_const(chunk);
	}

	ucvector_init(&scanlines);
	if (state->info_png.interlace_method == 0)
	{
		predict = lodepng_get_raw_size_idat(*w, *h, &state->info_png.color);
	}
	else
	{

		const LodePNGColorMode *color = &state->info_png.color;
		predict = 0;
		predict += lodepng_get_raw_size_idat((*w + 7) >> 3, (*h + 7) >> 3, color);
		if (*w > 4)
			predict += lodepng_get_raw_size_idat((*w + 3) >> 3, (*h + 7) >> 3, color);
		predict += lodepng_get_raw_size_idat((*w + 3) >> 2, (*h + 3) >> 3, color);
		if (*w > 2)
			predict += lodepng_get_raw_size_idat((*w + 1) >> 2, (*h + 3) >> 2, color);
		predict += lodepng_get_raw_size_idat((*w + 1) >> 1, (*h + 1) >> 2, color);
		if (*w > 1)
			predict += lodepng_get_raw_size_idat((*w + 0) >> 1, (*h + 1) >> 1, color);
		predict += lodepng_get_raw_size_idat((*w + 0), (*h + 0) >> 1, color);
	}
	if (!state->error && !ucvector_reserve(&scanlines, predict))
		state->error = 83;
	if (!state->error)
	{
		state->error = zlib_decompress(&scanlines.data, &scanlines.size, idat.data,
									   idat.size, &state->decoder.zlibsettings);
		if (!state->error && scanlines.size != predict)
			state->error = 91;
	}
	ucvector_cleanup(&idat);

	if (!state->error)
	{
		outsize = lodepng_get_raw_size(*w, *h, &state->info_png.color);
		*out = (unsigned char *)lodepng_malloc(outsize);
		if (!*out)
			state->error = 83;
	}
	if (!state->error)
	{
		for (i = 0; i < outsize; i++)
			(*out)[i] = 0;
		state->error = postProcessScanlines(*out, scanlines.data, *w, *h, &state->info_png);
	}
	ucvector_cleanup(&scanlines);
}

static inline unsigned lodepng_decode(unsigned char **out, unsigned *w, unsigned *h,
									  LodePNGState *state,
									  const unsigned char *in, size_t insize)
{
	*out = 0;
	decodeGeneric(out, w, h, state, in, insize);
	if (state->error)
		return state->error;
	if (!state->decoder.color_convert || lodepng_color_mode_equal(&state->info_raw, &state->info_png.color))
	{

		/*store the info_png color settings on the info_raw so that the info_raw still reflects what colortype
		the raw image has to the end user*/
		if (!state->decoder.color_convert)
		{
			state->error = lodepng_color_mode_copy(&state->info_raw, &state->info_png.color);
			if (state->error)
				return state->error;
		}
	}
	else
	{

		unsigned char *data = *out;
		size_t outsize;

		/*TODO: check if this works according to the statement in the documentation: "The converter can convert
		from grayscale input color type, to 8-bit grayscale or grayscale with alpha"*/
		if (!(state->info_raw.colortype == LCT_RGB || state->info_raw.colortype == LCT_RGBA) && !(state->info_raw.bitdepth == 8))
		{
			return 56;
		}

		outsize = lodepng_get_raw_size(*w, *h, &state->info_raw);
		*out = (unsigned char *)lodepng_malloc(outsize);
		if (!(*out))
		{
			state->error = 83;
		}
		else
			state->error = lodepng_convert(*out, data, &state->info_raw,
										   &state->info_png.color, *w, *h);
		lodepng_free(data);
	}
	return state->error;
}

static inline unsigned lodepng_decode_memory(unsigned char **out, unsigned *w, unsigned *h, const unsigned char *in,
											 size_t insize, LodePNGColorType colortype, unsigned bitdepth)
{
	unsigned error;
	LodePNGState state;
	lodepng_state_init(&state);
	state.info_raw.colortype = colortype;
	state.info_raw.bitdepth = bitdepth;
	error = lodepng_decode(out, w, h, &state, in, insize);
	lodepng_state_cleanup(&state);
	return error;
}

static inline unsigned lodepng_decode32_memory(unsigned char **out, unsigned *w, unsigned *h,
											   const unsigned char *in, size_t insize)
{
	return lodepng_decode_memory(out, w, h, in, insize, LCT_RGBA, 8);
}

static inline unsigned lodepng_decode32(unsigned char **out, unsigned *w, unsigned *h, const unsigned char *in, size_t insize)
{
	return lodepng_decode_memory(out, w, h, in, insize, LCT_RGBA, 8);
}

static inline unsigned lodepng_decode24(unsigned char **out, unsigned *w, unsigned *h, const unsigned char *in, size_t insize)
{
	return lodepng_decode_memory(out, w, h, in, insize, LCT_RGB, 8);
}

#ifdef LODEPNG_COMPILE_DISK
static inline unsigned lodepng_decode_file(unsigned char **out, unsigned *w, unsigned *h, const char *filename,
										   LodePNGColorType colortype, unsigned bitdepth)
{
	unsigned char *buffer = 0;
	size_t buffersize;
	unsigned error;

	*out = 0;
	*w = *h = 0;
	error = lodepng_load_file(&buffer, &buffersize, filename);
	if (!error)
		error = lodepng_decode_memory(out, w, h, buffer, buffersize, colortype, bitdepth);
	lodepng_free(buffer);
	return error;
}

static inline unsigned lodepng_decode32_file(unsigned char **out, unsigned *w, unsigned *h, const char *filename)
{
	return lodepng_decode_file(out, w, h, filename, LCT_RGBA, 8);
}

static inline unsigned lodepng_decode24_file(unsigned char **out, unsigned *w, unsigned *h, const char *filename)
{
	return lodepng_decode_file(out, w, h, filename, LCT_RGB, 8);
}
#endif
#endif