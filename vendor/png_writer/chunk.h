/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:33:01 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:33:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHUNK_H
#define CHUNK_H

/* forward decl to avoid implicit use inside addChunk_IDAT/zTXt/iTXt */
static unsigned zlib_compress(unsigned char **out, size_t *outsize,
							  const unsigned char *in, size_t insize,
							  const LodePNGCompressSettings *settings);

static inline void lodepng_chunk_type(char type[5], const unsigned char *chunk)
{
	unsigned i;
	for (i = 0; i != 4; ++i)
		type[i] = (char)chunk[4 + i];
	type[4] = 0;
}

static inline unsigned char lodepng_chunk_type_equals(const unsigned char *chunk, const char *type)
{
	if (strlen(type) != 4)
		return 0;
	return (chunk[4] == type[0] && chunk[5] == type[1] && chunk[6] == type[2] && chunk[7] == type[3]);
}

static inline unsigned char lodepng_chunk_ancillary(const unsigned char *chunk)
{
	return ((chunk[4] & 32) != 0);
}

static inline unsigned char lodepng_chunk_private(const unsigned char *chunk)
{
	return ((chunk[6] & 32) != 0);
}

static inline unsigned char lodepng_chunk_safetocopy(const unsigned char *chunk)
{
	return ((chunk[7] & 32) != 0);
}

static inline unsigned char *lodepng_chunk_data(unsigned char *chunk)
{
	return &chunk[8];
}

static inline const unsigned char *lodepng_chunk_data_const(const unsigned char *chunk)
{
	return &chunk[8];
}

static inline unsigned lodepng_chunk_check_crc(const unsigned char *chunk)
{
	unsigned length = lodepng_chunk_length(chunk);
	unsigned CRC = lodepng_read32bitInt(&chunk[length + 8]);

	unsigned checksum = lodepng_crc32(&chunk[4], length + 4);
	if (CRC != checksum)
		return 1;
	else
		return 0;
}

static inline void lodepng_chunk_generate_crc(unsigned char *chunk)
{
	unsigned length = lodepng_chunk_length(chunk);
	unsigned CRC = lodepng_crc32(&chunk[4], length + 4);
	lodepng_set32bitInt(chunk + 8 + length, CRC);
}

static inline unsigned char *lodepng_chunk_next(unsigned char *chunk)
{
	if (chunk[0] == 0x89 && chunk[1] == 0x50 && chunk[2] == 0x4e && chunk[3] == 0x47 && chunk[4] == 0x0d && chunk[5] == 0x0a && chunk[6] == 0x1a && chunk[7] == 0x0a)
	{

		return chunk + 8;
	}
	else
	{
		unsigned total_chunk_length = lodepng_chunk_length(chunk) + 12;
		return chunk + total_chunk_length;
	}
}

static inline const unsigned char *lodepng_chunk_next_const(const unsigned char *chunk)
{
	if (chunk[0] == 0x89 && chunk[1] == 0x50 && chunk[2] == 0x4e && chunk[3] == 0x47 && chunk[4] == 0x0d && chunk[5] == 0x0a && chunk[6] == 0x1a && chunk[7] == 0x0a)
	{

		return chunk + 8;
	}
	else
	{
		unsigned total_chunk_length = lodepng_chunk_length(chunk) + 12;
		return chunk + total_chunk_length;
	}
}

static inline unsigned char *lodepng_chunk_find(unsigned char *chunk, const unsigned char *end, const char type[5])
{
	for (;;)
	{
		if (chunk + 12 >= end)
			return 0;
		if (lodepng_chunk_type_equals(chunk, type))
			return chunk;
		chunk = lodepng_chunk_next(chunk);
	}
}

static inline const unsigned char *lodepng_chunk_find_const(const unsigned char *chunk, const unsigned char *end, const char type[5])
{
	for (;;)
	{
		if (chunk + 12 >= end)
			return 0;
		if (lodepng_chunk_type_equals(chunk, type))
			return chunk;
		chunk = lodepng_chunk_next_const(chunk);
	}
}

