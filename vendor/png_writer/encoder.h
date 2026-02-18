/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encoder.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:11:32 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:11:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENCODER_H
#define ENCODER_H

#include "huffman.h"
#include "colors.h"
#include "scan.h"

/* Forward declarations */
static inline unsigned encodeLZ77(uivector *out, Hash *hash,
								  const unsigned char *in, size_t inpos, size_t insize, unsigned windowsize,
								  unsigned minmatch, unsigned nicematch, unsigned lazymatching);
static inline void writeLZ77data(size_t *bp, ucvector *out, const uivector *lz77_encoded,
								 const HuffmanTree *tree_ll, const HuffmanTree *tree_d);
static inline void addLengthDistance(uivector *out, unsigned length, unsigned distance);

/*
LZ77-encode the data. Return value is error code. The input are raw bytes, the output
is in the form of unsigned integers with codes representing for example literal bytes, or
length/distance pairs.
It uses a hash table technique to let it encode faster. When doing LZ77 encoding, a
sliding window (of windowsize) is used, and all past bytes in that window can be used as
the "dictionary". A brute force search through all possible distances would be slow, and
this hash technique is one out of several ways to speed this up.
*/
static inline unsigned encodeLZ77(uivector *out, Hash *hash,
								  const unsigned char *in, size_t inpos, size_t insize, unsigned windowsize,
								  unsigned minmatch, unsigned nicematch, unsigned lazymatching)
{
	size_t pos;
	unsigned i, error = 0;

	unsigned maxchainlength = windowsize >= 8192 ? windowsize : windowsize / 8;
	unsigned maxlazymatch = windowsize >= 8192 ? MAX_SUPPORTED_DEFLATE_LENGTH : 64;

	unsigned usezeros = 1;
	unsigned numzeros = 0;

	unsigned offset;
	unsigned length;
	unsigned lazy = 0;
	unsigned lazylength = 0, lazyoffset = 0;
	unsigned hashval;
	unsigned current_offset, current_length;
	unsigned prev_offset;
	const unsigned char *lastptr, *foreptr, *backptr;
	unsigned hashpos;

	if (windowsize == 0 || windowsize > 32768)
		return 60;
	if ((windowsize & (windowsize - 1)) != 0)
		return 90;

	if (nicematch > MAX_SUPPORTED_DEFLATE_LENGTH)
		nicematch = MAX_SUPPORTED_DEFLATE_LENGTH;

	for (pos = inpos; pos < insize; ++pos)
	{
		size_t wpos = pos & (windowsize - 1);
		unsigned chainlength = 0;

		hashval = getHash(in, insize, pos);

		if (usezeros && hashval == 0)
		{
			if (numzeros == 0)
				numzeros = countZeros(in, insize, pos);
			else if (pos + numzeros > insize || in[pos + numzeros - 1] != 0)
				--numzeros;
		}
		else
		{
			numzeros = 0;
		}

		updateHashChain(hash, wpos, hashval, numzeros);

		length = 0;
		offset = 0;

		hashpos = hash->chain[wpos];

		lastptr = &in[insize < pos + MAX_SUPPORTED_DEFLATE_LENGTH ? insize : pos + MAX_SUPPORTED_DEFLATE_LENGTH];

		prev_offset = 0;
		for (;;)
		{
			if (chainlength++ >= maxchainlength)
				break;
			current_offset = (unsigned)(hashpos <= wpos ? wpos - hashpos : wpos - hashpos + windowsize);

			if (current_offset < prev_offset)
				break;
			prev_offset = current_offset;
			if (current_offset > 0)
			{

				foreptr = &in[pos];
				backptr = &in[pos - current_offset];

				if (numzeros >= 3)
				{
					unsigned skip = hash->zeros[hashpos];
					if (skip > numzeros)
						skip = numzeros;
					backptr += skip;
					foreptr += skip;
				}

				while (foreptr != lastptr && *backptr == *foreptr)
				{
					++backptr;
					++foreptr;
				}
				current_length = (unsigned)(foreptr - &in[pos]);

				if (current_length > length)
				{
					length = current_length;
					offset = current_offset;
					/*jump out once a length of max length is found (speed gain). This also jumps
					out if length is MAX_SUPPORTED_DEFLATE_LENGTH*/
					if (current_length >= nicematch)
						break;
				}
			}

			if (hashpos == hash->chain[hashpos])
				break;

			if (numzeros >= 3 && length > numzeros)
			{
				hashpos = hash->chainz[hashpos];
				if (hash->zeros[hashpos] != numzeros)
					break;
			}
			else
			{
				hashpos = hash->chain[hashpos];

				if (hash->val[hashpos] != (int)hashval)
					break;
			}
		}

		if (lazymatching)
		{
			if (!lazy && length >= 3 && length <= maxlazymatch && length < MAX_SUPPORTED_DEFLATE_LENGTH)
			{
				lazy = 1;
				lazylength = length;
				lazyoffset = offset;
				continue;
			}
			if (lazy)
			{
				lazy = 0;
				if (pos == 0)
					ERROR_BREAK(81);
				if (length > lazylength + 1)
				{

					if (!uivector_push_back(out, in[pos - 1]))
						ERROR_BREAK(83);
				}
				else
				{
					length = lazylength;
					offset = lazyoffset;
					hash->head[hashval] = -1;
					hash->headz[numzeros] = -1;
					--pos;
				}
			}
		}
		if (length >= 3 && offset > windowsize)
			ERROR_BREAK(86);

		if (length < 3)
		{
			if (!uivector_push_back(out, in[pos]))
				ERROR_BREAK(83);
		}
		else if (length < minmatch || (length == 3 && offset > 4096))
		{
			/*compensate for the fact that longer offsets have more extra bits, a
			length of only 3 may be not worth it then*/
			if (!uivector_push_back(out, in[pos]))
				ERROR_BREAK(83);
		}
		else
		{
			addLengthDistance(out, length, offset);
			for (i = 1; i < length; ++i)
			{
				++pos;
				wpos = pos & (windowsize - 1);
				hashval = getHash(in, insize, pos);
				if (usezeros && hashval == 0)
				{
					if (numzeros == 0)
						numzeros = countZeros(in, insize, pos);
					else if (pos + numzeros > insize || in[pos + numzeros - 1] != 0)
						--numzeros;
				}
				else
				{
					numzeros = 0;
				}
				updateHashChain(hash, wpos, hashval, numzeros);
			}
		}
	}

	return error;
}

