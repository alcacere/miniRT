/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deflate.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:19:44 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:19:44 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFLATE_H
#define DEFLATE_H

#define NUM_CODE_LENGTH_CODES 19
#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285
static const unsigned CLCL_ORDER[NUM_CODE_LENGTH_CODES] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

/* Forward declarations - encodeLZ77 and writeLZ77data are in encoder.h */
/* getTreeInflateFixed is declared in huffman.h, implementations below */
static inline void generateFixedLitLenTree(HuffmanTree *tree);
static inline void generateFixedDistanceTree(HuffmanTree *tree);

/* ...existing code... */

static unsigned deflateNoCompression(ucvector *out, const unsigned char *data, size_t datasize)
{
	/*non compressed deflate block data: 1 bit BFINAL,2 bits BTYPE,(5 bits): it jumps to start of next byte,
	2 bytes LEN, 2 bytes NLEN, LEN bytes literal DATA*/

	size_t i, j, numdeflateblocks = (datasize + 65534) / 65535;
	unsigned datapos = 0;
	for (i = 0; i != numdeflateblocks; ++i)
	{
		unsigned BFINAL, BTYPE, LEN, NLEN;
		unsigned char firstbyte;

		BFINAL = (i == numdeflateblocks - 1);
		BTYPE = 0;

		firstbyte = (unsigned char)(BFINAL + ((BTYPE & 1) << 1) + ((BTYPE & 2) << 1));
		ucvector_push_back(out, firstbyte);

		LEN = 65535;
		if (datasize - datapos < 65535)
			LEN = (unsigned)datasize - datapos;
		NLEN = 65535 - LEN;

		ucvector_push_back(out, (unsigned char)(LEN & 255));
		ucvector_push_back(out, (unsigned char)(LEN >> 8));
		ucvector_push_back(out, (unsigned char)(NLEN & 255));
		ucvector_push_back(out, (unsigned char)(NLEN >> 8));

		for (j = 0; j < 65535 && datapos < datasize; ++j)
		{
			ucvector_push_back(out, data[datapos++]);
		}
	}

	return 0;
}