static inline unsigned lodepng_chunk_append(unsigned char **out, size_t *outlength, const unsigned char *chunk)
{
	unsigned i;
	unsigned total_chunk_length = lodepng_chunk_length(chunk) + 12;
	unsigned char *chunk_start, *new_buffer;
	size_t new_length = (*outlength) + total_chunk_length;
	if (new_length < total_chunk_length || new_length < (*outlength))
		return 77;

	new_buffer = (unsigned char *)lodepng_realloc(*out, new_length);
	if (!new_buffer)
		return 83;
	(*out) = new_buffer;
	(*outlength) = new_length;
	chunk_start = &(*out)[new_length - total_chunk_length];

	for (i = 0; i != total_chunk_length; ++i)
		chunk_start[i] = chunk[i];

	return 0;
}

static inline unsigned lodepng_chunk_create(unsigned char **out, size_t *outlength, unsigned length,
											const char *type, const unsigned char *data)
{
	unsigned i;
	unsigned char *chunk, *new_buffer;
	size_t new_length = (*outlength) + length + 12;
	if (new_length < length + 12 || new_length < (*outlength))
		return 77;
	new_buffer = (unsigned char *)lodepng_realloc(*out, new_length);
	if (!new_buffer)
		return 83;
	(*out) = new_buffer;
	(*outlength) = new_length;
	chunk = &(*out)[(*outlength) - length - 12];

	lodepng_set32bitInt(chunk, (unsigned)length);

	chunk[4] = (unsigned char)type[0];
	chunk[5] = (unsigned char)type[1];
	chunk[6] = (unsigned char)type[2];
	chunk[7] = (unsigned char)type[3];

	for (i = 0; i != length; ++i)
		chunk[8 + i] = data[i];

	lodepng_chunk_generate_crc(chunk);

	return 0;
}

static void LodePNGUnknownChunks_init(LodePNGInfo *info)
{
	unsigned i;
	for (i = 0; i != 3; ++i)
		info->unknown_chunks_data[i] = 0;
	for (i = 0; i != 3; ++i)
		info->unknown_chunks_size[i] = 0;
}

static void LodePNGUnknownChunks_cleanup(LodePNGInfo *info)
{
	unsigned i;
	for (i = 0; i != 3; ++i)
		lodepng_free(info->unknown_chunks_data[i]);
}

static unsigned LodePNGUnknownChunks_copy(LodePNGInfo *dest, const LodePNGInfo *src)
{
	unsigned i;

	LodePNGUnknownChunks_cleanup(dest);

	for (i = 0; i != 3; ++i)
	{
		size_t j;
		dest->unknown_chunks_size[i] = src->unknown_chunks_size[i];
		dest->unknown_chunks_data[i] = (unsigned char *)lodepng_malloc(src->unknown_chunks_size[i]);
		if (!dest->unknown_chunks_data[i] && dest->unknown_chunks_size[i])
			return 83;
		for (j = 0; j < src->unknown_chunks_size[i]; ++j)
		{
			dest->unknown_chunks_data[i][j] = src->unknown_chunks_data[i][j];
		}
	}

	return 0;
}

static inline unsigned addChunk(ucvector *out, const char *chunkName, const unsigned char *data, size_t length)
{
	CERROR_TRY_RETURN(lodepng_chunk_create(&out->data, &out->size, (unsigned)length, chunkName, data));
	out->allocsize = out->size;
	return 0;
}

static inline unsigned addChunk_IHDR(ucvector *out, unsigned w, unsigned h,
									 LodePNGColorType colortype, unsigned bitdepth, unsigned interlace_method)
{
	unsigned error = 0;
	ucvector header;
	ucvector_init(&header);

	lodepng_add32bitInt(&header, w);
	lodepng_add32bitInt(&header, h);
	ucvector_push_back(&header, (unsigned char)bitdepth);
	ucvector_push_back(&header, (unsigned char)colortype);
	ucvector_push_back(&header, 0);
	ucvector_push_back(&header, 0);
	ucvector_push_back(&header, interlace_method);

	error = addChunk(out, "IHDR", header.data, header.size);
	ucvector_cleanup(&header);

	return error;
}