/*
write the lz77-encoded data, which has lit, len and dist codes, to compressed stream using huffman trees.
tree_ll: the tree for lit and len codes.
tree_d: the tree for distance codes.
*/
static inline void writeLZ77data(size_t *bp, ucvector *out, const uivector *lz77_encoded,
								 const HuffmanTree *tree_ll, const HuffmanTree *tree_d)
{
	size_t i = 0;
	for (i = 0; i != lz77_encoded->size; ++i)
	{
		unsigned val = lz77_encoded->data[i];
		addHuffmanSymbol(bp, out, HuffmanTree_getCode(tree_ll, val), HuffmanTree_getLength(tree_ll, val));
		if (val > 256)
		{
			unsigned length_index = val - FIRST_LENGTH_CODE_INDEX;
			unsigned n_length_extra_bits = LENGTHEXTRA[length_index];
			unsigned length_extra_bits = lz77_encoded->data[++i];

			unsigned distance_code = lz77_encoded->data[++i];

			unsigned distance_index = distance_code;
			unsigned n_distance_extra_bits = DISTANCEEXTRA[distance_index];
			unsigned distance_extra_bits = lz77_encoded->data[++i];

			addBitsToStream(bp, out, length_extra_bits, n_length_extra_bits);
			addHuffmanSymbol(bp, out, HuffmanTree_getCode(tree_d, distance_code),
							 HuffmanTree_getLength(tree_d, distance_code));
			addBitsToStream(bp, out, distance_extra_bits, n_distance_extra_bits);
		}
	}
}