static unsigned deflateDynamic(ucvector *out, size_t *bp, Hash *hash,
							   const unsigned char *data, size_t datapos, size_t dataend,
							   const LodePNGCompressSettings *settings, unsigned final)
{
	unsigned error = 0;

	/*
	A block is compressed as follows: The PNG data is lz77 encoded, resulting in
	literal bytes and length/distance pairs. This is then huffman compressed with
	two huffman trees. One huffman tree is used for the lit and len values ("ll"),
	another huffman tree is used for the dist values ("d"). These two trees are
	stored using their code lengths, and to compress even more these code lengths
	are also run-length encoded and huffman compressed. This gives a huffman tree
	of code lengths "cl". The code lenghts used to describe this third tree are
	the code length code lengths ("clcl").
	*/

	uivector lz77_encoded;
	HuffmanTree tree_ll;
	HuffmanTree tree_d;
	HuffmanTree tree_cl;
	uivector frequencies_ll;
	uivector frequencies_d;
	uivector frequencies_cl;
	uivector bitlen_lld;
	uivector bitlen_lld_e;
	/*bitlen_cl is the code length code lengths ("clcl"). The bit lengths of codes to represent tree_cl
	(these are written as is in the file, it would be crazy to compress these using yet another huffman
	tree that needs to be represented by yet another set of code lengths)*/
	uivector bitlen_cl;
	size_t datasize = dataend - datapos;

	/*
	Due to the huffman compression of huffman tree representations ("two levels"), there are some anologies:
	bitlen_lld is to tree_cl what data is to tree_ll and tree_d.
	bitlen_lld_e is to bitlen_lld what lz77_encoded is to data.
	bitlen_cl is to bitlen_lld_e what bitlen_lld is to lz77_encoded.
	*/

	unsigned BFINAL = final;
	size_t numcodes_ll, numcodes_d, i;
	unsigned HLIT, HDIST, HCLEN;

	uivector_init(&lz77_encoded);
	HuffmanTree_init(&tree_ll);
	HuffmanTree_init(&tree_d);
	HuffmanTree_init(&tree_cl);
	uivector_init(&frequencies_ll);
	uivector_init(&frequencies_d);
	uivector_init(&frequencies_cl);
	uivector_init(&bitlen_lld);
	uivector_init(&bitlen_lld_e);
	uivector_init(&bitlen_cl);

	/*This while loop never loops due to a break at the end, it is here to
	allow breaking out of it to the cleanup phase on error conditions.*/
	while (!error)
	{
		if (settings->use_lz77)
		{
			error = encodeLZ77(&lz77_encoded, hash, data, datapos, dataend, settings->windowsize,
							   settings->minmatch, settings->nicematch, settings->lazymatching);
			if (error)
				break;
		}
		else
		{
			if (!uivector_resize(&lz77_encoded, datasize))
				ERROR_BREAK(83);
			for (i = datapos; i < dataend; ++i)
				lz77_encoded.data[i - datapos] = data[i];
		}

		if (!uivector_resizev(&frequencies_ll, 286, 0))
			ERROR_BREAK(83);
		if (!uivector_resizev(&frequencies_d, 30, 0))
			ERROR_BREAK(83);

		for (i = 0; i != lz77_encoded.size; ++i)
		{
			unsigned symbol = lz77_encoded.data[i];
			++frequencies_ll.data[symbol];
			if (symbol > 256)
			{
				unsigned dist = lz77_encoded.data[i + 2];
				++frequencies_d.data[dist];
				i += 3;
			}
		}
		frequencies_ll.data[256] = 1;

		error = HuffmanTree_makeFromFrequencies(&tree_ll, frequencies_ll.data, 257, frequencies_ll.size, 15);
		if (error)
			break;

		error = HuffmanTree_makeFromFrequencies(&tree_d, frequencies_d.data, 2, frequencies_d.size, 15);
		if (error)
			break;

		numcodes_ll = tree_ll.numcodes;
		if (numcodes_ll > 286)
			numcodes_ll = 286;
		numcodes_d = tree_d.numcodes;
		if (numcodes_d > 30)
			numcodes_d = 30;

		for (i = 0; i != numcodes_ll; ++i)
			uivector_push_back(&bitlen_lld, HuffmanTree_getLength(&tree_ll, (unsigned)i));
		for (i = 0; i != numcodes_d; ++i)
			uivector_push_back(&bitlen_lld, HuffmanTree_getLength(&tree_d, (unsigned)i));

		/*run-length compress bitlen_ldd into bitlen_lld_e by using repeat codes 16 (copy length 3-6 times),
		17 (3-10 zeroes), 18 (11-138 zeroes)*/
		for (i = 0; i != (unsigned)bitlen_lld.size; ++i)
		{
			unsigned j = 0;
			while (i + j + 1 < (unsigned)bitlen_lld.size && bitlen_lld.data[i + j + 1] == bitlen_lld.data[i])
				++j;

			if (bitlen_lld.data[i] == 0 && j >= 2)
			{
				++j;
				if (j <= 10)
				{
					uivector_push_back(&bitlen_lld_e, 17);
					uivector_push_back(&bitlen_lld_e, j - 3);
				}
				else
				{
					if (j > 138)
						j = 138;
					uivector_push_back(&bitlen_lld_e, 18);
					uivector_push_back(&bitlen_lld_e, j - 11);
				}
				i += (j - 1);
			}
			else if (j >= 3)
			{
				size_t k;
				unsigned num = j / 6, rest = j % 6;
				uivector_push_back(&bitlen_lld_e, bitlen_lld.data[i]);
				for (k = 0; k < num; ++k)
				{
					uivector_push_back(&bitlen_lld_e, 16);
					uivector_push_back(&bitlen_lld_e, 6 - 3);
				}
				if (rest >= 3)
				{
					uivector_push_back(&bitlen_lld_e, 16);
					uivector_push_back(&bitlen_lld_e, rest - 3);
				}
				else
					j -= rest;
				i += j;
			}
			else
			{
				uivector_push_back(&bitlen_lld_e, bitlen_lld.data[i]);
			}
		}

		if (!uivector_resizev(&frequencies_cl, NUM_CODE_LENGTH_CODES, 0))
			ERROR_BREAK(83);
		for (i = 0; i != bitlen_lld_e.size; ++i)
		{
			++frequencies_cl.data[bitlen_lld_e.data[i]];
			/*after a repeat code come the bits that specify the number of repetitions,
			those don't need to be in the frequencies_cl calculation*/
			if (bitlen_lld_e.data[i] >= 16)
				++i;
		}

		error = HuffmanTree_makeFromFrequencies(&tree_cl, frequencies_cl.data,
												frequencies_cl.size, frequencies_cl.size, 7);
		if (error)
			break;

		if (!uivector_resize(&bitlen_cl, tree_cl.numcodes))
			ERROR_BREAK(83);
		for (i = 0; i != tree_cl.numcodes; ++i)
		{

			bitlen_cl.data[i] = HuffmanTree_getLength(&tree_cl, CLCL_ORDER[i]);
		}
		while (bitlen_cl.data[bitlen_cl.size - 1] == 0 && bitlen_cl.size > 4)
		{

			if (!uivector_resize(&bitlen_cl, bitlen_cl.size - 1))
				ERROR_BREAK(83);
		}
		if (error)
			break;

		/*
		Write everything into the output

		After the BFINAL and BTYPE, the dynamic block consists out of the following:
		- 5 bits HLIT, 5 bits HDIST, 4 bits HCLEN
		- (HCLEN+4)*3 bits code lengths of code length alphabet
		- HLIT + 257 code lenghts of lit/length alphabet (encoded using the code length
		  alphabet, + possible repetition codes 16, 17, 18)
		- HDIST + 1 code lengths of distance alphabet (encoded using the code length
		  alphabet, + possible repetition codes 16, 17, 18)
		- compressed data
		- 256 (end code)
		*/

		addBitToStream(out, bp, BFINAL);
		addBitToStream(out, bp, 0);
		addBitToStream(out, bp, 1);

		HLIT = (unsigned)(numcodes_ll - 257);
		HDIST = (unsigned)(numcodes_d - 1);
		HCLEN = (unsigned)bitlen_cl.size - 4;

		while (!bitlen_cl.data[HCLEN + 4 - 1] && HCLEN > 0)
			--HCLEN;
		addBitsToStream(bp, out, HLIT, 5);
		addBitsToStream(bp, out, HDIST, 5);
		addBitsToStream(bp, out, HCLEN, 4);

		for (i = 0; i != HCLEN + 4; ++i)
			addBitsToStream(bp, out, bitlen_cl.data[i], 3);

		for (i = 0; i != bitlen_lld_e.size; ++i)
		{
			addHuffmanSymbol(bp, out, HuffmanTree_getCode(&tree_cl, bitlen_lld_e.data[i]),
							 HuffmanTree_getLength(&tree_cl, bitlen_lld_e.data[i]));

			if (bitlen_lld_e.data[i] == 16)
				addBitsToStream(bp, out, bitlen_lld_e.data[++i], 2);
			else if (bitlen_lld_e.data[i] == 17)
				addBitsToStream(bp, out, bitlen_lld_e.data[++i], 3);
			else if (bitlen_lld_e.data[i] == 18)
				addBitsToStream(bp, out, bitlen_lld_e.data[++i], 7);
		}

		writeLZ77data(bp, out, &lz77_encoded, &tree_ll, &tree_d);

		if (HuffmanTree_getLength(&tree_ll, 256) == 0)
			ERROR_BREAK(64);

		addHuffmanSymbol(bp, out, HuffmanTree_getCode(&tree_ll, 256), HuffmanTree_getLength(&tree_ll, 256));

		break;
	}

	uivector_cleanup(&lz77_encoded);
	HuffmanTree_cleanup(&tree_ll);
	HuffmanTree_cleanup(&tree_d);
	HuffmanTree_cleanup(&tree_cl);
	uivector_cleanup(&frequencies_ll);
	uivector_cleanup(&frequencies_d);
	uivector_cleanup(&frequencies_cl);
	uivector_cleanup(&bitlen_lld_e);
	uivector_cleanup(&bitlen_lld);
	uivector_cleanup(&bitlen_cl);

	return error;
}