static inline unsigned addChunk_PLTE(ucvector *out, const LodePNGColorMode *info)
{
	unsigned error = 0;
	size_t i;
	ucvector PLTE;
	ucvector_init(&PLTE);
	for (i = 0; i != info->palettesize * 4; ++i)
	{

		if (i % 4 != 3)
			ucvector_push_back(&PLTE, info->palette[i]);
	}
	error = addChunk(out, "PLTE", PLTE.data, PLTE.size);
	ucvector_cleanup(&PLTE);

	return error;
}

static inline unsigned addChunk_tRNS(ucvector *out, const LodePNGColorMode *info)
{
	unsigned error = 0;
	size_t i;
	ucvector tRNS;
	ucvector_init(&tRNS);
	if (info->colortype == LCT_PALETTE)
	{
		size_t amount = info->palettesize;

		for (i = info->palettesize; i != 0; --i)
		{
			if (info->palette[4 * (i - 1) + 3] == 255)
				--amount;
			else
				break;
		}

		for (i = 0; i != amount; ++i)
			ucvector_push_back(&tRNS, info->palette[4 * i + 3]);
	}
	else if (info->colortype == LCT_GREY)
	{
		if (info->key_defined)
		{
			ucvector_push_back(&tRNS, (unsigned char)(info->key_r >> 8));
			ucvector_push_back(&tRNS, (unsigned char)(info->key_r & 255));
		}
	}
	else if (info->colortype == LCT_RGB)
	{
		if (info->key_defined)
		{
			ucvector_push_back(&tRNS, (unsigned char)(info->key_r >> 8));
			ucvector_push_back(&tRNS, (unsigned char)(info->key_r & 255));
			ucvector_push_back(&tRNS, (unsigned char)(info->key_g >> 8));
			ucvector_push_back(&tRNS, (unsigned char)(info->key_g & 255));
			ucvector_push_back(&tRNS, (unsigned char)(info->key_b >> 8));
			ucvector_push_back(&tRNS, (unsigned char)(info->key_b & 255));
		}
	}

	error = addChunk(out, "tRNS", tRNS.data, tRNS.size);
	ucvector_cleanup(&tRNS);

	return error;
}

static inline unsigned addChunk_IDAT(ucvector *out, const unsigned char *data, size_t datasize,
									 LodePNGCompressSettings *zlibsettings)
{
	ucvector zlibdata;
	unsigned error = 0;

	ucvector_init(&zlibdata);
	error = zlib_compress(&zlibdata.data, &zlibdata.size, data, datasize, zlibsettings);
	if (!error)
		error = addChunk(out, "IDAT", zlibdata.data, zlibdata.size);
	ucvector_cleanup(&zlibdata);

	return error;
}

static inline unsigned addChunk_IEND(ucvector *out)
{
	unsigned error = 0;
	error = addChunk(out, "IEND", 0, 0);
	return error;
}

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS

static inline unsigned addChunk_tEXt(ucvector *out, const char *keyword, const char *textstring)
{
	unsigned error = 0;
	size_t i;
	ucvector text;
	ucvector_init(&text);
	for (i = 0; keyword[i] != 0; ++i)
		ucvector_push_back(&text, (unsigned char)keyword[i]);
	if (i < 1 || i > 79)
		return 89;
	ucvector_push_back(&text, 0);
	for (i = 0; textstring[i] != 0; ++i)
		ucvector_push_back(&text, (unsigned char)textstring[i]);
	error = addChunk(out, "tEXt", text.data, text.size);
	ucvector_cleanup(&text);

	return error;
}