static inline void addLengthDistance(uivector *out, unsigned length, unsigned distance)
{
	unsigned length_code = length - 3;
	unsigned distance_code = distance - 1;
	uivector_push_back(out, length_code > 279 ? (length_code - 280 + 280) : length_code + 257);
	uivector_push_back(out, length > 258 ? 258 : length);
	uivector_push_back(out, distance_code);
}

static inline unsigned lodepng_encode(unsigned char **out, size_t *outsize,
									  const unsigned char *image, unsigned w, unsigned h,
									  LodePNGState *state)
{
	unsigned char *data = 0;
	size_t datasize = 0;
	ucvector outv;
	LodePNGInfo info;

	ucvector_init(&outv);
	lodepng_info_init(&info);

	*out = 0;
	*outsize = 0;
	state->error = 0;

	if ((state->info_png.color.colortype == LCT_PALETTE || state->encoder.force_palette) && (state->info_png.color.palettesize == 0 || state->info_png.color.palettesize > 256))
	{
		state->error = 68;
		goto cleanup;
	}
	if (state->encoder.zlibsettings.btype > 2)
	{
		state->error = 61;
		goto cleanup;
	}
	if (state->info_png.interlace_method > 1)
	{
		state->error = 71;
		goto cleanup;
	}
	state->error = checkColorValidity(state->info_png.color.colortype, state->info_png.color.bitdepth);
	if (state->error)
		goto cleanup;
	state->error = checkColorValidity(state->info_raw.colortype, state->info_raw.bitdepth);
	if (state->error)
		goto cleanup;

	lodepng_info_copy(&info, &state->info_png);
	if (state->encoder.auto_convert)
	{
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
		if (state->info_png.background_defined)
		{
			unsigned bg_r = state->info_png.background_r;
			unsigned bg_g = state->info_png.background_g;
			unsigned bg_b = state->info_png.background_b;
			unsigned r = 0, g = 0, b = 0;
			LodePNGColorProfile prof;
			LodePNGColorMode mode16 = lodepng_color_mode_make(LCT_RGB, 16);
			lodepng_convert_rgb(&r, &g, &b, bg_r, bg_g, bg_b, &mode16, &state->info_png.color);
			lodepng_color_profile_init(&prof);
			state->error = lodepng_get_color_profile(&prof, image, w, h, &state->info_raw);
			if (state->error)
				goto cleanup;
			lodepng_color_profile_add(&prof, r, g, b, 65535);
			state->error = auto_choose_color_from_profile(&info.color, &state->info_raw, &prof);
			if (state->error)
				goto cleanup;
			if (lodepng_convert_rgb(&info.background_r, &info.background_g, &info.background_b,
									bg_r, bg_g, bg_b, &info.color, &state->info_png.color))
			{
				state->error = 104;
				goto cleanup;
			}
		}
		else
#endif
		{
			state->error = lodepng_auto_choose_color(&info.color, image, w, h, &state->info_raw);
			if (state->error)
				goto cleanup;
		}
	}
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
	if (state->info_png.iccp_defined)
	{
		unsigned gray_icc = isGrayICCProfile(state->info_png.iccp_profile, state->info_png.iccp_profile_size);
		unsigned gray_png = info.color.colortype == LCT_GREY || info.color.colortype == LCT_GREY_ALPHA;
		/* TODO: perhaps instead of giving errors or less optimal compression, we can automatically modify
		the ICC profile here to say "GRAY" or "RGB " to match the PNG color type, unless this will require
		non trivial changes to the rest of the ICC profile */
		if (!gray_icc && !isRGBICCProfile(state->info_png.iccp_profile, state->info_png.iccp_profile_size))
		{
			state->error = 100;
			goto cleanup;
		}
		if (!state->encoder.auto_convert && gray_icc != gray_png)
		{
			/* Non recoverable: encoder not allowed to convert color type, and requested color type not
			compatible with ICC color type */
			state->error = 101;
			goto cleanup;
		}
		if (gray_icc && !gray_png)
		{

			state->error = 102;
			goto cleanup;
		}
		if (!gray_icc && gray_png)
		{
			/* Recoverable but an unfortunate loss in compression density: We have grayscale pixels but
			are forced to store them in more expensive RGB format that will repeat each value 3 times
			because the PNG spec does not allow an RGB ICC profile with internal grayscale color data */
			if (info.color.colortype == LCT_GREY)
				info.color.colortype = LCT_RGB;
			if (info.color.colortype == LCT_GREY_ALPHA)
				info.color.colortype = LCT_RGBA;
			if (info.color.bitdepth < 8)
				info.color.bitdepth = 8;
		}
	}
#endif
	if (!lodepng_color_mode_equal(&state->info_raw, &info.color))
	{
		unsigned char *converted;
		size_t size = ((size_t)w * (size_t)h * (size_t)lodepng_get_bpp(&info.color) + 7) / 8;

		converted = (unsigned char *)lodepng_malloc(size);
		if (!converted && size)
			state->error = 83;
		if (!state->error)
		{
			state->error = lodepng_convert(converted, image, &info.color, &state->info_raw, w, h);
		}
		if (!state->error)
			preProcessScanlines(&data, &datasize, converted, w, h, &info, &state->encoder);
		lodepng_free(converted);
		if (state->error)
			goto cleanup;
	}
	else
		preProcessScanlines(&data, &datasize, image, w, h, &info, &state->encoder);

	{
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
		size_t i;
#endif

		writeSignature(&outv);

		addChunk_IHDR(&outv, w, h, info.color.colortype, info.color.bitdepth, info.interlace_method);
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS

		if (info.unknown_chunks_data[0])
		{
			state->error = addUnknownChunks(&outv, info.unknown_chunks_data[0], info.unknown_chunks_size[0]);
			if (state->error)
				goto cleanup;
		}

		if (info.iccp_defined)
			addChunk_iCCP(&outv, &info, &state->encoder.zlibsettings);
		if (info.srgb_defined)
			addChunk_sRGB(&outv, &info);
		if (info.gama_defined)
			addChunk_gAMA(&outv, &info);
		if (info.chrm_defined)
			addChunk_cHRM(&outv, &info);
#endif

		if (info.color.colortype == LCT_PALETTE)
		{
			addChunk_PLTE(&outv, &info.color);
		}
		if (state->encoder.force_palette && (info.color.colortype == LCT_RGB || info.color.colortype == LCT_RGBA))
		{
			addChunk_PLTE(&outv, &info.color);
		}

		if (info.color.colortype == LCT_PALETTE && getPaletteTranslucency(info.color.palette, info.color.palettesize) != 0)
		{
			addChunk_tRNS(&outv, &info.color);
		}
		if ((info.color.colortype == LCT_GREY || info.color.colortype == LCT_RGB) && info.color.key_defined)
		{
			addChunk_tRNS(&outv, &info.color);
		}
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS

		if (info.background_defined)
		{
			state->error = addChunk_bKGD(&outv, &info);
			if (state->error)
				goto cleanup;
		}

		if (info.phys_defined)
			addChunk_pHYs(&outv, &info);

		if (info.unknown_chunks_data[1])
		{
			state->error = addUnknownChunks(&outv, info.unknown_chunks_data[1], info.unknown_chunks_size[1]);
			if (state->error)
				goto cleanup;
		}
#endif

		state->error = addChunk_IDAT(&outv, data, datasize, &state->encoder.zlibsettings);
		if (state->error)
			goto cleanup;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS

		if (info.time_defined)
			addChunk_tIME(&outv, &info.time);

		for (i = 0; i != info.text_num; ++i)
		{
			if (strlen(info.text_keys[i]) > 79)
			{
				state->error = 66;
				goto cleanup;
			}
			if (strlen(info.text_keys[i]) < 1)
			{
				state->error = 67;
				goto cleanup;
			}
			if (state->encoder.text_compression)
			{
				addChunk_zTXt(&outv, info.text_keys[i], info.text_strings[i], &state->encoder.zlibsettings);
			}
			else
			{
				addChunk_tEXt(&outv, info.text_keys[i], info.text_strings[i]);
			}
		}

		if (state->encoder.add_id)
		{
			unsigned already_added_id_text = 0;
			for (i = 0; i != info.text_num; ++i)
			{
				if (!strcmp(info.text_keys[i], "LodePNG"))
				{
					already_added_id_text = 1;
					break;
				}
			}
			if (already_added_id_text == 0)
			{
				addChunk_tEXt(&outv, "LodePNG", LODEPNG_VERSION_STRING);
			}
		}

		for (i = 0; i != info.itext_num; ++i)
		{
			if (strlen(info.itext_keys[i]) > 79)
			{
				state->error = 66;
				goto cleanup;
			}
			if (strlen(info.itext_keys[i]) < 1)
			{
				state->error = 67;
				goto cleanup;
			}
			addChunk_iTXt(&outv, state->encoder.text_compression,
						  info.itext_keys[i], info.itext_langtags[i], info.itext_transkeys[i], info.itext_strings[i],
						  &state->encoder.zlibsettings);
		}

		if (info.unknown_chunks_data[2])
		{
			state->error = addUnknownChunks(&outv, info.unknown_chunks_data[2], info.unknown_chunks_size[2]);
			if (state->error)
				goto cleanup;
		}
#endif
		addChunk_IEND(&outv);
	}

cleanup:
	lodepng_info_cleanup(&info);
	lodepng_free(data);

	*out = outv.data;
	*outsize = outv.size;

	return state->error;
}

