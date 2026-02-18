/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:15:00 by marvin            #+#    #+#             */
/*   Updated: 2026/01/03 17:43:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdlib.h>

/* PNG color types */
typedef enum e_png_color_type
{
	LCT_GREY = 0,
	LCT_RGB = 2,
	LCT_PALETTE = 3,
	LCT_GREY_ALPHA = 4,
	LCT_RGBA = 6
} LodePNGColorType;

/* Color mode structure */
typedef struct s_png_color_mode
{
	LodePNGColorType colortype;
	unsigned bitdepth;
	unsigned char *palette;
	size_t palettesize;
	unsigned key_defined;
	unsigned key_r;
	unsigned key_g;
	unsigned key_b;
} LodePNGColorMode;

/* Color tree node for palette optimization */
typedef struct s_color_tree
{
	struct s_color_tree *children[16];
	int index;
} ColorTree;

/* Color profile for analyzing image statistics */
typedef struct s_png_color_profile
{
	unsigned colored;
	unsigned alpha;
	unsigned key;
	unsigned key_r;
	unsigned key_g;
	unsigned key_b;
	unsigned bits;
	size_t numpixels;
	unsigned numcolors;
	unsigned char palette[256 * 4];
} LodePNGColorProfile;

/* Time structure for tIME chunk */
typedef struct s_png_time
{
	unsigned year;
	unsigned month;
	unsigned day;
	unsigned hour;
	unsigned minute;
	unsigned second;
} LodePNGTime;

/* Info structure for PNG metadata */
typedef struct s_png_info
{
	unsigned compression_method;
	unsigned filter_method;
	unsigned interlace_method;
	LodePNGColorMode color;
	unsigned background_defined;
	unsigned background_r;
	unsigned background_g;
	unsigned background_b;
	size_t text_num;
	char **text_keys;
	char **text_strings;
	size_t itext_num;
	char **itext_keys;
	char **itext_langtags;
	char **itext_transkeys;
	char **itext_strings;
	unsigned time_defined;
	LodePNGTime time;
	unsigned phys_x;
	unsigned phys_y;
	unsigned phys_unit;
	unsigned gama_defined;
	unsigned gama_gamma;
	unsigned chrm_defined;
	unsigned chrm_white_x;
	unsigned chrm_white_y;
	unsigned chrm_red_x;
	unsigned chrm_red_y;
	unsigned chrm_green_x;
	unsigned chrm_green_y;
	unsigned chrm_blue_x;
	unsigned chrm_blue_y;
	unsigned srgb_defined;
	unsigned srgb_intent;
	unsigned iccp_defined;
	char *iccp_name;
	unsigned char *iccp_profile;
	unsigned iccp_profile_size;
	unsigned char *unknown_chunks_data[3];
	size_t unknown_chunks_size[3];
} LodePNGInfo;

/* Compress settings */
typedef struct s_compress_settings
{
	unsigned btype;
	unsigned use_lz77;
	unsigned windowsize;
	unsigned minmatch;
	unsigned nicematch;
	unsigned lazymatching;
	// optional custom hooks
	unsigned (*custom_zlib)(unsigned char **, size_t *, const unsigned char *, size_t, const struct s_compress_settings *);
	unsigned (*custom_deflate)(unsigned char **, size_t *, const unsigned char *, size_t, const struct s_compress_settings *);
	const void *custom_context;
} LodePNGCompressSettings;

typedef struct s_decompress_settings
{
	unsigned ignore_adler32;
	unsigned (*custom_zlib)(unsigned char **, size_t *, const unsigned char *, size_t, const struct s_decompress_settings *);
	unsigned (*custom_inflate)(unsigned char **, size_t *, const unsigned char *, size_t, const struct s_decompress_settings *);
	const void *custom_context;
} LodePNGDecompressSettings;

/* Filter strategy enum - DEFINED BEFORE USE */
typedef enum e_lodepng_filter_strategy
{
	LFS_ZERO = 0,
	LFS_MINSUM,
	LFS_ENTROPY,
	LFS_PREDEFINED,
	LFS_BRUTE_FORCE
} LodePNGFilterStrategy;

/* Encoder settings */
typedef struct s_encoder_settings
{
	LodePNGCompressSettings zlibsettings;
	unsigned filter_palette_zero;
	LodePNGFilterStrategy filter_strategy;
	unsigned auto_convert;
	unsigned force_palette;
	unsigned text_compression;
	unsigned add_id;
	const unsigned char *predefined_filters;
} LodePNGEncoderSettings;

/* Decoder settings */
typedef struct s_decoder_settings
{
	LodePNGDecompressSettings zlibsettings;
	unsigned color_convert;
	unsigned read_text_chunks;
	unsigned remember_unknown_chunks;
	unsigned ignore_crc;
	unsigned ignore_critical;
	unsigned ignore_end;
} LodePNGDecoderSettings;

/* Main PNG state */
typedef struct s_png_state
{
	LodePNGDecoderSettings decoder;
	LodePNGEncoderSettings encoder;
	LodePNGColorMode info_raw;
	LodePNGInfo info_png;
	unsigned error;
} LodePNGState;

/* Vector types */
// guard vector structs for re-includes
#ifndef UCVECTOR_DEFINED
#define UCVECTOR_DEFINED
typedef struct s_ucvector
{
	unsigned char *data;
	size_t size;
	size_t allocsize;
} ucvector;
#endif

#ifndef UIVECTOR_DEFINED
#define UIVECTOR_DEFINED
typedef struct s_uivector
{
	unsigned *data;
	size_t size;
	size_t allocsize;
} uivector;
#endif

/* Huffman tree */
#ifndef HUFFMAN_TREE_DEFINED
#define HUFFMAN_TREE_DEFINED
typedef struct s_huffman_tree
{
	unsigned *tree2d;
	unsigned *tree1d;
	unsigned *lengths;
	unsigned max_bit_len;
	unsigned numcodes;
} HuffmanTree;
#endif

/* Hash table for LZ77 */
#ifndef HASH_STRUCT_DEFINED
#define HASH_STRUCT_DEFINED
typedef struct s_hash
{
	int *head;
	unsigned short *chain;
	int *val;
	int *headz;
	unsigned short *chainz;
	unsigned short *zeros;
} Hash;
#endif

#ifndef HASH_CONSTANTS_DEFINED
#define HASH_CONSTANTS_DEFINED
#define HASH_NUM_VALUES 65536
#define HASH_BIT_MASK 65535
#define MAX_SUPPORTED_DEFLATE_LENGTH 258
#endif

#endif