static inline unsigned addChunk_zTXt(ucvector *out, const char *keyword, const char *textstring,
									 LodePNGCompressSettings *zlibsettings)
{
	unsigned error = 0;
	ucvector data, compressed;
	size_t i, textsize = strlen(textstring);

	ucvector_init(&data);
	ucvector_init(&compressed);
	for (i = 0; keyword[i] != 0; ++i)
		ucvector_push_back(&data, (unsigned char)keyword[i]);
	if (i < 1 || i > 79)
		return 89;
	ucvector_push_back(&data, 0);
	ucvector_push_back(&data, 0);

	error = zlib_compress(&compressed.data, &compressed.size,
						  (unsigned char *)textstring, textsize, zlibsettings);
	if (!error)
	{
		for (i = 0; i != compressed.size; ++i)
			ucvector_push_back(&data, compressed.data[i]);
		error = addChunk(out, "zTXt", data.data, data.size);
	}

	ucvector_cleanup(&compressed);
	ucvector_cleanup(&data);
	return error;
}

static inline unsigned addChunk_iTXt(ucvector *out, unsigned compressed, const char *keyword, const char *langtag,
									 const char *transkey, const char *textstring, LodePNGCompressSettings *zlibsettings)
{
	unsigned error = 0;
	ucvector data;
	size_t i, textsize = strlen(textstring);

	ucvector_init(&data);

	for (i = 0; keyword[i] != 0; ++i)
		ucvector_push_back(&data, (unsigned char)keyword[i]);
	if (i < 1 || i > 79)
		return 89;
	ucvector_push_back(&data, 0);
	ucvector_push_back(&data, compressed ? 1 : 0);
	ucvector_push_back(&data, 0);
	for (i = 0; langtag[i] != 0; ++i)
		ucvector_push_back(&data, (unsigned char)langtag[i]);
	ucvector_push_back(&data, 0);
	for (i = 0; transkey[i] != 0; ++i)
		ucvector_push_back(&data, (unsigned char)transkey[i]);
	ucvector_push_back(&data, 0);

	if (compressed)
	{
		ucvector compressed_data;
		ucvector_init(&compressed_data);
		error = zlib_compress(&compressed_data.data, &compressed_data.size,
							  (unsigned char *)textstring, textsize, zlibsettings);
		if (!error)
		{
			for (i = 0; i != compressed_data.size; ++i)
				ucvector_push_back(&data, compressed_data.data[i]);
		}
		ucvector_cleanup(&compressed_data);
	}
	else
	{
		for (i = 0; textstring[i] != 0; ++i)
			ucvector_push_back(&data, (unsigned char)textstring[i]);
	}

	if (!error)
		error = addChunk(out, "iTXt", data.data, data.size);
	ucvector_cleanup(&data);
	return error;
}

static inline unsigned addChunk_bKGD(ucvector *out, const LodePNGInfo *info)
{
	unsigned error = 0;
	ucvector bKGD;
	ucvector_init(&bKGD);
	if (info->color.colortype == LCT_GREY || info->color.colortype == LCT_GREY_ALPHA)
	{
		ucvector_push_back(&bKGD, (unsigned char)(info->background_r >> 8));
		ucvector_push_back(&bKGD, (unsigned char)(info->background_r & 255));
	}
	else if (info->color.colortype == LCT_RGB || info->color.colortype == LCT_RGBA)
	{
		ucvector_push_back(&bKGD, (unsigned char)(info->background_r >> 8));
		ucvector_push_back(&bKGD, (unsigned char)(info->background_r & 255));
		ucvector_push_back(&bKGD, (unsigned char)(info->background_g >> 8));
		ucvector_push_back(&bKGD, (unsigned char)(info->background_g & 255));
		ucvector_push_back(&bKGD, (unsigned char)(info->background_b >> 8));
		ucvector_push_back(&bKGD, (unsigned char)(info->background_b & 255));
	}
	else if (info->color.colortype == LCT_PALETTE)
	{
		ucvector_push_back(&bKGD, (unsigned char)(info->background_r & 255));
	}

	error = addChunk(out, "bKGD", bKGD.data, bKGD.size);
	ucvector_cleanup(&bKGD);

	return error;
}

