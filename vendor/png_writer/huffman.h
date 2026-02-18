/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   huffman.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:02:18 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:02:18 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "types.h"
#include "bpm.h"

#ifndef NUM_CODE_LENGTH_CODES
#define NUM_CODE_LENGTH_CODES 19
#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285
static const unsigned LENGTHBASE[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
static const unsigned LENGTHEXTRA[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
static const unsigned DISTANCEBASE[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static const unsigned DISTANCEEXTRA[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
#endif

#ifndef HUFFMAN_TREE_DEFINED
typedef struct s_huffman_tree
{
	unsigned *tree2d;
	unsigned *tree1d;
	unsigned *lengths;
	unsigned max_bit_len;
	unsigned num_codes;
} HuffmanTree;
#endif

/* Forward declarations needed by getTreeInflateDynamic (avoid implicit decls) */
static inline void HuffmanTree_init(HuffmanTree *tree);
static inline void HuffmanTree_cleanup(HuffmanTree *tree);
static inline unsigned HuffmanTree_makeFromLengths(HuffmanTree *tree, const unsigned *bitlen,
												   size_t numcodes, unsigned maxbitlen);
static inline unsigned huffmanDecodeSymbol(const unsigned char *in, size_t *bp,
										   const HuffmanTree *codetree, size_t inbitlength);

/* Forward declarations */
static inline void getTreeInflateFixed(HuffmanTree *tree_ll, HuffmanTree *tree_d);

static inline unsigned getTreeInflateDynamic(HuffmanTree *tree_ll, HuffmanTree *tree_d,
											 const unsigned char *in, size_t *bp, size_t inlength)
{
	/* dynamic tree parsing per RFC1951 */
	static const unsigned CLCL_ORDER[NUM_CODE_LENGTH_CODES] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
	unsigned HLIT, HDIST, HCLEN;
	unsigned bitlen_cl[NUM_CODE_LENGTH_CODES];
	unsigned bitlen_ll[286], bitlen_d[30];
	unsigned error = 0, i;

	if ((*bp + 14) > inlength * 8)
		return 49;

	HLIT = readBitsFromStream(bp, in, 5) + 257; /* # of literal/length codes */
	HDIST = readBitsFromStream(bp, in, 5) + 1;	/* # of distance codes */
	HCLEN = readBitsFromStream(bp, in, 4) + 4;	/* # of code-length codes */

	for (i = 0; i < NUM_CODE_LENGTH_CODES; ++i)
		bitlen_cl[i] = 0;
	for (i = 0; i < HCLEN; ++i)
		bitlen_cl[CLCL_ORDER[i]] = readBitsFromStream(bp, in, 3);

	HuffmanTree tree_cl;
	HuffmanTree_init(&tree_cl);
	error = HuffmanTree_makeFromLengths(&tree_cl, bitlen_cl, NUM_CODE_LENGTH_CODES, 7);
	if (error)
	{
		HuffmanTree_cleanup(&tree_cl);
		return error;
	}

	/* read code lengths for ll and d trees */
	{
		unsigned temp[286 + 30];
		unsigned num = HLIT + HDIST, idx = 0;
		while (idx < num)
		{
			unsigned code = huffmanDecodeSymbol(in, bp, &tree_cl, inlength * 8);
			if (code <= 15)
			{
				temp[idx++] = code;
			}
			else if (code == 16)
			{
				if (idx == 0)
				{
					error = 54;
					break;
				}
				unsigned repeat = readBitsFromStream(bp, in, 2) + 3;
				unsigned prev = temp[idx - 1];
				while (repeat-- && idx < num)
					temp[idx++] = prev;
			}
			else if (code == 17)
			{
				unsigned repeat = readBitsFromStream(bp, in, 3) + 3;
				while (repeat-- && idx < num)
					temp[idx++] = 0;
			}
			else if (code == 18)
			{
				unsigned repeat = readBitsFromStream(bp, in, 7) + 11;
				while (repeat-- && idx < num)
					temp[idx++] = 0;
			}
			else
			{
				error = 16;
				break;
			}
		}

		if (!error)
		{
			for (i = 0; i < HLIT; ++i)
				bitlen_ll[i] = temp[i];
			for (i = 0; i < HDIST; ++i)
				bitlen_d[i] = temp[HLIT + i];
		}
	}

	HuffmanTree_cleanup(&tree_cl);
	if (error)
		return error;

	error = HuffmanTree_makeFromLengths(tree_ll, bitlen_ll, HLIT, 15);
	if (error)
		return error;
	return HuffmanTree_makeFromLengths(tree_d, bitlen_d, HDIST, 15);
}

static inline void HuffmanTree_init(HuffmanTree *tree)
{
	tree->tree2d = 0;
	tree->tree1d = 0;
	tree->lengths = 0;
}

static inline void HuffmanTree_cleanup(HuffmanTree *tree)
{
	lodepng_free(tree->tree2d);
	lodepng_free(tree->tree1d);
	lodepng_free(tree->lengths);
}

static inline unsigned HuffmanTree_make2DTree(HuffmanTree *tree)
{
	unsigned nodefilled = 0;
	unsigned treepos = 0;
	unsigned n, i;

	tree->tree2d = (unsigned *)lodepng_malloc(tree->numcodes * 2 * sizeof(unsigned));
	if (!tree->tree2d)
		return 83;

	/*
	convert tree1d[] to tree2d[][]. In the 2D array, a value of 32767 means
	uninited, a value >= numcodes is an address to another bit, a value < numcodes
	is a code. The 2 rows are the 2 possible bit values (0 or 1), there are as
	many columns as codes - 1.
	A good huffman tree has N * 2 - 1 nodes, of which N - 1 are internal nodes.
	Here, the internal nodes are stored (what their 0 and 1 option point to).
	There is only memory for such good tree currently, if there are more nodes
	(due to too long length codes), error 55 will happen
	*/
	for (n = 0; n < tree->numcodes * 2; ++n)
	{
		tree->tree2d[n] = 32767;
	}

	for (n = 0; n < tree->numcodes; ++n)
	{
		for (i = 0; i != tree->lengths[n]; ++i)
		{
			unsigned char bit = (unsigned char)((tree->tree1d[n] >> (tree->lengths[n] - i - 1)) & 1);

			if (treepos > 2147483647 || treepos + 2 > tree->numcodes)
				return 55;
			if (tree->tree2d[2 * treepos + bit] == 32767)
			{
				if (i + 1 == tree->lengths[n])
				{
					tree->tree2d[2 * treepos + bit] = n;
					treepos = 0;
				}
				else
				{
					/*put address of the next step in here, first that address has to be found of course
					(it's just nodefilled + 1)...*/
					++nodefilled;

					tree->tree2d[2 * treepos + bit] = nodefilled + tree->numcodes;
					treepos = nodefilled;
				}
			}
			else
				treepos = tree->tree2d[2 * treepos + bit] - tree->numcodes;
		}
	}

	for (n = 0; n < tree->numcodes * 2; ++n)
	{
		if (tree->tree2d[n] == 32767)
			tree->tree2d[n] = 0;
	}

	return 0;
}

/*
Second step for the ...makeFromLengths and ...makeFromFrequencies functions.
numcodes, lengths and maxbitlen must already be filled in correctly. return
value is error.
*/
static inline unsigned HuffmanTree_makeFromLengths2(HuffmanTree *tree)
{
	uivector blcount;
	uivector nextcode;
	unsigned error = 0;
	unsigned bits, n;

	uivector_init(&blcount);
	uivector_init(&nextcode);

	tree->tree1d = (unsigned *)lodepng_malloc(tree->numcodes * sizeof(unsigned));
	if (!tree->tree1d)
		error = 83;

	if (!uivector_resizev(&blcount, tree->max_bit_len + 1, 0) || !uivector_resizev(&nextcode, tree->max_bit_len + 1, 0))
		error = 83;

	if (!error)
	{

		for (bits = 0; bits != tree->numcodes; ++bits)
			++blcount.data[tree->lengths[bits]];

		for (bits = 1; bits <= tree->max_bit_len; ++bits)
		{
			nextcode.data[bits] = (nextcode.data[bits - 1] + blcount.data[bits - 1]) << 1;
		}

		for (n = 0; n != tree->numcodes; ++n)
		{
			if (tree->lengths[n] != 0)
				tree->tree1d[n] = nextcode.data[tree->lengths[n]]++;
		}
	}

	uivector_cleanup(&blcount);
	uivector_cleanup(&nextcode);

	if (!error)
		return HuffmanTree_make2DTree(tree);
	else
		return error;
}

/*
given the code lengths (as stored in the PNG file), generate the tree as defined
by Deflate. maxbitlen is the maximum bits that a code in the tree can have.
return value is error.
*/
static inline unsigned HuffmanTree_makeFromLengths(HuffmanTree *tree, const unsigned *bitlen,
												   size_t numcodes, unsigned maxbitlen)
{
	unsigned i;
	tree->lengths = (unsigned *)lodepng_malloc(numcodes * sizeof(unsigned));
	if (!tree->lengths)
		return 83;
	for (i = 0; i != numcodes; ++i)
		tree->lengths[i] = bitlen[i];
	tree->numcodes = (unsigned)numcodes;
	tree->max_bit_len = maxbitlen;
	return HuffmanTree_makeFromLengths2(tree);
}

static inline unsigned lodepng_huffman_code_lengths(unsigned *lengths, const unsigned *frequencies,
													size_t numcodes, unsigned maxbitlen)
{
	unsigned error = 0;
	unsigned i;
	size_t numpresent = 0;
	BPMNode *leaves;

	if (numcodes == 0)
		return 80;
	if ((1u << maxbitlen) < (unsigned)numcodes)
		return 80;

	leaves = (BPMNode *)lodepng_malloc(numcodes * sizeof(*leaves));
	if (!leaves)
		return 83;

	for (i = 0; i != numcodes; ++i)
	{
		if (frequencies[i] > 0)
		{
			leaves[numpresent].weight = (int)frequencies[i];
			leaves[numpresent].index = i;
			++numpresent;
		}
	}

	for (i = 0; i != numcodes; ++i)
		lengths[i] = 0;

	/*ensure at least two present symbols. There should be at least one symbol
	according to RFC 1951 section 3.2.7. Some decoders incorrectly require two. To
	make these work as well ensure there are at least two symbols. The
	Package-Merge code below also doesn't work correctly if there's only one
	symbol, it'd give it the theoritical 0 bits but in practice zlib wants 1 bit*/
	if (numpresent == 0)
	{
		lengths[0] = lengths[1] = 1;
	}
	else if (numpresent == 1)
	{
		lengths[leaves[0].index] = 1;
		lengths[leaves[0].index == 0 ? 1 : 0] = 1;
	}
	else
	{
		BPMLists lists;
		BPMNode *node;

		bpmnode_sort(leaves, numpresent);

		lists.listsize = maxbitlen;
		lists.memsize = 2 * maxbitlen * (maxbitlen + 1);
		lists.nextfree = 0;
		lists.numfree = lists.memsize;
		lists.memory = (BPMNode *)lodepng_malloc(lists.memsize * sizeof(*lists.memory));
		lists.freelist = (BPMNode **)lodepng_malloc(lists.memsize * sizeof(BPMNode *));
		lists.chains0 = (BPMNode **)lodepng_malloc(lists.listsize * sizeof(BPMNode *));
		lists.chains1 = (BPMNode **)lodepng_malloc(lists.listsize * sizeof(BPMNode *));
		if (!lists.memory || !lists.freelist || !lists.chains0 || !lists.chains1)
			error = 83;

		if (!error)
		{
			for (i = 0; i != lists.memsize; ++i)
				lists.freelist[i] = &lists.memory[i];

			bpmnode_create(&lists, leaves[0].weight, 1, 0);
			bpmnode_create(&lists, leaves[1].weight, 2, 0);

			for (i = 0; i != lists.listsize; ++i)
			{
				lists.chains0[i] = &lists.memory[0];
				lists.chains1[i] = &lists.memory[1];
			}

			for (i = 2; i != 2 * numpresent - 2; ++i)
				boundaryPM(&lists, leaves, numpresent, (int)maxbitlen - 1, (int)i);

			for (node = lists.chains1[maxbitlen - 1]; node; node = node->tail)
			{
				for (i = 0; i != node->index; ++i)
					++lengths[leaves[i].index];
			}
		}

		lodepng_free(lists.memory);
		lodepng_free(lists.freelist);
		lodepng_free(lists.chains0);
		lodepng_free(lists.chains1);
	}

	lodepng_free(leaves);
	return error;
}

static inline unsigned HuffmanTree_makeFromFrequencies(HuffmanTree *tree, const unsigned *frequencies,
													   size_t mincodes, size_t numcodes, unsigned maxbitlen)
{
	unsigned error = 0;
	while (!frequencies[numcodes - 1] && numcodes > mincodes)
		--numcodes;
	tree->max_bit_len = maxbitlen;
	tree->numcodes = (unsigned)numcodes;
	tree->lengths = (unsigned *)lodepng_realloc(tree->lengths, numcodes * sizeof(unsigned));
	if (!tree->lengths)
		return 83;

	memset(tree->lengths, 0, numcodes * sizeof(unsigned));

	error = lodepng_huffman_code_lengths(tree->lengths, frequencies, numcodes, maxbitlen);
	if (!error)
		error = HuffmanTree_makeFromLengths2(tree);
	return error;
}

static inline unsigned HuffmanTree_getCode(const HuffmanTree *tree, unsigned index)
{
	return tree->tree1d[index];
}

static inline unsigned HuffmanTree_getLength(const HuffmanTree *tree, unsigned index)
{
	return tree->lengths[index];
}

/*
returns the code, or (unsigned)(-1) if error happened
inbitlength is the length of the complete buffer, in bits (so its byte length times 8)
*/
static inline unsigned huffmanDecodeSymbol(const unsigned char *in, size_t *bp,
										   const HuffmanTree *codetree, size_t inbitlength)
{
	unsigned treepos = 0, ct;
	for (;;)
	{
		if (*bp >= inbitlength)
			return (unsigned)(-1);
		/*
		decode the symbol from the tree. The "readBitFromStream" code is inlined in
		the expression below because this is the biggest bottleneck while decoding
		*/
		ct = codetree->tree2d[(treepos << 1) + READBIT(*bp, in)];
		++(*bp);
		if (ct < codetree->numcodes)
			return ct;
		else
			treepos = ct - codetree->numcodes;

		if (treepos >= codetree->numcodes)
			return (unsigned)(-1);
	}
}

static inline unsigned inflateHuffmanBlock(ucvector *out, const unsigned char *in, size_t *bp,
										   size_t *pos, size_t inlength, unsigned btype)
{
	unsigned error = 0;
	HuffmanTree tree_ll;
	HuffmanTree tree_d;
	size_t inbitlength = inlength * 8;

	HuffmanTree_init(&tree_ll);
	HuffmanTree_init(&tree_d);

	if (btype == 1)
		getTreeInflateFixed(&tree_ll, &tree_d);
	else if (btype == 2)
		error = getTreeInflateDynamic(&tree_ll, &tree_d, in, bp, inlength);

	while (!error)
	{

		unsigned code_ll = huffmanDecodeSymbol(in, bp, &tree_ll, inbitlength);
		if (code_ll <= 255)
		{

			if (!ucvector_resize(out, (*pos) + 1))
				ERROR_BREAK(83);
			out->data[*pos] = (unsigned char)code_ll;
			++(*pos);
		}
		else if (code_ll >= FIRST_LENGTH_CODE_INDEX && code_ll <= LAST_LENGTH_CODE_INDEX)
		{
			unsigned code_d, distance;
			unsigned numextrabits_l, numextrabits_d;
			size_t start, forward, backward, length;

			length = LENGTHBASE[code_ll - FIRST_LENGTH_CODE_INDEX];

			numextrabits_l = LENGTHEXTRA[code_ll - FIRST_LENGTH_CODE_INDEX];
			if ((*bp + numextrabits_l) > inbitlength)
				ERROR_BREAK(51);
			length += readBitsFromStream(bp, in, numextrabits_l);

			code_d = huffmanDecodeSymbol(in, bp, &tree_d, inbitlength);
			if (code_d > 29)
			{
				if (code_d == (unsigned)(-1))
				{
					/*return error code 10 or 11 depending on the situation that happened in huffmanDecodeSymbol
					(10=no endcode, 11=wrong jump outside of tree)*/
					error = (*bp) > inlength * 8 ? 10 : 11;
				}
				else
					error = 18;
				break;
			}
			distance = DISTANCEBASE[code_d];

			numextrabits_d = DISTANCEEXTRA[code_d];
			if ((*bp + numextrabits_d) > inbitlength)
				ERROR_BREAK(51);
			distance += readBitsFromStream(bp, in, numextrabits_d);

			start = (*pos);
			if (distance > start)
				ERROR_BREAK(52);
			backward = start - distance;

			if (!ucvector_resize(out, (*pos) + length))
				ERROR_BREAK(83);
			if (distance < length)
			{
				for (forward = 0; forward < length; ++forward)
				{
					out->data[(*pos)++] = out->data[backward++];
				}
			}
			else
			{
				memcpy(out->data + *pos, out->data + backward, length);
				*pos += length;
			}
		}
		else if (code_ll == 256)
		{
			break;
		}
		else
		{
			/*return error code 10 or 11 depending on the situation that happened in huffmanDecodeSymbol
			(10=no endcode, 11=wrong jump outside of tree)*/
			error = ((*bp) > inlength * 8) ? 10 : 11;
			break;
		}
	}

	HuffmanTree_cleanup(&tree_ll);
	HuffmanTree_cleanup(&tree_d);

	return error;
}

static void addHuffmanSymbol(size_t *bp, ucvector *compressed, unsigned code, unsigned bitlen)
{
	addBitsToStreamReversed(bp, compressed, code, bitlen);
}

#endif