static unsigned deflateFixed(ucvector *out, size_t *bp, Hash *hash,
							 const unsigned char *data,
							 size_t datapos, size_t dataend,
							 const LodePNGCompressSettings *settings, unsigned final)
{
	HuffmanTree tree_ll;
	HuffmanTree tree_d;

	unsigned BFINAL = final;
	unsigned error = 0;
	size_t i;

	HuffmanTree_init(&tree_ll);
	HuffmanTree_init(&tree_d);

	generateFixedLitLenTree(&tree_ll);
	generateFixedDistanceTree(&tree_d);

	addBitToStream(out, bp, BFINAL);
	addBitToStream(out, bp, 1);
	addBitToStream(out, bp, 0);

	if (settings->use_lz77)
	{
		uivector lz77_encoded;
		uivector_init(&lz77_encoded);
		error = encodeLZ77(&lz77_encoded, hash, data, datapos, dataend, settings->windowsize,
						   settings->minmatch, settings->nicematch, settings->lazymatching);
		if (!error)
			writeLZ77data(bp, out, &lz77_encoded, &tree_ll, &tree_d);
		uivector_cleanup(&lz77_encoded);
	}
	else
	{
		for (i = datapos; i < dataend; ++i)
		{
			addHuffmanSymbol(bp, out, HuffmanTree_getCode(&tree_ll, data[i]), HuffmanTree_getLength(&tree_ll, data[i]));
		}
	}

	if (!error)
		addHuffmanSymbol(bp, out, HuffmanTree_getCode(&tree_ll, 256), HuffmanTree_getLength(&tree_ll, 256));

	HuffmanTree_cleanup(&tree_ll);
	HuffmanTree_cleanup(&tree_d);

	return error;
}