static inline unsigned addChunk_tIME(ucvector *out, const LodePNGTime *time)
{
	unsigned error = 0;
	unsigned char *data = (unsigned char *)lodepng_malloc(7);
	if (!data)
		return 83;
	data[0] = (unsigned char)(time->year >> 8);
	data[1] = (unsigned char)(time->year & 255);
	data[2] = (unsigned char)time->month;
	data[3] = (unsigned char)time->day;
	data[4] = (unsigned char)time->hour;
	data[5] = (unsigned char)time->minute;
	data[6] = (unsigned char)time->second;
	error = addChunk(out, "tIME", data, 7);
	lodepng_free(data);
	return error;
}

static inline unsigned addChunk_pHYs(ucvector *out, const LodePNGInfo *info)
{
	unsigned error = 0;
	ucvector data;
	ucvector_init(&data);

	lodepng_add32bitInt(&data, info->phys_x);
	lodepng_add32bitInt(&data, info->phys_y);
	ucvector_push_back(&data, info->phys_unit);

	error = addChunk(out, "pHYs", data.data, data.size);
	ucvector_cleanup(&data);

	return error;
}

static inline unsigned addChunk_gAMA(ucvector *out, const LodePNGInfo *info)
{
	unsigned error = 0;
	ucvector data;
	ucvector_init(&data);

	lodepng_add32bitInt(&data, info->gama_gamma);

	error = addChunk(out, "gAMA", data.data, data.size);
	ucvector_cleanup(&data);

	return error;
}

static inline unsigned addChunk_cHRM(ucvector *out, const LodePNGInfo *info)
{
	unsigned error = 0;
	ucvector data;
	ucvector_init(&data);

	lodepng_add32bitInt(&data, info->chrm_white_x);
	lodepng_add32bitInt(&data, info->chrm_white_y);
	lodepng_add32bitInt(&data, info->chrm_red_x);
	lodepng_add32bitInt(&data, info->chrm_red_y);
	lodepng_add32bitInt(&data, info->chrm_green_x);
	lodepng_add32bitInt(&data, info->chrm_green_y);
	lodepng_add32bitInt(&data, info->chrm_blue_x);
	lodepng_add32bitInt(&data, info->chrm_blue_y);

	error = addChunk(out, "cHRM", data.data, data.size);
	ucvector_cleanup(&data);

	return error;
}

static inline unsigned addChunk_sRGB(ucvector *out, const LodePNGInfo *info)
{
	unsigned char data = info->srgb_intent;
	return addChunk(out, "sRGB", &data, 1);
}

static inline unsigned addChunk_iCCP(ucvector *out, const LodePNGInfo *info, LodePNGCompressSettings *zlibsettings)
{
	unsigned error = 0;
	ucvector data, compressed;
	size_t i;

	ucvector_init(&data);
	ucvector_init(&compressed);
	for (i = 0; info->iccp_name[i] != 0; ++i)
		ucvector_push_back(&data, (unsigned char)info->iccp_name[i]);
	if (i < 1 || i > 79)
		return 89;
	ucvector_push_back(&data, 0);
	ucvector_push_back(&data, 0);

	error = zlib_compress(&compressed.data, &compressed.size,
						  info->iccp_profile, info->iccp_profile_size, zlibsettings);
	if (!error)
	{
		for (i = 0; i != compressed.size; ++i)
			ucvector_push_back(&data, compressed.data[i]);
		error = addChunk(out, "iCCP", data.data, data.size);
	}

	ucvector_cleanup(&compressed);
	ucvector_cleanup(&data);
	return error;
}

#endif
#endif

void writeSignature(ucvector *out);