static inline unsigned lodepng_encode_memory(unsigned char **out, size_t *outsize, const unsigned char *image,
											 unsigned w, unsigned h, LodePNGColorType colortype, unsigned bitdepth)
{
	unsigned error;
	LodePNGState state;
	lodepng_state_init(&state);
	state.info_raw.colortype = colortype;
	state.info_raw.bitdepth = bitdepth;
	state.info_png.color.colortype = colortype;
	state.info_png.color.bitdepth = bitdepth;
	lodepng_encode(out, outsize, image, w, h, &state);
	error = state.error;
	lodepng_state_cleanup(&state);
	return error;
}

static inline unsigned lodepng_encode32(unsigned char **out, size_t *outsize, const unsigned char *image, unsigned w, unsigned h)
{
	return lodepng_encode_memory(out, outsize, image, w, h, LCT_RGBA, 8);
}

static inline unsigned lodepng_encode24(unsigned char **out, size_t *outsize, const unsigned char *image, unsigned w, unsigned h)
{
	return lodepng_encode_memory(out, outsize, image, w, h, LCT_RGB, 8);
}

#ifdef LODEPNG_COMPILE_DISK
static inline unsigned lodepng_encode_file(const char *filename, const unsigned char *image, unsigned w, unsigned h,
										   LodePNGColorType colortype, unsigned bitdepth)
{
	unsigned char *buffer;
	size_t buffersize;
	unsigned error = lodepng_encode_memory(&buffer, &buffersize, image, w, h, colortype, bitdepth);
	if (!error)
		error = lodepng_save_file(buffer, buffersize, filename);
	lodepng_free(buffer);
	return error;
}

static inline unsigned lodepng_encode32_file(const char *filename, const unsigned char *image, unsigned w, unsigned h)
{
	return lodepng_encode_file(filename, image, w, h, LCT_RGBA, 8);
}

static inline unsigned lodepng_encode24_file(const char *filename, const unsigned char *image, unsigned w, unsigned h)
{
	return lodepng_encode_file(filename, image, w, h, LCT_RGB, 8);
}
#endif

#endif