static unsigned lodepng_deflatev(ucvector *out, const unsigned char *in, size_t insize,
								 const LodePNGCompressSettings *settings)
{
	unsigned error = 0;
	size_t i, blocksize, numdeflateblocks;
	size_t bp = 0;
	Hash hash;

	if (settings->btype > 2)
		return 61;
	else if (settings->btype == 0)
		return deflateNoCompression(out, in, insize);
	else if (settings->btype == 1)
		blocksize = insize;
	else
	{

		blocksize = insize / 8 + 8;
		if (blocksize < 65536)
			blocksize = 65536;
		if (blocksize > 262144)
			blocksize = 262144;
	}

	numdeflateblocks = (insize + blocksize - 1) / blocksize;
	if (numdeflateblocks == 0)
		numdeflateblocks = 1;

	error = hash_init(&hash, settings->windowsize);
	if (error)
		return error;

	for (i = 0; i != numdeflateblocks && !error; ++i)
	{
		unsigned final = (i == numdeflateblocks - 1);
		size_t start = i * blocksize;
		size_t end = start + blocksize;
		if (end > insize)
			end = insize;

		if (settings->btype == 1)
			error = deflateFixed(out, &bp, &hash, in, start, end, settings, final);
		else if (settings->btype == 2)
			error = deflateDynamic(out, &bp, &hash, in, start, end, settings, final);
	}

	hash_cleanup(&hash);

	return error;
}

static inline unsigned lodepng_deflate(unsigned char **out, size_t *outsize,
									   const unsigned char *in, size_t insize,
									   const LodePNGCompressSettings *settings)
{
	unsigned error;
	ucvector v;
	ucvector_init_buffer(&v, *out, *outsize);
	error = lodepng_deflatev(&v, in, insize, settings);
	*out = v.data;
	*outsize = v.size;
	return error;
}

static unsigned deflate(unsigned char **out, size_t *outsize,
						const unsigned char *in, size_t insize,
						const LodePNGCompressSettings *settings)
{
	if (settings->custom_deflate)
	{
		return settings->custom_deflate(out, outsize, in, insize, settings);
	}
	else
	{
		return lodepng_deflate(out, outsize, in, insize, settings);
	}
}

static inline void generateFixedLitLenTree(HuffmanTree *tree)
{
	unsigned i;
	tree->lengths = (unsigned *)lodepng_malloc(286 * sizeof(unsigned));
	tree->numcodes = 286;
	for (i = 0; i <= 143; ++i)
		tree->lengths[i] = 8;
	for (i = 144; i <= 279; ++i)
		tree->lengths[i] = 9;
	for (i = 280; i <= 287; ++i)
		tree->lengths[i] = 8;
	HuffmanTree_makeFromLengths2(tree);
}

static inline void generateFixedDistanceTree(HuffmanTree *tree)
{
	unsigned i;
	tree->lengths = (unsigned *)lodepng_malloc(30 * sizeof(unsigned));
	tree->numcodes = 30;
	for (i = 0; i < 30; ++i)
		tree->lengths[i] = 5;
	HuffmanTree_makeFromLengths2(tree);
}

static inline void getTreeInflateFixed(HuffmanTree *tree_ll, HuffmanTree *tree_d)
{
	generateFixedLitLenTree(tree_ll);
	generateFixedDistanceTree(tree_d);
}

#endif